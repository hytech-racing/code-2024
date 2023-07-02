
#define TOTAL_IC 12
#define EVEN_IC_CELLS 12      // Number of cells monitored by ICs with even addresses
#define ODD_IC_CELLS 9        // Number of cells monitored by ICS with odd addresses
#define THERMISTORS_PER_IC 4  // Number of cell temperature monitoring thermistors connected to each IC

BMS_detailed_voltages live_detailed_voltages[TOTAL_IC][4];
BMS_voltages live_voltages;
BMS_detailed_temperatures live_detailed_temperatures[TOTAL_IC][2];
BMS_onboard_temperatures live_onboard_temperatures;
BMS_temperatures live_temperatures;
MC_temps live_mc_temps1;
MC_temps live_mc_temps2;
MC_temps live_mc_temps3;
MC_temps live_mc_temps4;

Metro detailed_voltages_timer = Metro(5000);
Metro voltages_timer = Metro(250);
Metro detailed_temperatures_timer = Metro(5000);
Metro onboard_temperatures_timer = Metro(1000);
Metro temperatures_timer = Metro(1000);
Metro mc_temps1_timer = Metro(1000);
Metro mc_temps2_timer = Metro(1000);
Metro mc_temps3_timer = Metro(1000);
Metro mc_temps4_timer = Metro(1000);

IMU_accelerometer live_imu_accelerometer;
IMU_gyroscope live_imu_gyroscope;
MC_status live_mc1_status;
MC_status live_mc2_status;
MC_status live_mc3_status;
MC_status live_mc4_status;
MC_energy live_mc1_energy;
MC_energy live_mc2_energy;
MC_energy live_mc3_energy;
MC_energy live_mc4_energy;
MC_setpoints_command live_mc1_setpoints_command;
MC_setpoints_command live_mc2_setpoints_command;
MC_setpoints_command live_mc3_setpoints_command;
MC_setpoints_command live_mc4_setpoints_command;
MCU_status live_mcu_status;
MCU_pedal_readings live_mcu_pedal_readings;
MCU_load_cells live_mcu_load_cells;
MCU_analog_readings live_mcu_analog_readings;
BMS_status live_bms_status;
Dashboard_status live_dashboard_status;
GPS_lat_long live_gps_lat_long;
GPS_other live_gps_other;

Metro imu_accelerometer_timer = Metro(5000);
Metro imu_gyroscope_timer = Metro(5000);
Metro mc1_status_timer = Metro(5000);
Metro mc2_status_timer = Metro(5000);
Metro mc3_status_timer = Metro(5000);
Metro mc4_status_timer = Metro(5000);
Metro mc1_energy_timer = Metro(5000);
Metro mc2_energy_timer = Metro(5000);
Metro mc3_energy_timer = Metro(5000);
Metro mc4_energy_timer = Metro(5000);
Metro mc1_setpoints_command_timer = Metro(5000);
Metro mc2_setpoints_command_timer = Metro(5000);
Metro mc3_setpoints_command_timer = Metro(5000);
Metro mc4_setpoints_command_timer = Metro(5000);
Metro mcu_status_timer = Metro(5000);
Metro mcu_pedal_readings_timer = Metro(5000);
Metro mcu_load_cells_timer = Metro(5000);
Metro mcu_analog_readings_timer = Metro(5000);
Metro bms_status_timer = Metro(5000);
Metro dashboard_status_timer = Metro(5000);
Metro gps_lat_long_timer = Metro(1000);
Metro gps_other_timer = Metro(5000);

GPS_other tpms_lf;
GPS_other temp1_lf;
GPS_other temp2_lf;
GPS_other temp3_lf;
GPS_other temp4_lf;
GPS_other temp5_lf;
GPS_other tpms_rf;
GPS_other temp1_rf;
GPS_other temp2_rf;
GPS_other temp3_rf;
GPS_other temp4_rf;
GPS_other temp5_rf;
GPS_other tpms_lr;
GPS_other temp1_lr;
GPS_other temp2_lr;
GPS_other temp3_lr;
GPS_other temp4_lr;
GPS_other temp5_lr;
GPS_other tpms_rr;
GPS_other temp1_rr;
GPS_other temp2_rr;
GPS_other temp3_rr;
GPS_other temp4_rr;
GPS_other temp5_rr;

