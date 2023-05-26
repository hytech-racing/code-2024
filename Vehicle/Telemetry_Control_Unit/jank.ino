#define TOTAL_IC 12                   // Number of LTC6811-2 ICs that are used in the accumulator
#define EVEN_IC_CELLS 12              // Number of cells monitored by ICs with even addresses
#define ODD_IC_CELLS 9                // Number of cells monitored by ICS with odd addresses
#define THERMISTORS_PER_IC 4          // Number of cell temperature monitoring thermistors connected to each IC
#define MAX_SUCCESSIVE_FAULTS 20      // Number of successive faults permitted before AMS fault is broadcast over CAN
#define MIN_VOLTAGE 30000             // Minimum allowable single cell voltage in units of 100μV
#define MAX_VOLTAGE 42000             // Maxiumum allowable single cell voltage in units of 100μV
#define MAX_TOTAL_VOLTAGE 5330000     // Maximum allowable pack total voltage in units of 100μV
#define MAX_THERMISTOR_VOLTAGE 26225  // Maximum allowable pack temperature corresponding to 60C in units 100μV
#define BALANCE_ON true
#define BALANCE_COOL 6000        // Sets balancing duty cycle as 33.3%
#define BALANCE_STANDARD 4000    // Sets balancing duty cycle as 50%
#define BALANCE_HOT 3000         // Sets balancing duty cycle as 66%
#define BALANCE_CONTINUOUS 2000  // Sets balancing duty cycle as 100%
#define BALANCE_MODE 1           // Mode 0 is normal balance, mode 1 is progressive balance

//BMS_detailed_voltages detailed_voltages;
BMS_temperatures temperatures;
BMS_voltages voltages;
MC_temps mc_temps1;
MC_temps mc_temps2;
MC_temps mc_temps3;
MC_temps mc_temps4;


uint16_t cell_voltages[TOTAL_IC][12];  // 2D Array to hold cell voltages being read in; voltages are read in with the base unit as 100μV
BMS_detailed_voltages detailed_voltages[TOTAL_IC][4];
uint16_t gpio_voltages[TOTAL_IC][6];  // 2D Array to hold GPIO voltages being read in; voltages are read in with the base unit as 100μV
float gpio_temps[TOTAL_IC][6];        // 2D Array to hold GPIO temperatures being read in; temperatures are read in with the base unit as K
int max_board_temp_location[2];       // [0]: IC#; [1]: Cell#
int min_board_temp_location[2];       // [0]: IC#; [1]: Cell#
int max_thermistor_location[2];       // [0]: IC#; [1]: Cell#
int max_humidity_location[2];         // [0]: IC#; [1]: Cell#
int min_thermistor_location[2];       // [0]: IC#; [1]: Cell#
uint16_t max_humidity = 0;
uint16_t max_thermistor_voltage = 0;
uint16_t min_thermistor_voltage = 65535;
uint16_t max_board_temp_voltage = 0;
uint16_t min_board_temp_voltage = 65535;
float total_board_temps = 0;
float total_thermistor_temps = 0;

void parse_detailed_voltages(const CAN_message_t& rx_msg) {
  BMS_detailed_voltages detailed_voltages_message;
  detailed_voltages_message.load(rx_msg.buf);
  cell_voltages[detailed_voltages_message.get_ic_id()][(detailed_voltages_message.get_group_id() * 3)] = detailed_voltages_message.get_voltage_0();
  cell_voltages[detailed_voltages_message.get_ic_id()][(detailed_voltages_message.get_group_id() * 3) + 1] = detailed_voltages_message.get_voltage_1();
  cell_voltages[detailed_voltages_message.get_ic_id()][(detailed_voltages_message.get_group_id() * 3) + 2] = detailed_voltages_message.get_voltage_2();
  detailed_voltages[detailed_voltages_message.get_ic_id()][detailed_voltages_message.get_group_id()].load(rx_msg.buf);
}

void parse_voltages(const CAN_message_t& rx_msg) {
  voltages.load(rx_msg.buf);
}

