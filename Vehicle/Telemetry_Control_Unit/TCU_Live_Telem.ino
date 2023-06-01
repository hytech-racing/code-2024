
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

Metro detailed_voltages_timer = Metro(2000);
Metro voltages_timer = Metro(2000);
Metro detailed_temperatures_timer = Metro(2000);
Metro onboard_temperatures_timer = Metro(2000);
Metro temperatures_timer = Metro(2000);
Metro mc_temps1_timer = Metro(2000);
Metro mc_temps2_timer = Metro(2000);
Metro mc_temps3_timer = Metro(2000);
Metro mc_temps4_timer = Metro(2000);

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


void live_telem_loop() {

  if (millis() > 30000) {
    if (detailed_voltages_timer.check()) {
      send_detailed_voltages();
      Serial8.write(0);
    }
    
    if (voltages_timer.check()) {
      send_voltages();
    }
    if (detailed_temperatures_timer.check()) {
      send_detailed_temperatures();
      Serial8.write(0);
    }
    /*
    if (onboard_temperatures_timer.check()) {
      send_onboard_temperatures();
    }
    if (temperatures_timer.check()) {
      send_temperatures();
    }
    if (mc_temps1_timer.check()) {
      send_mc_temps1();
    }
    if (mc_temps2_timer.check()) {
      send_mc_temps2();
    }
    if (mc_temps3_timer.check()) {
      send_mc_temps3();
    }
    if (mc_temps4_timer.check()) {
      send_mc_temps4();
    }
    Serial8.write(0);
    */
  }
}