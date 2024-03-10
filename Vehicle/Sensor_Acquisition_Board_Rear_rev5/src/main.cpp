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
#include "HyTechCANInterface.h"
#include "MCP_ADC.h"
#include "TelemetryInterface.h"

/* Systems */
#include "SysClock.h"

/**
 * Utilities
*/
#include "MessageQueueDefine.h"
#include "DebouncedButton.h"
#include "Filter_IIR.h"

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

/**
 * Systems
*/
SysClock sys_clock;

/* Metro timers */
// Sensor read
Metro timer_read_all_adcs = Metro(10);
Metro timer_read_imu = Metro(20);

/* Utilities */
// IIR filter for DSP
// Thermistors
Filter_IIR thermistor_iir[TOTAL_THERMISTOR_COUNT];
// Loadcells
// Filter_IIR loadcell_iir[TOTAL_LOADCELL_COUNT] = Filter_IIR{LOADCELL_ALPHA, LOADCELL_ALPHA};  // actually will be done by torque controllers themselves if needed

/* Function prototypes */
void init_all_CAN_devices();
void process_ring_buffer(CANBufferType &rx_buffer, unsigned long curr_millis);
void init_all_iir_filters();
void tick_all_interfaces(const SysTick_s &curr_tick);
void tick_all_systems(const SysTick_s &curr_tick);

void setup() {

  // Tick system clock
  SysTick_s curr_tick = sys_clock.tick(micros());

  // Set IIR filter alpha for thermistors
  init_all_iir_filters();

  // Initialize debounced button
  btn_pi_shutdown.begin(PI_SHUTDOWN, 100);

  // RS232
  Serial1.begin(IMU_RS232_SPEED);

}

void loop() {

  // Tick system clock
  SysTick_s curr_tick = sys_clock.tick(micros());

  // Process received CAN messages
  // Not currently needed

  // Tick interfaces
  tick_all_interfaces(curr_tick);

  // Tick systems
  // Not currently needed

  // Send outbound CAN messages
  send_all_CAN_msg(CAN2_txBuffer, &TELEM_CAN);

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

/**
 * Initialize all IIR filters
*/
void init_all_iir_filters() {
  for (int i = 0; i < TOTAL_THERMISTOR_COUNT; i++) {
    thermistor_iir[i].set_alpha(THERM_ALPHA);
  }
}

/**
 * Tick interfaces
*/
void tick_all_interfaces(const SysTick_s &curr_tick) {

  TriggerBits_s t = curr_tick.triggers;

  if (t.trigger100) { // 100Hz
    ADC.a1.tick();
    ADC.a2.tick();
    ADC.a3.tick();
  }

  if (t.trigger50) {  // 50Hz
    telem_interface.tick(ADC.a1.get(), ADC.a2.get(), ADC.a3.get(), btn_pi_shutdown.isPressed(), thermistor_iir);
  }

}

/**
 * Tick systems
*/
void tick_all_systems(const SysTick_s &curr_tick) {
  
}



