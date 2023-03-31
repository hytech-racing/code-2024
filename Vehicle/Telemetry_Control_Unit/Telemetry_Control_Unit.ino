
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
#include <Metro.h>
#include <XBTools.h>
#include <Adafruit_GPS.h>
#include <ADC_SPI.h>

#define NRF Serial2
#define ESP Serial8

#define NUM_BMS_IC 12
#define SER_PKT_LEN 15
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

    /* Set up real-time clock */
    //Teensy3Clock.set(9999999999); // set time (epoch) at powerup  (COMMENT OUT THIS LINE AND PUSH ONCE RTC HAS BEEN SET!!!!)
    setSyncProvider(getTeensy3Time); // registers Teensy RTC as system time
    if (timeStatus() != timeSet) {
        Serial.println("RTC not set up - uncomment the Teensy3Clock.set() function call to set the time");
    } else {
        Serial.println("System time set to RTC");
    }
    last_sec_epoch = Teensy3Clock.get();
    
    //FLEXCAN0_MCR &= 0xFFFDFFFF; // Enables CAN message self-reception
    CAN_1.begin();
    CAN_2.begin();
    CAN_3.begin();
  
    /* Set up SD card */
    Serial.println("Initializing SD card...");
    SdFile::dateTimeCallback(sd_date_time); // Set date/time callback function
    if (!SD.begin(BUILTIN_SDCARD)) { // Begin Arduino SD API (Teensy 3.5)
        Serial.println("SD card failed or not present");
    }
    char filename[] = "data0000.CSV";
    for (uint8_t i = 0; i < 10000; i++) {
        filename[4] = i / 1000     + '0';
        filename[5] = i / 100 % 10 + '0';
        filename[6] = i / 10  % 10 + '0';
        filename[7] = i       % 10 + '0';
        if (!SD.exists(filename)) {
            logger = SD.open(filename, (uint8_t) O_WRITE | (uint8_t) O_CREAT); // Open file for writing
            break;
        }
        if (i == 9999) { // If all possible filenames are in use, print error
            Serial.println("All possible SD card log filenames are in use - please clean up the SD card");
        }
    }
    
    if (logger) {
        Serial.println("Successfully opened SD file");
    } else {
        Serial.println("Failed to open SD file");
    }
    
    logger.println("time,msg.id,msg.len,data"); // Print CSV heading to the logfile
    logger.flush();
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
/* Parse all CAN lines */
void parse_can_lines() {
  while(CAN_1.read(msg_rx)) {
    parse_can_message(msg_rx);
  }
  while(CAN_2.read(msg_rx)) {
    parse_can_message(msg_rx);
  }
  while(CAN_3.read(msg_rx)) {
    parse_can_message(msg_rx);
  }
}

/* Parse a result of a CAN line 
  @param msg     Result of reading from a can line 
  */