void parse_temperature(const CAN_message_t& rx_msg) {
  temperatures.load(rx_msg.buf);
}
void parse_mc_temps1(const CAN_message_t& rx_msg) {
  mc_temps1.load(rx_msg.buf);
}
void parse_mc_temps2(const CAN_message_t& rx_msg) {
  mc_temps2.load(rx_msg.buf);
}
void parse_mc_temps3(const CAN_message_t& rx_msg) {
  mc_temps3.load(rx_msg.buf);
}
void parse_mc_temps4(const CAN_message_t& rx_msg) {
  mc_temps4.load(rx_msg.buf);
}

void write_voltages() {
  uint64_t current_time = getTime();
  //Evens
  for (size_t i = 0; i < TOTAL_IC, i += 2) {
    for (size_t j = 0; j < 4; j++) {
      NRF.print(current_time);
      NRF.print(",");
      NRF.print(ID_BMS_DETAILED_VOLTAGES, HEX);
      NRF.print(",");
      NRF.print("8");
      NRF.print(",");
      uint8_t* buf = (uint8_t *) (&(detailed_voltages[i][j]);
      for (int i = 0; i < 8; i++) {
        if (buf[i] < 16) {
          NRF.print("0");
        }
        NRF.print(buf[i], HEX);
      }
      NRF.println();
    }
  }
  NRF.println();
  //Odds
  for (size_t i = 1; i < TOTAL_IC, i += 2) {
    for (size_t j = 0; j < 3; j++) {
      NRF.print(current_time);
      NRF.print(",");
      NRF.print(msg->id, HEX);
      NRF.print(",");
      NRF.print(msg->len);
      NRF.print(",");
      uint8_t* buf = (uint8_t *) (&(detailed_voltages[i][j]);
      for (int i = 0; i < msg->len; i++) {
        if (buf[i] < 16) {
          NRF.print("0");
        }
        NRF.print(buf[i], HEX);
      }
      NRF.println();
    }
  }

  // Data print functions
  // Print cell voltages
  void print_voltages_temps() {
    ESP.println("---------------------------------------------------------------");
    ESP.print("Total pack voltage: ");
    ESP.print(voltages.get_total() / 100.0, 4);
    ESP.print("V\t");
    ESP.print("Max voltage differential: ");
    ESP.print(voltages.get_high() / 10000.0 - voltages.get_low() / 10000.0, 4);
    ESP.println("V");
    ESP.print("Max V: ");
    ESP.print(voltages.get_high() / 10000.0, 4);
    ESP.print("V \t ");
    ESP.print("Min V: ");
    ESP.print(voltages.get_low() / 10000.0, 4);
    ESP.print("V \t");
    ESP.print("Avg V: ");
    ESP.print(voltages.get_average() / 10000.0, 4);
    ESP.println("V \t");
    ESP.println("Raw Cell Voltages\t\t\t\t\t\t\t\t\t\t\t\t\tBalancing Status");
    ESP.print("\tC0\tC1\tC2\tC3\tC4\tC5\tC6\tC7\tC8\tC9\tC10\tC11\t\t");
    for (int ic = 0; ic < TOTAL_IC; ic++) {
      ESP.print("IC");
      ESP.print(ic);
      ESP.print("\t");
      for (int cell = 0; cell < EVEN_IC_CELLS; cell++) {
        ESP.print(cell_voltages[ic][cell] / 10000.0, 4);
        ESP.print("V\t");
      }
      ESP.println();
    }
  }

  void print_temps() {
    ESP.println("---------------------------------------------------------------");
    ESP.printf("BMS max temp: %u\n", temperatures.get_high_temperature() / 100.);
    ESP.printf("Motor 1: %u Motor 2: %u Motor 3: %u Motor 4: %u\n", mc_temps1.get_motor_temp() / 10., mc_temps2.get_motor_temp() / 10., mc_temps3.get_motor_temp() / 10., mc_temps4.get_motor_temp() / 10.);
    ESP.printf("Inverter 1: %u Inverter 2: %u Inverter 3: %u Inverter 4: %u\n", mc_temps1.get_inverter_temp() / 10., mc_temps2.get_inverter_temp() / 10., mc_temps3.get_inverter_temp() / 10., mc_temps4.get_inverter_temp() / 10.);
  }