Metro tpms_lf_timer = Metro(5000);
Metro temp1_lf_timer = Metro(5000);
Metro temp2_lf_timer = Metro(5000);
Metro temp3_lf_timer = Metro(5000);
Metro temp4_lf_timer = Metro(5000);
Metro temp5_lf_timer = Metro(5000);
Metro tpms_rf_timer = Metro(5000);
Metro temp1_rf_timer = Metro(5000);
Metro temp2_rf_timer = Metro(5000);
Metro temp3_rf_timer = Metro(5000);
Metro temp4_rf_timer = Metro(5000);
Metro temp5_rf_timer = Metro(5000);
Metro tpms_lr_timer = Metro(5000);
Metro temp1_lr_timer = Metro(5000);
Metro temp2_lr_timer = Metro(5000);
Metro temp3_lr_timer = Metro(5000);
Metro temp4_lr_timer = Metro(5000);
Metro temp5_lr_timer = Metro(5000);
Metro tpms_rr_timer = Metro(5000);
Metro temp1_rr_timer = Metro(5000);
Metro temp2_rr_timer = Metro(5000);
Metro temp3_rr_timer = Metro(5000);
Metro temp4_rr_timer = Metro(5000);
Metro temp5_rr_timer = Metro(5000);

static CAN_message_t live_telem_msg;

void parse_detailed_voltages(const CAN_message_t& rx_msg) {
  BMS_detailed_voltages detailed_voltages_message;
  detailed_voltages_message.load(rx_msg.buf);
  live_detailed_voltages[detailed_voltages_message.get_ic_id()][detailed_voltages_message.get_group_id()].load(rx_msg.buf);
}
void send_detailed_voltages() {
  live_telem_msg.len = sizeof(BMS_detailed_voltages);
  live_telem_msg.id = ID_BMS_DETAILED_VOLTAGES;
  for (int ic = 0; ic < TOTAL_IC; ic++) {
    for (int group = 0; group < ((ic % 2 == 0) ? 4 : 3); group++) {
      live_detailed_voltages[ic][group].write(live_telem_msg.buf);
      send_can_message(Serial8, live_telem_msg);
    }
  }
}

void parse_voltages(const CAN_message_t& rx_msg) {
  live_voltages.load(rx_msg.buf);
}
void send_voltages() {
  live_voltages.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(BMS_voltages);
  live_telem_msg.id = ID_BMS_VOLTAGES;
  send_can_message(Serial8, live_telem_msg);
}

void parse_detailed_temperatures(const CAN_message_t& rx_msg) {
  BMS_detailed_temperatures detailed_temps_message;
  detailed_temps_message.load(rx_msg.buf);
  live_detailed_temperatures[detailed_temps_message.get_ic_id()][detailed_temps_message.get_group_id()].load(rx_msg.buf);
}
void send_detailed_temperatures() {
  live_telem_msg.len = sizeof(BMS_detailed_temperatures);
  live_telem_msg.id = ID_BMS_DETAILED_TEMPERATURES;
  for (int ic = 0; ic < TOTAL_IC; ic++) {
    for (int group = 0; group < 2; group++) {
      live_detailed_temperatures[ic][group].write(live_telem_msg.buf);
      send_can_message(Serial8, live_telem_msg);
    }
  }
}

void parse_onboard_temperatures(const CAN_message_t& rx_msg) {
  live_onboard_temperatures.load(rx_msg.buf);
}
void send_onboard_temperatures() {
  live_onboard_temperatures.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(BMS_onboard_temperatures);
  live_telem_msg.id = ID_BMS_ONBOARD_TEMPERATURES;
  send_can_message(Serial8, live_telem_msg);
}

void parse_temperatures(const CAN_message_t& rx_msg) {
  live_temperatures.load(rx_msg.buf);
}
void send_temperatures() {
  live_temperatures.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(BMS_temperatures);
  live_telem_msg.id = ID_BMS_TEMPERATURES;
  send_can_message(Serial8, live_telem_msg);
}

void parse_mc_temps1(const CAN_message_t& rx_msg) {
  live_mc_temps1.load(rx_msg.buf);
}
void send_mc_temps1() {
  live_mc_temps1.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(MC_temps);
  live_telem_msg.id = ID_MC1_TEMPS;
  send_can_message(Serial8, live_telem_msg);
}

void parse_mc_temps2(const CAN_message_t& rx_msg) {
  live_mc_temps2.load(rx_msg.buf);
}
void send_mc_temps2() {
  live_mc_temps2.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(MC_temps);
  live_telem_msg.id = ID_MC2_TEMPS;
  send_can_message(Serial8, live_telem_msg);
}

