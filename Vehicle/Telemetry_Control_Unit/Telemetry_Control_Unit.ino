
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

#include <SD.h>
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

#define NRF Serial2
#define ESP Serial8

#define NUM_BMS_IC 12
#define SER_PKT_LEN 13
#define TEMP_SENSE_CHANNEL 0
#define ECU_CURRENT_CHANNEL 1
#define SUPPLY_READ_CHANNEL 2
#define COOLING_CURRENT_CHANNEL 3
#define ALPHA 0.9772                     // parameter for the sowftware filter used on ADC pedal channels

/*
 * Variables to store filtered values from ADC channels
 */
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

File logger;

/*
 * Variables to help with time calculation
 */
uint64_t global_ms_offset = 0;
uint64_t last_sec_epoch;

ADC_SPI ADC(A1, 1800000);
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
Metro timer_debug_RTC = Metro(1000);
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
    delay(5000); // Prevents suprious text files when turning the car on and off rapidly
    
    /* Set up Serial, XBee and CAN */
    Serial.begin(115200);
    NRF.begin(115200);
    ESP.begin(115200);

    //FLEXCAN0_MCR &= 0xFFFDFFFF; // Enables CAN message self-reception
    CAN_1.begin();
    CAN_2.begin();
    CAN_3.begin();
    
    setupClock();

    setupSD();
}
void loop() {
    /* Process and log incoming CAN messages */
    parse_can_lines();
    read_analog_values();
    /* Send messages over XBee */
    send_xbee();
    /* Flush data to SD card occasionally */
    if (timer_flush.check()) {
        logger.flush(); // Flush data to disk (data is also flushed whenever the 512 Byte buffer fills up, but this call ensures we don't lose more than a second of data when the car turns off)
    }
    /* Print timestamp to serial occasionally */
    if (timer_debug_RTC.check()) {
        Serial.println(Teensy3Clock.get());
    }
    /* Process MCU analog readings */
    if (timer_mcu_analog_readings.check()) {
        process_mcu_analog_readings();
    }
    /* Couloumb counting */
    #if COULOUMB_COUNTING_EN
    if (timer_total_discharge.check()) {
        write_total_discharge();
    }
    #endif
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
    CAN_1.write(msg_tx);
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
