
/*
 * Teensy 4.1 Telemetry Control Unit code
 * Written by Soohyun Kim, with assistance by Ryan Gallaway and Nathan Cheek. 
 * 
 * Rev 2 - 4/23/2019
 * Last Modified: 9/29/2022
 */

 /*
  * TODO: GPS
  */
#define GPS_EN false
#define COULOUMB_COUNTING_EN false

#define CIRCULAR_BUFFER_INT_SAFE
#include "CircularBuffer.h"

#include <SD.h>
//#include <MTP_Teensy.h>
#include <FlexCAN_T4.h>
#include <HyTech_CAN.h>
#include <kinetis_flexcan.h>
#include <Wire.h>
#include <TimeLib.h>
#include <Time.h>
#include <Metro.h>
#include <XBTools.h>
#include <Adafruit_GPS.h>
#include <ADC_SPI.h>


#define NUM_BMS_IC 12
#define SER_PKT_LEN 13
#define TEMP_SENSE_CHANNEL 0
#define ECU_CURRENT_CHANNEL 1
#define SUPPLY_READ_CHANNEL 2
#define COOLING_CURRENT_CHANNEL 3
#define ALPHA 0.9772                     // parameter for the sowftware filter used on ADC pedal channels

#define CAN_MSG_Q_SZ 512
#define SD_BUFF_SZ 4096

/*
 * Serial larger buffer
 */
#define NRF Serial2
#define ESP Serial8
#define SER_BUF_SZ 4096
uint8_t NRFbuffer[SER_BUF_SZ];
uint8_t ESPbuffer[SER_BUF_SZ];

/*
 * Variables to store filtered values from ADC channels
 */
ADC_SPI ADC(A1, 1800000);
float filtered_temp_reading{};
float filtered_ecu_current_reading{};
float filtered_supply_reading{};
float filtered_cooling_current_reading{};

/*
 * CAN Variables
 */
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> CAN_1;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> CAN_2;
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> CAN_3;
static CAN_message_t msg_rx;
static CAN_message_t msg_tx;
static CAN_message_t xb_msg;
typedef struct CAN_msg_time {
  CAN_message_t msg;
  uint64_t time = 0;
} CAN_msg_time;
CircularBuffer<CAN_msg_time, CAN_MSG_Q_SZ> CAN_msg_q;


/*
 * SD Card Variables
 */
typedef struct SD_write_buf {
  unsigned int size = 0;
  uint8_t buffer[SD_BUFF_SZ];
} SD_write_buf;
SD_write_buf SD_buf_1;
SD_write_buf SD_buf_2;
SD_write_buf *incoming_buf = &SD_buf_1;
SD_write_buf *current_write_buf = &SD_buf_2;

File logger;

/*
 * Perf counters and debug
 */
typedef struct perf_counters {
  uint16_t CAN_1_freq = 0;
  uint16_t CAN_2_freq = 0;
  uint16_t CAN_3_freq = 0;
  uint16_t GPS_freq = 0;
  uint32_t loops = 0;
  uint32_t bytes_written = 0;
  uint16_t messages_queued = 0;
  unsigned long max_loop_latency = 0;
  uint16_t slow_loops = 0;
  uint16_t slow_loop_time = 0;
} perf_counters;
perf_counters counters;
Metro timer_debug_RTC = Metro(1000);



Metro timer_mcu_status = Metro(2000);
Metro timer_debug_dashboard = Metro(2000);
Metro timer_debug_mcu_pedal_readings = Metro(200);
Metro timer_debug_bms_balancing_status = Metro(3000);
Metro timer_mcu_analog_readings = Metro(500);
Metro timer_debug_bms_status = Metro(1000);
Metro timer_debug_bms_temperatures = Metro(3000);
Metro timer_debug_bms_detailed_temperatures = Metro(3000);
Metro timer_debug_bms_voltages = Metro(1000);
Metro timer_debug_bms_detailed_voltages = Metro(3000);
Metro timer_energy = Metro(1000);
Metro timer_setpoints_command = Metro(1000);
Metro timer_status = Metro(1000);
Metro timer_temps = Metro(1000);
Metro timer_detailed_voltages = Metro(1000);
Metro timer_status_send = Metro(100);
Metro timer_status_send_xbee = Metro(2000);
Metro timer_gps = Metro(100);
Metro timer_flush = Metro(100);
#if COULOUMB_COUNTING_EN
Metro timer_total_discharge = Metro(1000);
Metro timer_debug_bms_coulomb_counts = Metro(1000);
#endif
Metro timer_em_status = Metro(1000);
Metro timer_em_measurement = Metro(1000);
Metro timer_imu_accelerometer = Metro(200);
Metro timer_imu_gyroscope = Metro(200);
Metro timer_sab_readings_front = Metro(200);
Metro timer_sab_readings_rear = Metro(200);
Metro timer_sab_readings_gps = Metro(200);