void parse_mc_temps3(const CAN_message_t& rx_msg) {
  live_mc_temps3.load(rx_msg.buf);
}
void send_mc_temps3() {
  live_mc_temps3.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(MC_temps);
  live_telem_msg.id = ID_MC3_TEMPS;
  send_can_message(Serial8, live_telem_msg);
}

void parse_mc_temps4(const CAN_message_t& rx_msg) {
  live_mc_temps4.load(rx_msg.buf);
}
void send_mc_temps4() {
  live_mc_temps4.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(MC_temps);
  live_telem_msg.id = ID_MC4_TEMPS;
  send_can_message(Serial8, live_telem_msg);
}


void parse_imu_accelerometer(const CAN_message_t& rx_msg) {
  live_imu_accelerometer.load(rx_msg.buf);
}
void send_imu_accelerometer() {
  live_imu_accelerometer.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(IMU_accelerometer);
  live_telem_msg.id = ID_IMU_ACCELEROMETER;
  send_can_message(Serial8, live_telem_msg);
  //MULTI LINE EDITING
}

void parse_imu_gyroscope(const CAN_message_t& rx_msg) {
  live_imu_gyroscope.load(rx_msg.buf);
}
void send_imu_gyroscope() {
  live_imu_gyroscope.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(IMU_gyroscope);
  live_telem_msg.id = ID_IMU_GYROSCOPE;
  send_can_message(Serial8, live_telem_msg);
  //MULTI LINE EDITING
}

void parse_mc1_status(const CAN_message_t& rx_msg) {
  live_mc1_status.load(rx_msg.buf);
}
void send_mc1_status() {
  live_mc1_status.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(MC_status);
  live_telem_msg.id = ID_MC1_STATUS;
  send_can_message(Serial8, live_telem_msg);
  //MULTI LINE EDITING
}

void parse_mc2_status(const CAN_message_t& rx_msg) {
  live_mc2_status.load(rx_msg.buf);
}
void send_mc2_status() {
  live_mc2_status.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(MC_status);
  live_telem_msg.id = ID_MC2_STATUS;
  send_can_message(Serial8, live_telem_msg);
  //MULTI LINE EDITING
}

void parse_mc3_status(const CAN_message_t& rx_msg) {
  live_mc3_status.load(rx_msg.buf);
}
void send_mc3_status() {
  live_mc3_status.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(MC_status);
  live_telem_msg.id = ID_MC3_STATUS;
  send_can_message(Serial8, live_telem_msg);
  //MULTI LINE EDITING
}

void parse_mc4_status(const CAN_message_t& rx_msg) {
  live_mc4_status.load(rx_msg.buf);
}
void send_mc4_status() {
  live_mc4_status.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(MC_status);
  live_telem_msg.id = ID_MC4_STATUS;
  send_can_message(Serial8, live_telem_msg);
  //MULTI LINE EDITING
}

void parse_mc1_energy(const CAN_message_t& rx_msg) {
  live_mc1_energy.load(rx_msg.buf);
}
void send_mc1_energy() {
  live_mc1_energy.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(MC_energy);
  live_telem_msg.id = ID_MC1_ENERGY;
  send_can_message(Serial8, live_telem_msg);
  //MULTI LINE EDITING
}

void parse_mc2_energy(const CAN_message_t& rx_msg) {
  live_mc2_energy.load(rx_msg.buf);
}
void send_mc2_energy() {
  live_mc2_energy.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(MC_energy);
  live_telem_msg.id = ID_MC2_ENERGY;
  send_can_message(Serial8, live_telem_msg);
  //MULTI LINE EDITING
}

void parse_mc3_energy(const CAN_message_t& rx_msg) {
  live_mc3_energy.load(rx_msg.buf);
}
void send_mc3_energy() {
  live_mc3_energy.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(MC_energy);
  live_telem_msg.id = ID_MC3_ENERGY;
  send_can_message(Serial8, live_telem_msg);
  //MULTI LINE EDITING
}

void parse_mc4_energy(const CAN_message_t& rx_msg) {
  live_mc4_energy.load(rx_msg.buf);
}
void send_mc4_energy() {
  live_mc4_energy.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(MC_energy);
  live_telem_msg.id = ID_MC4_ENERGY;
  send_can_message(Serial8, live_telem_msg);
  //MULTI LINE EDITING
}