void parse_can_message(CAN_message_t& msg) {
    write_to_SD(&msg); // Write to SD card buffer (if the buffer fills up, triggering a flush to disk, this will take 8ms)
    // Identify received CAN messages and load contents into corresponding structs
    if (msg.id == ID_BMS_DETAILED_VOLTAGES) {
        BMS_detailed_voltages temp = BMS_detailed_voltages(msg.buf);
        bms_detailed_voltages[temp.get_group_id()][temp.get_ic_id()].load(msg.buf);
    }
    else if (msg.id == ID_BMS_DETAILED_TEMPERATURES) {
        BMS_detailed_temperatures temp = BMS_detailed_temperatures(msg.buf);
        bms_detailed_temperatures[temp.get_group_id()][temp.get_ic_id()].load(msg.buf);
    }
    else if (msg.id == ID_BMS_ONBOARD_DETAILED_TEMPERATURES) {
        BMS_onboard_detailed_temperatures temp = BMS_onboard_detailed_temperatures(msg.buf);
        bms_onboard_detailed_temperatures[temp.get_ic_id()].load(msg.buf);
    }
    else if (msg.id == ID_BMS_BALANCING_STATUS) {
        BMS_balancing_status temp = BMS_balancing_status(msg.buf);
        bms_balancing_status[temp.get_group_id()].load(msg.buf);
    }
    else switch(msg.id) {
        case ID_MCU_STATUS:                         
            // Serial.println("mcu_received");
            mcu_status.load(msg.buf);                        break;
        case ID_DASHBOARD_STATUS:                   dashboard_status.load(msg.buf);                  break;
        case ID_MCU_PEDAL_READINGS:                 mcu_pedal_readings.load(msg.buf);                break;
        case ID_MCU_ANALOG_READINGS:                mcu_analog_readings.load(msg.buf);               break;
        case ID_BMS_VOLTAGES:                       bms_voltages.load(msg.buf);                      break;
        case ID_BMS_TEMPERATURES:                   bms_temperatures.load(msg.buf);                  break;
        case ID_BMS_ONBOARD_TEMPERATURES:           bms_onboard_temperatures.load(msg.buf);          break;
        case ID_BMS_STATUS:                         bms_status.load(msg.buf);                        break;
        case ID_BMS_COULOMB_COUNTS:                 bms_coulomb_counts.load(msg.buf);                break;
        case ID_CCU_STATUS:                         ccu_status.load(msg.buf);                        break;
        case ID_MC1_STATUS:                         mc1_status.load(msg.buf);                        break;
        case ID_MC2_STATUS:                         mc2_status.load(msg.buf);                        break;
        case ID_MC3_STATUS:                         mc3_status.load(msg.buf);                        break;
        case ID_MC4_STATUS:                         mc4_status.load(msg.buf);                        break;
        case ID_MC1_TEMPS:                          mc1_temps.load(msg.buf);                         break;
        case ID_MC2_TEMPS:                          mc2_temps.load(msg.buf);                         break;
        case ID_MC3_TEMPS:                          mc3_temps.load(msg.buf);                         break;
        case ID_MC4_TEMPS:                          mc4_temps.load(msg.buf);                         break;
        case ID_MC1_ENERGY:                         mc1_energy.load(msg.buf);                        break;
        case ID_MC2_ENERGY:                         mc2_energy.load(msg.buf);                        break;
        case ID_MC3_ENERGY:                         mc3_energy.load(msg.buf);                        break;
        case ID_MC4_ENERGY:                         mc4_energy.load(msg.buf);                        break;
        case ID_MC1_SETPOINTS_COMMAND:              mc1_setpoints_command.load(msg.buf);             break;
        case ID_MC2_SETPOINTS_COMMAND:              mc2_setpoints_command.load(msg.buf);             break;
        case ID_MC3_SETPOINTS_COMMAND:              mc3_setpoints_command.load(msg.buf);             break;
        case ID_MC4_SETPOINTS_COMMAND:              mc4_setpoints_command.load(msg.buf);             break;
        case ID_EM_STATUS:                          em_status.load(msg.buf);                         break;
        case ID_EM_MEASUREMENT:                     em_measurement.load(msg.buf);                    break;
        case ID_IMU_ACCELEROMETER:                  imu_accelerometer.load(msg.buf);                 break;
        case ID_IMU_GYROSCOPE:                      imu_gyroscope.load(msg.buf);                     break;
        case ID_SAB_READINGS_FRONT:                 sab_readings_front.load(msg.buf);                break;
        case ID_SAB_READINGS_REAR:                  sab_readings_rear.load(msg.buf);                 break;
        case ID_SAB_READINGS_GPS:                   sab_readings_gps.load(msg.buf);                  break;
    }
}

/* Writes a given CAN message to the SD card
  @param *msg pointer to a CAN message
  */