MCU_status mcu_status;
MCU_pedal_readings mcu_pedal_readings;
MCU_analog_readings mcu_analog_readings;
MCU_GPS_readings mcu_gps_readings;

MC_energy mc1_energy;
MC_energy mc2_energy;
MC_energy mc3_energy;
MC_energy mc4_energy;
MC_setpoints_command mc1_setpoints_command;
MC_setpoints_command mc2_setpoints_command;
MC_setpoints_command mc3_setpoints_command;
MC_setpoints_command mc4_setpoints_command;
MC_status mc1_status;
MC_status mc2_status;
MC_status mc3_status;
MC_status mc4_status;
MC_temps mc1_temps;
MC_temps mc2_temps;
MC_temps mc3_temps;
MC_temps mc4_temps;

BMS_voltages bms_voltages;
BMS_detailed_voltages bms_detailed_voltages[4][NUM_BMS_IC];
BMS_temperatures bms_temperatures;
BMS_detailed_temperatures bms_detailed_temperatures[2][NUM_BMS_IC];
BMS_onboard_temperatures bms_onboard_temperatures;
BMS_onboard_detailed_temperatures bms_onboard_detailed_temperatures[NUM_BMS_IC]; // Not used
BMS_status bms_status;
BMS_balancing_status bms_balancing_status[2];
BMS_coulomb_counts bms_coulomb_counts;                                                
CCU_status ccu_status;
Dashboard_status dashboard_status;
uint32_t total_discharge;
unsigned long previous_data_time;
EM_status em_status;
EM_measurement em_measurement;
IMU_accelerometer imu_accelerometer;
IMU_gyroscope imu_gyroscope;
SAB_readings_front sab_readings_front;
SAB_readings_rear sab_readings_rear;
SAB_readings_gps sab_readings_gps;

void parse_can_lines();
void parse_can_message(int can_read_result);
void write_to_SD(CAN_message_t *msg);
time_t getTeensy3Time();
void process_current();
void process_glv_voltage();
void setup_total_discharge();
void process_total_discharge();
void write_total_discharge();
int write_xbee_data();
void send_xbee();
void sd_date_time(uint16_t* date, uint16_t* time);

void setup() {
    //MTP.begin();
    setupSD();
    //MTP.addFilesystem(SD, "SD Card");
    
    /* Set up Serial, XBee and CAN */
    Serial.begin(115200);
    NRF.begin(1000000);
    NRF.addMemoryForWrite(NRFbuffer, SER_BUF_SZ);
    ESP.begin(1000000);
    ESP.addMemoryForWrite(ESPbuffer, SER_BUF_SZ);

    setupClock();

    setupGPS();

    createFile();

    //FLEXCAN0_MCR &= 0xFFFDFFFF; // Enables CAN message self-reception
    CAN_1.begin();
    CAN_1.setBaudRate(500000);
    CAN_2.begin();
    CAN_2.setBaudRate(500000);
    CAN_3.begin();
    CAN_3.setBaudRate(500000);
    CAN_1.enableMBInterrupts();
    CAN_2.enableMBInterrupts();
    CAN_3.enableMBInterrupts();
    CAN_1.onReceive(parse_can1_message);
    CAN_2.onReceive(parse_can2_message);
    CAN_3.onReceive(parse_can3_message);
    
    Serial.println("TCU On");
}

Metro telem = Metro(1000);