void parse_mc1_setpoints_command(const CAN_message_t& rx_msg) {
  live_mc1_setpoints_command.load(rx_msg.buf);
}
void send_mc1_setpoints_command() {
  live_mc1_setpoints_command.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(MC_setpoints_command);
  live_telem_msg.id = ID_MC1_SETPOINTS_COMMAND;
  send_can_message(Serial8, live_telem_msg);
  //MULTI LINE EDITING
}

void parse_mc2_setpoints_command(const CAN_message_t& rx_msg) {
  live_mc2_setpoints_command.load(rx_msg.buf);
}
void send_mc2_setpoints_command() {
  live_mc2_setpoints_command.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(MC_setpoints_command);
  live_telem_msg.id = ID_MC2_SETPOINTS_COMMAND;
  send_can_message(Serial8, live_telem_msg);
  //MULTI LINE EDITING
}

void parse_mc3_setpoints_command(const CAN_message_t& rx_msg) {
  live_mc3_setpoints_command.load(rx_msg.buf);
}
void send_mc3_setpoints_command() {
  live_mc3_setpoints_command.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(MC_setpoints_command);
  live_telem_msg.id = ID_MC3_SETPOINTS_COMMAND;
  send_can_message(Serial8, live_telem_msg);
  //MULTI LINE EDITING
}

void parse_mc4_setpoints_command(const CAN_message_t& rx_msg) {
  live_mc4_setpoints_command.load(rx_msg.buf);
}
void send_mc4_setpoints_command() {
  live_mc4_setpoints_command.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(MC_setpoints_command);
  live_telem_msg.id = ID_MC4_SETPOINTS_COMMAND;
  send_can_message(Serial8, live_telem_msg);
  //MULTI LINE EDITING
}

void parse_mcu_status(const CAN_message_t& rx_msg) {
  live_mcu_status.load(rx_msg.buf);
}
void send_mcu_status() {
  live_mcu_status.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(MCU_status);
  live_telem_msg.id = ID_MCU_STATUS;
  send_can_message(Serial8, live_telem_msg);
  //MULTI LINE EDITING
}

void parse_mcu_pedal_readings(const CAN_message_t& rx_msg) {
  live_mcu_pedal_readings.load(rx_msg.buf);
}
void send_mcu_pedal_readings() {
  live_mcu_pedal_readings.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(MCU_pedal_readings);
  live_telem_msg.id = ID_MCU_PEDAL_READINGS;
  send_can_message(Serial8, live_telem_msg);
  //MULTI LINE EDITING
}

void parse_mcu_load_cells(const CAN_message_t& rx_msg) {
  live_mcu_load_cells.load(rx_msg.buf);
}
void send_mcu_load_cells() {
  live_mcu_load_cells.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(MCU_load_cells);
  live_telem_msg.id = ID_MCU_LOAD_CELLS;
  send_can_message(Serial8, live_telem_msg);
  //MULTI LINE EDITING
}

void parse_mcu_analog_readings(const CAN_message_t& rx_msg) {
  live_mcu_analog_readings.load(rx_msg.buf);
}
void send_mcu_analog_readings() {
  live_mcu_analog_readings.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(MCU_analog_readings);
  live_telem_msg.id = ID_MCU_ANALOG_READINGS;
  send_can_message(Serial8, live_telem_msg);
  //MULTI LINE EDITING
}

void parse_bms_status(const CAN_message_t& rx_msg) {
  live_bms_status.load(rx_msg.buf);
}
void send_bms_status() {
  live_bms_status.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(BMS_status);
  live_telem_msg.id = ID_BMS_STATUS;
  send_can_message(Serial8, live_telem_msg);
  //MULTI LINE EDITING
}

void parse_dashboard_status(const CAN_message_t& rx_msg) {
  live_dashboard_status.load(rx_msg.buf);
}
void send_dashboard_status() {
  live_dashboard_status.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(Dashboard_status);
  live_telem_msg.id = ID_DASHBOARD_STATUS;
  send_can_message(Serial8, live_telem_msg);
  //MULTI LINE EDITING
}

void parse_gps_lat_long(const CAN_message_t& rx_msg) {
  live_gps_lat_long.load(rx_msg.buf);
}
void send_gps_lat_long() {
  live_gps_lat_long.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_lat_long);
  live_telem_msg.id = ID_GPS_LAT_LONG;
  send_can_message(Serial8, live_telem_msg);
  //MULTI LINE EDITING
}

