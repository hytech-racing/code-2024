/*
 * Teensy 4.0 code for rear Sensor Acquisition Board rev5
 */

/**
 *  Include files 
 */

/* Framework */
#include <Arduino.h>
#include <stdint.h>

/* Libraries */
#include "FlexCAN_T4.h"
#include "HyTech_CAN.h"

/* Constants */
#include "rear_sab_dfs.h"

/* Interfaces */
#include "HytechCANInterface.h"
#include "MCP_ADC.h"
#include "Filter_IIR.h"
#include "TelemetryInterface.h"

/* Systems */
#include "DebouncedButton.h"
#include "SysClock.h"

/**
 * Data source
*/

/* One CAN line on rear SAB rev5 */
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> TELEM_CAN;  // Telemetry

/* Sensors */
// ADCs
struct ADCs
{
  MCP_ADC<4> a1 = MCP_ADC<4>(ADC1_CS);  // RL corner board
  MCP_ADC<4> a2 = MCP_ADC<4>(ADC2_CS);  // RR corner board
  MCP_ADC<8> a3 = MCP_ADC<8>(ADC3_CS);  // Thermistors
} ADC;
// Pi shutdown
DebouncedButton btn_pi_shutdown;
// VectorNav

/**
 * Interfaces
*/
TelemetryInterface telem_interface(&CAN2_txBuffer, {THERM_3, THERM_4, THERM_5, THERM_6, THERM_7, THERM_8, THERM_9,
                                                    SUS_POT_3, SUS_POT_4, RL_LOAD_CELL, RR_LOAD_CELL});

/* Metro timers */
// Sensor read
Metro timer_read_all_adcs = Metro(10);
Metro timer_read_imu = Metro(20);

/* Sensor declare */


/* DSP utilities */
Filter_IIR thermistor_iir = Filter_IIR(THERM_ALPHA);  // IIR filter thermistor reading

/* Function prototypes */
void parse_telem_can_message(const CAN_message_t &RX_msg);

void setup() {

  // Initialize debounced button
  btn_pi_shutdown.begin(PI_SHUTDOWN, 100);

  // RS232
  Serial1.begin(IMU_RS232_SPEED);

}

void loop() {
  // CAN
  TELEM_CAN.events();
  // ADC
  read_all_adcs();
  // IIR filter thermistors
  


}

/**
 * Initialize CAN lines 
*/
void init_all_CAN_devices() {
  // Telemetry CAN line
  TELEM_CAN.begin();
  TELEM_CAN.setBaudRate(TELEM_CAN_BAUDRATE);
  TELEM_CAN.setMaxMB(16);
  TELEM_CAN.enableFIFO();
  TELEM_CAN.enableFIFOInterrupt();
  TELEM_CAN.onReceive(on_can2_receive);
  TELEM_CAN.mailboxStatus();

  // delay(500);
}

/**
 * Process Rx buffer
 * Prototype. Not needed atm.
*/
void process_ring_buffer(CANBufferType &rx_buffer, unsigned long curr_millis) {
  while (rx_buffer.available()) {
    CAN_message_t recvd_msg;
    uint8_t buf[sizeof(CAN_message_t)];
    rx_buffer.pop_front(buf, sizeof(CAN_message_t));
    memmove(&recvd_msg, buf, sizeof(recvd_msg));
    switch (recvd_msg.id)
    {
    case 0:
      /* code */
      break;
    
    default:
      break;
    }
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