void loop() {
    unsigned long before = millis();
    CAN_1.events();
    CAN_2.events();
    CAN_3.events();
    gpsLoop();
    //MTP.loop();
    /* Process and log incoming CAN messages */
    //parse_can_lines();
    //read_analog_values();
    //readESP();
    /* Send messages over XBee */
    send_xbee();
    parse_can_q();
    
    //write_buf_to_SD(current_write_buf);
    /* Flush data to SD card occasionally */
    if (telem.check()) { //1000
      print_voltages_temps();
      print_temps();
    }
    if (timer_flush.check()) { //100 
        logger.flush(); // Flush data to disk (data is also flushed whenever the 512 Byte buffer fills up, but this call ensures we don't lose more than a second of data when the car turns off)
    }
    /* Print timestamp to serial occasionally */
    if (timer_debug_RTC.check()) { //1000
        Serial.printf("Clock: %u\n", Teensy3Clock.get());
        Serial.printf("CAN1: %u, CAN2: %u, CAN3: %u, GPS: %u, Loops: %lu\n",counters.CAN_1_freq, counters.CAN_2_freq, counters.CAN_3_freq, counters.GPS_freq, counters.loops);
        Serial.printf("Messages written: %lu Messages queued: %u\n", counters.bytes_written, counters.messages_queued);
        Serial.printf("Max loop latency: %lu\n", counters.max_loop_latency);
        Serial.printf("Slow loops: %u Slow loop time: %u\n", counters.slow_loops, counters.slow_loop_time);
        Serial.println();
        counters = (perf_counters){
          .CAN_1_freq = 0, 
          .CAN_2_freq = 0, 
          .CAN_3_freq = 0, 
          .GPS_freq = 0, 
          .loops = 0,
          .bytes_written = 0,
          .messages_queued = 0,
          .max_loop_latency = 0,
          .slow_loops = 0,
          .slow_loop_time = 0
          };
    }
    /* Process MCU analog readings */
    if (timer_mcu_analog_readings.check()) { //500
        process_mcu_analog_readings();
    }
    /* Couloumb counting */
    #if COULOUMB_COUNTING_EN
    if (timer_total_discharge.check()) {
        write_total_discharge();
    }
    #endif
    counters.loops++;
    unsigned long after = millis();
    if (after-before > counters.max_loop_latency) {
      counters.max_loop_latency = after-before;
    }
    if (after-before > 1) {
      counters.slow_loops++;
      counters.slow_loop_time += after-before;
    }
}



time_t getTeensy3Time() {
    return Teensy3Clock.get();
}

inline void read_analog_values() {
    /* Filter ADC readings */
    filtered_temp_reading        = ALPHA * filtered_temp_reading      + (1 - ALPHA) * ADC.read_channel(TEMP_SENSE_CHANNEL);
    filtered_ecu_current_reading   = ALPHA * filtered_ecu_current_reading   + (1 - ALPHA) * ADC.read_channel(ECU_CURRENT_CHANNEL);
    filtered_supply_reading      = ALPHA * filtered_supply_reading      + (1 - ALPHA) * ADC.read_channel(SUPPLY_READ_CHANNEL);
    filtered_cooling_current_reading = ALPHA * filtered_cooling_current_reading + (1 - ALPHA) * ADC.read_channel(COOLING_CURRENT_CHANNEL);
}
void process_mcu_analog_readings() {
    //self derived
    float current_ecu = (filtered_ecu_current_reading - 2048) / 151.0;
    float current_cooling = (filtered_cooling_current_reading - 2048) / 151.0;
    //Serial.println(current_cooling);
    //Serial.println(current_ecu);
    float glv_voltage_value = (((filtered_supply_reading/4096) * 5) * 55/12) + 0.14; //ADC->12V conversion + offset likely due to resistor values
    //Serial.print("GLV: ");
    //Serial.println(glv_voltage_value);
    //mcu_analog_readings.set_ecu_current(current_ecu * 5000);
    //mcu_analog_readings.set_cooling_current(current_cooling * 5000);
    //mcu_analog_readings.set_glv_battery_voltage(glv_voltage_value * 2500);
    //mcu_analog_readings.set_temperature(filtered_temp_reading);
    mcu_analog_readings.write(msg_tx.buf);
    msg_tx.id = ID_MCU_ANALOG_READINGS;
    msg_tx.len = sizeof(mcu_analog_readings);
    //CAN_1.write(msg_tx);
    mcu_analog_readings.write(xb_msg.buf);
    xb_msg.id = ID_MCU_ANALOG_READINGS;
    xb_msg.len = sizeof(mcu_analog_readings);
    write_xbee_data();   
}
void setup_total_discharge() {
  total_discharge = 0;
  previous_data_time = millis();
  bms_coulomb_counts.set_total_discharge(total_discharge);
}

/* Coloumb counting */
#if COULOUMB_COUNTING_EN
void process_total_discharge() {
  double new_current = mc_current_information.get_dc_bus_current() / 10;
  unsigned long current_time = millis();
  uint32_t added_Ah = new_current * ((current_time - previous_data_time) * 10000 / (1000 * 60 * 60 )); //scaled by 10000 for telemetry parsing
  previous_data_time = current_time;
  total_discharge += added_Ah;
  bms_coulomb_counts.set_total_discharge(total_discharge);
}
void write_total_discharge() {
  bms_coulomb_counts.write(msg_tx.buf);
  msg_tx.id = ID_BMS_COULOMB_COUNTS;
  msg_tx.len = sizeof(bms_coulomb_counts);
  CAN.write(msg_tx);
}
#endif

void sd_date_time(uint16_t* date, uint16_t* time) {
    // return date using FAT_DATE macro to format fields
    *date = FAT_DATE(year(), month(), day());
    // return time using FAT_TIME macro to format fields
    *time = FAT_TIME(hour(), minute(), second());
}