void parse_gps_other(const CAN_message_t& rx_msg) {
  live_gps_other.load(rx_msg.buf);
}
void send_gps_other() {
  live_gps_other.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_GPS_OTHER;
  send_can_message(Serial8, live_telem_msg);
  //MULTI LINE EDITING
}

void parse_tpms_lf(const CAN_message_t& rx_msg) {
  tpms_lf.load(rx_msg.buf);
}
void send_tpms_lf() {
  tpms_lf.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_TPMS_LF;
  send_can_message(Serial8, live_telem_msg);
}
void parse_temp1_lf(const CAN_message_t& rx_msg) {
  temp1_lf.load(rx_msg.buf);
}
void send_temp1_lf() {
  temp1_lf.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_TEMP1_LF;
  send_can_message(Serial8, live_telem_msg);
}
void parse_temp2_lf(const CAN_message_t& rx_msg) {
  temp2_lf.load(rx_msg.buf);
}
void send_temp2_lf() {
  temp2_lf.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_TEMP2_LF;
  send_can_message(Serial8, live_telem_msg);
}
void parse_temp3_lf(const CAN_message_t& rx_msg) {
  temp3_lf.load(rx_msg.buf);
}
void send_temp3_lf() {
  temp3_lf.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_TEMP3_LF;
  send_can_message(Serial8, live_telem_msg);
}
void parse_temp4_lf(const CAN_message_t& rx_msg) {
  temp4_lf.load(rx_msg.buf);
}
void send_temp4_lf() {
  temp4_lf.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_TEMP4_LF;
  send_can_message(Serial8, live_telem_msg);
}
void parse_temp5_lf(const CAN_message_t& rx_msg) {
  temp5_lf.load(rx_msg.buf);
}
void send_temp5_lf() {
  temp5_lf.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_TEMP5_LF;
  send_can_message(Serial8, live_telem_msg);
}
void parse_tpms_rf(const CAN_message_t& rx_msg) {
  tpms_rf.load(rx_msg.buf);
}
void send_tpms_rf() {
  tpms_rf.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_TPMS_RF;
  send_can_message(Serial8, live_telem_msg);
}
void parse_temp1_rf(const CAN_message_t& rx_msg) {
  temp1_rf.load(rx_msg.buf);
}
void send_temp1_rf() {
  temp1_rf.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_TEMP1_RF;
  send_can_message(Serial8, live_telem_msg);
}
void parse_temp2_rf(const CAN_message_t& rx_msg) {
  temp2_rf.load(rx_msg.buf);
}
void send_temp2_rf() {
  temp2_rf.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_TEMP2_RF;
  send_can_message(Serial8, live_telem_msg);
}
void parse_temp3_rf(const CAN_message_t& rx_msg) {
  temp3_rf.load(rx_msg.buf);
}
void send_temp3_rf() {
  temp3_rf.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_TEMP3_RF;
  send_can_message(Serial8, live_telem_msg);
}
void parse_temp4_rf(const CAN_message_t& rx_msg) {
  temp4_rf.load(rx_msg.buf);
}
void send_temp4_rf() {
  temp4_rf.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_TEMP4_RF;
  send_can_message(Serial8, live_telem_msg);
}
void parse_temp5_rf(const CAN_message_t& rx_msg) {
  temp5_rf.load(rx_msg.buf);
}
void send_temp5_rf() {
  temp5_rf.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_TEMP5_RF;
  send_can_message(Serial8, live_telem_msg);
}
void parse_tpms_lr(const CAN_message_t& rx_msg) {
  tpms_lr.load(rx_msg.buf);
}
void send_tpms_lr() {
  tpms_lr.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_TPMS_LR;
  send_can_message(Serial8, live_telem_msg);
}
void parse_temp1_lr(const CAN_message_t& rx_msg) {
  temp1_lr.load(rx_msg.buf);
}
void send_temp1_lr() {
  temp1_lr.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_TEMP1_LR;
  send_can_message(Serial8, live_telem_msg);
}
void parse_temp2_lr(const CAN_message_t& rx_msg) {
  temp2_lr.load(rx_msg.buf);
}
void send_temp2_lr() {
  temp2_lr.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_TEMP2_LR;
  send_can_message(Serial8, live_telem_msg);
}
void parse_temp3_lr(const CAN_message_t& rx_msg) {
  temp3_lr.load(rx_msg.buf);
}
void send_temp3_lr() {
  temp3_lr.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_TEMP3_LR;
  send_can_message(Serial8, live_telem_msg);
}
void parse_temp4_lr(const CAN_message_t& rx_msg) {
  temp4_lr.load(rx_msg.buf);
}
void send_temp4_lr() {
  temp4_lr.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_TEMP4_LR;
  send_can_message(Serial8, live_telem_msg);
}
void parse_temp5_lr(const CAN_message_t& rx_msg) {
  temp5_lr.load(rx_msg.buf);
}
void send_temp5_lr() {
  temp5_lr.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_TEMP5_LR;
  send_can_message(Serial8, live_telem_msg);
}
void parse_tpms_rr(const CAN_message_t& rx_msg) {
  tpms_rr.load(rx_msg.buf);
}
void send_tpms_rr() {
  tpms_rr.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_TPMS_RR;
  send_can_message(Serial8, live_telem_msg);
}
void parse_temp1_rr(const CAN_message_t& rx_msg) {
  temp1_rr.load(rx_msg.buf);
}
void send_temp1_rr() {
  temp1_rr.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_TEMP1_RR;
  send_can_message(Serial8, live_telem_msg);
}
void parse_temp2_rr(const CAN_message_t& rx_msg) {
  temp2_rr.load(rx_msg.buf);
}
void send_temp2_rr() {
  temp2_rr.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_TEMP2_RR;
  send_can_message(Serial8, live_telem_msg);
}
void parse_temp3_rr(const CAN_message_t& rx_msg) {
  temp3_rr.load(rx_msg.buf);
}
void send_temp3_rr() {
  temp3_rr.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_TEMP3_RR;
  send_can_message(Serial8, live_telem_msg);
}
void parse_temp4_rr(const CAN_message_t& rx_msg) {
  temp4_rr.load(rx_msg.buf);
}
void send_temp4_rr() {
  temp4_rr.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_TEMP4_RR;
  send_can_message(Serial8, live_telem_msg);
}
void parse_temp5_rr(const CAN_message_t& rx_msg) {
  temp5_rr.load(rx_msg.buf);
}
void send_temp5_rr() {
  temp5_rr.write(live_telem_msg.buf);
  live_telem_msg.len = sizeof(GPS_other);
  live_telem_msg.id = ID_TEMP5_RR;
  send_can_message(Serial8, live_telem_msg);
}