void write_to_SD(CAN_message_t *msg) { // Note: This function does not flush data to disk! It will happen when the buffer fills or when the above flush timer fires
    // Calculate Time
    //This block is verified to loop through
    uint64_t sec_epoch = Teensy3Clock.get();
    if (sec_epoch != last_sec_epoch) {
        global_ms_offset = millis() % 1000;
        last_sec_epoch = sec_epoch;
    }
    uint64_t current_time = sec_epoch * 1000 + (millis() - global_ms_offset) % 1000;

    // Log to SD
    logger.print(current_time);
    logger.print(",");
    logger.print(msg->id, HEX);
    logger.print(",");
    logger.print(msg->len);
    logger.print(",");
    for (int i = 0; i < msg->len; i++) {
        if (msg->buf[i] < 16) {
            logger.print("0");
        }
        logger.print(msg->buf[i], HEX);
    }
    logger.println();
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
/*
  Puts encoded data into a SER_PKT_LEN + 2 long array
  @param *cobs_buf array of uint8_t[SER_PKT_LEN + 2]

  */
void create_ser_data(uint8_t *cobs_buf) {
    /*
     * DECODED FRAME STRUCTURE:
     * [ msg id (4) | msg len (1) | msg contents (8) | checksum (2) ]
     * ENCODED FRAME STRUCTURE:
     * [ fletcher (1) | msg id (4) | msg len (1) | msg contents (8) | checksum (2) | delimiter (1) ]
     */
    uint8_t xb_buf[SER_PKT_LEN];
    memcpy(xb_buf, &xb_msg.id, sizeof(xb_msg.id));        // msg id
    memcpy(xb_buf + sizeof(xb_msg.id), &xb_msg.len, sizeof(uint8_t));     // msg len
    memcpy(xb_buf + sizeof(xb_msg.id) + sizeof(uint8_t), xb_msg.buf, xb_msg.len); // msg contents
    // calculate checksum
    uint16_t checksum = fletcher16(xb_buf, SER_PKT_LEN - 2);
    //Serial.print("CHECKSUM: ");
    //Serial.println(checksum, HEX);
    memcpy(&xb_buf[SER_PKT_LEN - 2], &checksum, sizeof(uint16_t));
    for (int i = 0; i < SER_PKT_LEN; i++) {
        //Serial.print(xb_buf[i], HEX);
        //Serial.print(" ");
    }
    //Serial.println();
    cobs_encode(xb_buf, SER_PKT_LEN, cobs_buf);
    cobs_buf[SER_PKT_LEN+1] = 0x0;
    for (int i = 0; i < SER_PKT_LEN+2; i++) {
        //Serial.print(cobs_buf[i], HEX);
        //Serial.print(" ");
    }
    //Serial.println();
    memset(xb_buf, 0, sizeof(CAN_message_t));
}

void create_ser_data()

int write_xbee_data() {
    /*
     * DECODED FRAME STRUCTURE:
     * [ msg id (4) | msg len (1) | msg contents (8) | checksum (2) ]
     * ENCODED FRAME STRUCTURE:
     * [ fletcher (1) | msg id (4) | msg len (1) | msg contents (8) | checksum (2) | delimiter (1) ]
     */
    uint8_t xb_buf[SER_PKT_LEN];
    memcpy(xb_buf, &xb_msg.id, sizeof(xb_msg.id));        // msg id
    memcpy(xb_buf + sizeof(xb_msg.id), &xb_msg.len, sizeof(uint8_t));     // msg len
    memcpy(xb_buf + sizeof(xb_msg.id) + sizeof(uint8_t), xb_msg.buf, xb_msg.len); // msg contents
    // calculate checksum
    uint16_t checksum = fletcher16(xb_buf, SER_PKT_LEN - 2);
    //Serial.print("CHECKSUM: ");
    //Serial.println(checksum, HEX);
    memcpy(&xb_buf[SER_PKT_LEN - 2], &checksum, sizeof(uint16_t));
    for (int i = 0; i < SER_PKT_LEN; i++) {
        //Serial.print(xb_buf[i], HEX);
        //Serial.print(" ");
    }
    //Serial.println();
    uint8_t cobs_buf[2 + SER_PKT_LEN];
    cobs_encode(xb_buf, SER_PKT_LEN, cobs_buf);
    cobs_buf[SER_PKT_LEN+1] = 0x0;
    for (int i = 0; i < SER_PKT_LEN+2; i++) {
        //Serial.print(cobs_buf[i], HEX);
        //Serial.print(" ");
    }
    //Serial.println();
    int written = NRF.write(cobs_buf, 2 + SER_PKT_LEN);
    //Serial.print("Wrote ");
    //Serial.print(written);
    //Serial.println(" bytes");
    memset(xb_buf, 0, sizeof(CAN_message_t));
    return written;
}
void send_xbee() {
    if (timer_debug_bms_voltages.check()) {
        bms_voltages.write(xb_msg.buf);
        xb_msg.len = sizeof(BMS_voltages);
        xb_msg.id = ID_BMS_VOLTAGES;
        write_xbee_data();
        /*NRF.print("BMS VOLTAGE AVERAGE: ");
        NRF.println(bms_voltages.get_average() / (double) 10000, 4);
        NRF.print("BMS VOLTAGE LOW: ");
        NRF.println(bms_voltages.get_low() / (double) 10000, 4);
        NRF.print("BMS VOLTAGE HIGH: ");
        NRF.println(bms_voltages.get_high() / (double) 10000, 4);
        NRF.print("BMS VOLTAGE TOTAL: ");
        NRF.println(bms_voltages.get_total() / (double) 100, 2);*/
    }
    if (timer_debug_bms_detailed_voltages.check()) {
        for (int ic = 0; ic < NUM_BMS_IC; ic++) {
            for (int group = 0; group < ((ic % 2 == 0) ? 4 : 3); group++) {
                bms_detailed_voltages[group][ic].write(xb_msg.buf);
                xb_msg.len = sizeof(BMS_detailed_voltages);
                xb_msg.id = ID_BMS_DETAILED_VOLTAGES;
                write_xbee_data();
            }
        }
    }
    if (timer_debug_bms_temperatures.check()) {
        bms_temperatures.write(xb_msg.buf);
        xb_msg.len = sizeof(BMS_temperatures);
        xb_msg.id = ID_BMS_TEMPERATURES;
        write_xbee_data();
        /*NRF.print("BMS AVERAGE TEMPERATURE: ");
        NRF.println(bms_temperatures.get_average_temperature() / (double) 100, 2);
        NRF.print("BMS LOW TEMPERATURE: ");
        NRF.println(bms_temperatures.get_low_temperature() / (double) 100, 2);
        NRF.print("BMS HIGH TEMPERATURE: ");
        NRF.println(bms_temperatures.get_high_temperature() / (double) 100, 2);*/
    }
    if (timer_debug_bms_detailed_temperatures.check()) {
        for (int ic = 0; ic < NUM_BMS_IC; ic++) {
            for (int group = 0; group < 2; group++) {
                bms_detailed_temperatures[group][ic].write(xb_msg.buf);
                xb_msg.len = sizeof(BMS_detailed_temperatures);
                xb_msg.id = ID_BMS_DETAILED_TEMPERATURES;
                write_xbee_data();
            }
        }
    }
    if (timer_debug_bms_status.check()) {
        bms_status.write(xb_msg.buf);
        xb_msg.len = sizeof(BMS_status);
        xb_msg.id = ID_BMS_STATUS;
        write_xbee_data();
        /*NRF.print("BMS STATE: ");
        NRF.println(bms_status.get_state());
        NRF.print("BMS ERROR FLAGS: 0x");
        NRF.println(bms_status.get_error_flags(), HEX);
        NRF.print("BMS CURRENT: ");
        NRF.println(bms_status.get_current() / (double) 100, 2);*/
    }
    #if COULOUMB_COUNTING_EN
    if (timer_debug_bms_coulomb_counts.check()) {
        bms_coulomb_counts.write(xb_msg.buf);
        xb_msg.len = sizeof(BMS_coulomb_counts);
        xb_msg.id = ID_BMS_COULOMB_COUNTS;
        write_xbee_data();
    }
    #endif
    if (timer_mcu_status.check()) {
        mcu_status.write(xb_msg.buf);
        xb_msg.len = sizeof(MCU_status);
        xb_msg.id = ID_MCU_STATUS;
        // Serial.println("xbee mcu send");
        write_xbee_data();
    }
    if (timer_debug_mcu_pedal_readings.check()) {
        mcu_pedal_readings.write(xb_msg.buf);
        xb_msg.len = sizeof(MCU_pedal_readings);
        xb_msg.id = ID_MCU_PEDAL_READINGS;
        write_xbee_data();
    }
    
    if (timer_debug_bms_balancing_status.check()) {
        for (int i = 0; i < 2; i++) {
            bms_balancing_status[i].write(xb_msg.buf);
            xb_msg.len = sizeof(BMS_balancing_status);
            xb_msg.id = ID_BMS_BALANCING_STATUS;
            write_xbee_data();
        }
    }
    if (timer_debug_dashboard.check()) {
        dashboard_status.write(xb_msg.buf);
        xb_msg.len = sizeof(dashboard_status);
        xb_msg.id = ID_DASHBOARD_STATUS;
        write_xbee_data();
    }
    if (timer_em_status.check()) {
        em_status.write(xb_msg.buf);
        xb_msg.len = sizeof(em_status);
        xb_msg.id = ID_EM_STATUS;
        write_xbee_data();
    }
    if (timer_em_measurement.check()) {
        em_measurement.write(xb_msg.buf);
        xb_msg.len = sizeof(em_measurement);
        xb_msg.id = ID_EM_MEASUREMENT;
        write_xbee_data();
    }
    if (timer_imu_accelerometer.check()) {
        imu_accelerometer.write(xb_msg.buf);
        xb_msg.len = sizeof(imu_accelerometer);
        xb_msg.id = ID_IMU_ACCELEROMETER;
        write_xbee_data();
    }
    if (timer_imu_gyroscope.check()) {
        imu_gyroscope.write(xb_msg.buf);
        xb_msg.len = sizeof(imu_gyroscope);
        xb_msg.id = ID_IMU_GYROSCOPE;
        write_xbee_data();
    }
    if (timer_energy.check()) {
        mc1_energy.write(xb_msg.buf);
        xb_msg.len = sizeof(mc1_energy);
        xb_msg.id = ID_MC1_ENERGY;
        write_xbee_data();
        mc2_energy.write(xb_msg.buf);
        xb_msg.len = sizeof(mc2_energy);
        xb_msg.id = ID_MC2_ENERGY;
        write_xbee_data();
        mc3_energy.write(xb_msg.buf);
        xb_msg.len = sizeof(mc3_energy);
        xb_msg.id = ID_MC3_ENERGY;
        write_xbee_data();
        mc4_energy.write(xb_msg.buf);
        xb_msg.len = sizeof(mc4_energy);
        xb_msg.id = ID_MC4_ENERGY;
        write_xbee_data();
    }
    if (timer_setpoints_command.check()) {
        mc1_setpoints_command.write(xb_msg.buf);
        xb_msg.len = sizeof(mc1_setpoints_command);
        xb_msg.id = ID_MC1_SETPOINTS_COMMAND;
        write_xbee_data();
        mc2_setpoints_command.write(xb_msg.buf);
        xb_msg.len = sizeof(mc2_setpoints_command);
        xb_msg.id = ID_MC2_SETPOINTS_COMMAND;
        write_xbee_data();
        mc3_setpoints_command.write(xb_msg.buf);
        xb_msg.len = sizeof(mc3_setpoints_command);
        xb_msg.id = ID_MC3_SETPOINTS_COMMAND;
        write_xbee_data();
        mc4_setpoints_command.write(xb_msg.buf);
        xb_msg.len = sizeof(mc4_setpoints_command);
        xb_msg.id = ID_MC4_SETPOINTS_COMMAND;
        write_xbee_data();
    }
    if (timer_status.check()) {
        mc1_status.write(xb_msg.buf);
        xb_msg.len = sizeof(mc1_status);
        xb_msg.id = ID_MC1_STATUS;
        write_xbee_data();
        mc2_status.write(xb_msg.buf);
        xb_msg.len = sizeof(mc2_status);
        xb_msg.id = ID_MC2_STATUS;
        write_xbee_data();
        mc3_status.write(xb_msg.buf);
        xb_msg.len = sizeof(mc3_status);
        xb_msg.id = ID_MC3_STATUS;
        write_xbee_data();
        mc4_status.write(xb_msg.buf);
        xb_msg.len = sizeof(mc4_status);
        xb_msg.id = ID_MC4_STATUS;
        write_xbee_data();
    }
    if (timer_temps.check()) {
        mc1_temps.write(xb_msg.buf);
        xb_msg.len = sizeof(mc1_temps);
        xb_msg.id = ID_MC1_TEMPS;
        write_xbee_data();
        mc2_temps.write(xb_msg.buf);
        xb_msg.len = sizeof(mc2_temps);
        xb_msg.id = ID_MC2_TEMPS;
        write_xbee_data();
        mc3_temps.write(xb_msg.buf);
        xb_msg.len = sizeof(mc3_temps);
        xb_msg.id = ID_MC3_TEMPS;
        write_xbee_data();
        mc4_temps.write(xb_msg.buf);
        xb_msg.len = sizeof(mc4_temps);
        xb_msg.id = ID_MC4_TEMPS;
        write_xbee_data();
    }
    if (timer_sab_readings_front.check()) {
        sab_readings_front.write(xb_msg.buf);
        xb_msg.len = sizeof(sab_readings_front);
        xb_msg.id = ID_SAB_READINGS_FRONT;
        write_xbee_data();
    }
    if (timer_sab_readings_rear.check()) {
        sab_readings_rear.write(xb_msg.buf);
        xb_msg.len = sizeof(sab_readings_rear);
        xb_msg.id = ID_SAB_READINGS_REAR;
        write_xbee_data();
    }
    if (timer_sab_readings_gps.check()) {
        sab_readings_gps.write(xb_msg.buf);
        xb_msg.len = sizeof(sab_readings_gps);
        xb_msg.id = ID_SAB_READINGS_GPS;
        write_xbee_data();
    }
}
void sd_date_time(uint16_t* date, uint16_t* time) {
    // return date using FAT_DATE macro to format fields
    *date = FAT_DATE(year(), month(), day());
    // return time using FAT_TIME macro to format fields
    *time = FAT_TIME(hour(), minute(), second());
}
