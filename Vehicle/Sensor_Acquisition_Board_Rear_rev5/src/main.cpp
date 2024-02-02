/*
 * Teensy 4.0 code for rear Sensor Acquisition Board rev5
 */
/* Header include */
// Framework
#include <Arduino.h>
// Constants
#include "rear_sab_dfs.h"
// CAN
#include <FlexCAN_T4.h>
#include <HyTech_CAN.h>
// Timer
#include "Metro.h"
// Sensors
#include "DebouncedButton.h"
#include "RideHeightRS485.h"
#include "ADC_SPI.h"

FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> TELEM_CAN;
CAN_message_t msg;

/* CAN messages */
BMS_coulomb_counts bms_coulomb_counts;
SAB_ride_heights sab_ride_heights;
SAB_thermistors_1 sab_thermistors_1;
SAB_thermistors_2 sab_thermistors_2;
SAB_CB sab_corner_board_readings;

/* Metro timers */
// CAN send
Metro timer_CAN_sab_loadcells_send = Metro(50);
Metro timer_CAN_sab_potentiometers_send = Metro(50);
Metro timer_CAN_sab_ride_height_send = Metro(50);
Metro timer_CAN_sab_thermistors_send = Metro(100);
Metro timer_CAN_sab_IMU_send = Metro(20);
Metro timer_CAN_TCU_status_send = Metro(20);
Metro timer_CAN_sab_pitot_tubes_send = Metro(50);
// Sensor read
Metro timer_read_all_adcs = Metro(10);
Metro timer_read_ride_height = Metro(20);
Metro timer_read_imu = Metro(20);

/* Sensor declare */
// ADC
ADC_SPI adc1(ADC1_CS, ADC_SPI_SPEED);   // RL CB
ADC_SPI adc2(ADC2_CS, ADC_SPI_SPEED);   // RR CB
ADC_SPI adc3(ADC3_CS, ADC_SPI_SPEED);   // SAB
// Pi shutdown
DebouncedButton btn_pi_shutdown;
// Ride height
RideHeightRS485 ride_height_sensor(RS485_SERIAL_SPEED);
// IMU
// tbd

/* DSP utilities */
Filter

/* Function prototypes */
void parse_telem_can_message(const CAN_message_t &RX_msg);

void setup() {


  // Initialize debounced button
  btn_pi_shutdown.begin(PI_SHUTDOWN, 100);

  // RS485
  ride_height_sensor.begin();

  // RS232
  Serial1.begin();

  // CAN
  TELEM_CAN.begin();
  TELEM_CAN.setBaudRate(500000);
  TELEM_CAN.enableMBInterrupts();
  TELEM_CAN.onReceive(parse_telem_can_message);
  delay(500);

}

void loop() {
  // CAN
  TELEM_CAN.events();
  // ADC
  read_all_adcs();
  // 


}

/* Parse incoming CAN message needed for SAB operation */
/* Prototype */
void parse_telem_can_message(const CAN_message_t &RX_msg) {
  CAN_message_t rx_msg = RX_msg;
  switch (rx_msg.id) {
    case ID_BMS_COULOMB_COUNTS:
      bms_coulomb_counts.load(rx_msg.buf);
      break;
    default:
      break;
  }
}

/* Read all analog sensors from ADC */
inline void read_all_adcs() {
  if (timer_read_all_adcs.check()) {
    /* RL CB */
    // Sus pot
    sab_corner_board_readings.set_pot3(adc1.read_channel(SUS_POT_3));
    // Loadcell
    sab_corner_board_readings.set_RL_load_cell(adc1.read_channel(RL_LOAD_CELL));
    /* RR CB */
    // Sus pot
    sab_corner_board_readings.set_pot4(adc2.read_channel(SUS_POT_4));
    // Loadcell
    sab_corner_board_readings.set_RR_load_cell(adc2.read_channel(RR_LOAD_CELL));
    /* SAB */
    // Thermistors
    sab_thermistors_1.set_thermistor3(adc3.read_channel(THERM_3));
    sab_thermistors_1.set_thermistor4(adc3.read_channel(THERM_4));
    sab_thermistors_1.set_thermistor5(adc3.read_channel(THERM_5));
    sab_thermistors_1.set_thermistor6(adc3.read_channel(THERM_6));
    sab_thermistors_2.set_thermistor7(adc3.read_channel(THERM_7));
    sab_thermistors_2.set_thermistor8(adc3.read_channel(THERM_8));
    sab_thermistors_2.set_thermistor9(adc3.read_channel(THERM_9));
  }
}