uint8_t live_telem_ready = 0;

void live_telem_setup() {
  detailed_voltages_timer.reset();
  voltages_timer.reset();
  detailed_temperatures_timer.reset();
  onboard_temperatures_timer.reset();
  temperatures_timer.reset();
  mc_temps1_timer.reset();
  mc_temps2_timer.reset();
  mc_temps3_timer.reset();
  mc_temps4_timer.reset();

  imu_accelerometer_timer.reset();
  imu_gyroscope_timer.reset();
  mc1_status_timer.reset();
  mc2_status_timer.reset();
  mc3_status_timer.reset();
  mc4_status_timer.reset();
  mc1_energy_timer.reset();
  mc2_energy_timer.reset();
  mc3_energy_timer.reset();
  mc4_energy_timer.reset();
  mc1_setpoints_command_timer.reset();
  mc2_setpoints_command_timer.reset();
  mc3_setpoints_command_timer.reset();
  mc4_setpoints_command_timer.reset();
  mcu_status_timer.reset();
  mcu_pedal_readings_timer.reset();
  mcu_load_cells_timer.reset();
  mcu_analog_readings_timer.reset();
  bms_status_timer.reset();
  dashboard_status_timer.reset();
  gps_lat_long_timer.reset();
  gps_other_timer.reset();


  detailed_voltages_timer.offset(0);
  voltages_timer.offset(0);
  detailed_temperatures_timer.offset(0);
  onboard_temperatures_timer.offset(0);
  temperatures_timer.offset(0);
  mc_temps1_timer.offset(0);
  mc_temps2_timer.offset(0);
  mc_temps3_timer.offset(0);
  mc_temps4_timer.offset(0);

  imu_accelerometer_timer.offset(500);
  imu_gyroscope_timer.offset(500);
  mc1_status_timer.offset(1500);
  mc2_status_timer.offset(1500);
  mc3_status_timer.offset(1500);
  mc4_status_timer.offset(1500);
  mc1_energy_timer.offset(2500);
  mc2_energy_timer.offset(2500);
  mc3_energy_timer.offset(2500);
  mc4_energy_timer.offset(2500);
  mc1_setpoints_command_timer.offset(3500);
  mc2_setpoints_command_timer.offset(3500);
  mc3_setpoints_command_timer.offset(3500);
  mc4_setpoints_command_timer.offset(3500);
  mcu_status_timer.offset(4500);
  mcu_pedal_readings_timer.offset(4500);
  mcu_load_cells_timer.offset(4500);
  mcu_analog_readings_timer.offset(4500);
  bms_status_timer.offset(4500);
  dashboard_status_timer.offset(4500);
  gps_lat_long_timer.offset(4500);
  gps_other_timer.offset(4500);

  tpms_lf_timer.reset();
  temp1_lf_timer.reset();
  temp2_lf_timer.reset();
  temp3_lf_timer.reset();
  temp4_lf_timer.reset();
  temp5_lf_timer.reset();
  tpms_rf_timer.reset();
  temp1_rf_timer.reset();
  temp2_rf_timer.reset();
  temp3_rf_timer.reset();
  temp4_rf_timer.reset();
  temp5_rf_timer.reset();
  tpms_lr_timer.reset();
  temp1_lr_timer.reset();
  temp2_lr_timer.reset();
  temp3_lr_timer.reset();
  temp4_lr_timer.reset();
  temp5_lr_timer.reset();
  tpms_rr_timer.reset();
  temp1_rr_timer.reset();
  temp2_rr_timer.reset();
  temp3_rr_timer.reset();
  temp4_rr_timer.reset();
  temp5_rr_timer.reset();

  tpms_lf_timer.offset(1000);
  temp1_lf_timer.offset(1000);
  temp2_lf_timer.offset(1000);
  temp3_lf_timer.offset(1000);
  temp4_lf_timer.offset(1000);
  temp5_lf_timer.offset(1000);
  tpms_rf_timer.offset(2000);
  temp1_rf_timer.offset(2000);
  temp2_rf_timer.offset(2000);
  temp3_rf_timer.offset(2000);
  temp4_rf_timer.offset(2000);
  temp5_rf_timer.offset(2000);
  tpms_lr_timer.offset(3000);
  temp1_lr_timer.offset(3000);
  temp2_lr_timer.offset(3000);
  temp3_lr_timer.offset(3000);
  temp4_lr_timer.offset(3000);
  temp5_lr_timer.offset(3000);
  tpms_rr_timer.offset(4000);
  temp1_rr_timer.offset(4000);
  temp2_rr_timer.offset(4000);
  temp3_rr_timer.offset(4000);
  temp4_rr_timer.offset(4000);
  temp5_rr_timer.offset(4000);

  live_telem_ready = 1;
}

