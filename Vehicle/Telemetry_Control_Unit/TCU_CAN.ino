/*
 * Handles the input of CAN messages
 * sorry to whoever looks at this later
 * uwu🥺
 */

#include <FlexCAN_T4.h>
#include <HyTech_CAN.h>

#define MESSAGE_TABLE(F, ...) \
    F(MC_status, ID_MC1_STATUS, 1, 100 ,##__VA_ARGS__)\
    F(MC_temps, ID_MC1_TEMPS, 2, 100 ,##__VA_ARGS__)\
    F(MC_energy, ID_MC1_ENERGY, 3, 100 ,##__VA_ARGS__)

#define GET_CLASS_INST(CLASS, ID, NUM, TIME) CLASS ## _message ## NUM

#define GET_TIMER(CLASS, ID, NUM, TIME) timer_ ## CLASS ## NUM


#define TIMER(CLASS, ID, NUM, TIME)\
    Metro GET_TIMER(CLASS, ID, NUM, TIME) = Metro(TIME);

#define DECLARE_CLASS(CLASS, ID, NUM, TIME)\
    CLASS GET_CLASS_INST(CLASS, ID, NUM, TIME);

#define PARSING_CASES(CLASS, ID, NUM, TIME, MSG)\
    case ID: GET_CLASS_INST(CLASS, ID, NUM, TIME).load(MSG.buf); break;

#define UPLOAD_MSG(CLASS, ID, NUM, TIME, MSG)\
    GET_CLASS_INST(CLASS, ID, NUM, TIME).write(MSG.buf);\
    MSG.len = sizeof(CLASS);\
    MSG.id = ID;\
    write_xbee_data();

#define CHECK_TIMER(CLASS, ID, NUM, TIME, MSG)\
    if (GET_TIMER(CLASS, ID, NUM, TIME).check()) {\
        UPLOAD_MSG(CLASS, ID, TIME, MSG)\
    }

/*
MESSAGE_TABLE(TIMER)

MESSAGE_TABLE(DECLARE_CLASS)


void parse_can_message_macro(CAN_message_t& msg) {
    write_to_SD(&msg);

    switch(msg.id) {
        MESSAGE_TABLE(PARSING_CASES, msg)
    }
}
*/

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