void live_telem_loop() {

  if (live_telem_ready) {
    uint8_t sent = 0;
    /*if (detailed_voltages_timer.check()) {
      send_detailed_voltages();
      sent = 1;
      //Serial8.write(0);
    }*/
    if (voltages_timer.check()) {
      send_voltages();
      sent = 1;
    }
    /*
    if (detailed_temperatures_timer.check()) {
      send_detailed_temperatures();
      sent = 1;
    }
    if (onboard_temperatures_timer.check()) {
      send_onboard_temperatures();
      sent = 1;
    }
    */
    if (temperatures_timer.check()) {
      send_temperatures();
      sent = 1;
    }
    if (mc_temps1_timer.check()) {
      send_mc_temps1();
      sent = 1;
    }
    if (mc_temps2_timer.check()) {
      send_mc_temps2();
      sent = 1;
    }
    if (mc_temps3_timer.check()) {
      send_mc_temps3();
      sent = 1;
    }
    if (mc_temps4_timer.check()) {
      send_mc_temps4();
      sent = 1;
    }

    if(imu_accelerometer_timer.check()) {
      send_imu_accelerometer();
      sent = 1;
      //timer check multi line editing
    }
    if(imu_gyroscope_timer.check()) {
      send_imu_gyroscope();
      sent = 1;
      //timer check multi line editing
    }
    if(mc1_status_timer.check()) {
      send_mc1_status();
      sent = 1;
      //timer check multi line editing
    }
    if(mc2_status_timer.check()) {
      send_mc2_status();
      sent = 1;
      //timer check multi line editing
    }
    if(mc3_status_timer.check()) {
      send_mc3_status();
      sent = 1;
      //timer check multi line editing
    }
    if(mc4_status_timer.check()) {
      send_mc4_status();
      sent = 1;
      //timer check multi line editing
    }
    if(mc1_energy_timer.check()) {
      send_mc1_energy();
      sent = 1;
      //timer check multi line editing
    }
    if(mc2_energy_timer.check()) {
      send_mc2_energy();
      sent = 1;
      //timer check multi line editing
    }
    if(mc3_energy_timer.check()) {
      send_mc3_energy();
      sent = 1;
      //timer check multi line editing
    }
    if(mc4_energy_timer.check()) {
      send_mc4_energy();
      sent = 1;
      //timer check multi line editing
    }
    if(mc1_setpoints_command_timer.check()) {
      send_mc1_setpoints_command();
      sent = 1;
      //timer check multi line editing
    }
    if(mc2_setpoints_command_timer.check()) {
      send_mc2_setpoints_command();
      sent = 1;
      //timer check multi line editing
    }
    if(mc3_setpoints_command_timer.check()) {
      send_mc3_setpoints_command();
      sent = 1;
      //timer check multi line editing
    }
    if(mc4_setpoints_command_timer.check()) {
      send_mc4_setpoints_command();
      sent = 1;
      //timer check multi line editing
    }
    if(mcu_status_timer.check()) {
      send_mcu_status();
      sent = 1;
      //timer check multi line editing
    }
    if(mcu_pedal_readings_timer.check()) {
      send_mcu_pedal_readings();
      sent = 1;
      //timer check multi line editing
    }
    if(mcu_load_cells_timer.check()) {
      send_mcu_load_cells();
      sent = 1;
      //timer check multi line editing
    }
    if(mcu_analog_readings_timer.check()) {
      send_mcu_analog_readings();
      sent = 1;
      //timer check multi line editing
    }
    if(bms_status_timer.check()) {
      send_bms_status();
      sent = 1;
      //timer check multi line editing
    }
    if(dashboard_status_timer.check()) {
      send_dashboard_status();
      sent = 1;
      //timer check multi line editing
    }
    if(gps_lat_long_timer.check()) {
      send_gps_lat_long();
      sent = 1;
      //timer check multi line editing
    }
    if(gps_other_timer.check()) {
      send_gps_other();
      sent = 1;
      //timer check multi line editing
    }

    if(tpms_lf_timer.check()) {
      send_tpms_lf();
      sent = 1;
    }
    if(temp1_lf_timer.check()) {
      send_temp1_lf();
      sent = 1;
    }
    if(temp2_lf_timer.check()) {
      send_temp2_lf();
      sent = 1;
    }
    if(temp3_lf_timer.check()) {
      send_temp3_lf();
      sent = 1;
    }
    if(temp4_lf_timer.check()) {
      send_temp4_lf();
      sent = 1;
    }
    if(temp5_lf_timer.check()) {
      send_temp5_lf();
      sent = 1;
    }
    if(tpms_rf_timer.check()) {
      send_tpms_rf();
      sent = 1;
    }
    if(temp1_rf_timer.check()) {
      send_temp1_rf();
      sent = 1;
    }
    if(temp2_rf_timer.check()) {
      send_temp2_rf();
      sent = 1;
    }
    if(temp3_rf_timer.check()) {
      send_temp3_rf();
      sent = 1;
    }
    if(temp4_rf_timer.check()) {
      send_temp4_rf();
      sent = 1;
    }
    if(temp5_rf_timer.check()) {
      send_temp5_rf();
      sent = 1;
    }
    if(tpms_lr_timer.check()) {
      send_tpms_lr();
      sent = 1;
    }
    if(temp1_lr_timer.check()) {
      send_temp1_lr();
      sent = 1;
    }
    if(temp2_lr_timer.check()) {
      send_temp2_lr();
      sent = 1;
    }
    if(temp3_lr_timer.check()) {
      send_temp3_lr();
      sent = 1;
    }
    if(temp4_lr_timer.check()) {
      send_temp4_lr();
      sent = 1;
    }
    if(temp5_lr_timer.check()) {
      send_temp5_lr();
      sent = 1;
    }
    if(tpms_rr_timer.check()) {
      send_tpms_rr();
      sent = 1;
    }
    if(temp1_rr_timer.check()) {
      send_temp1_rr();
      sent = 1;
    }
    if(temp2_rr_timer.check()) {
      send_temp2_rr();
      sent = 1;
    }
    if(temp3_rr_timer.check()) {
      send_temp3_rr();
      sent = 1;
    }
    if(temp4_rr_timer.check()) {
      send_temp4_rr();
      sent = 1;
    }
    if(temp5_rr_timer.check()) {
      send_temp5_rr();
      sent = 1;
    }

    if (sent) {
      Serial8.write(0);
    }
  }
}