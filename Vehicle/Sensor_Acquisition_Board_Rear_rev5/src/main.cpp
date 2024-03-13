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
// #include "HyTechCANInterface.h"
#include "MCP_ADC.h"
// #include "TelemetryInterface.h"

/* Systems */
#include "SysClock.h"

/**
 * Utilities
*/
// #include "MessageQueueDefine.h"
#include "DebouncedButton.h"
#include "Filter_IIR.h"

/**
 * Data source
*/

/* One CAN line on rear SAB rev5 */
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> TELEM_CAN;  // Telemetry

/* Outbound CAN message */
SAB_thermistors_1 sab_thermistors_1;
SAB_thermistors_2 sab_thermistors_2;
SAB_CB sab_cb;
TCU_status tcu_status;

/* Inbound CAN message */
MCU_status mcu_status;

/* Sensors */
// ADCs
MCP_ADC<4> ADC1 = MCP_ADC<4>(ADC1_CS, ADC_ISOSPI_SPEED);  // RL corner board
MCP_ADC<4> ADC2 = MCP_ADC<4>(ADC2_CS, ADC_ISOSPI_SPEED);  // RR corner board
MCP_ADC<8> ADC3 = MCP_ADC<8>(ADC3_CS, ADC_SPI_SPEED);  // Thermistors

// ADC_SPI ADC1(ADC1_CS, ADC_SPI_SPEED);


// Pi shutdown
DebouncedButton btn_pi_shutdown;
// VectorNav

/**
 * Interfaces
*/
// TelemetryInterface telem_interface(&CAN2_txBuffer, {THERM_3, THERM_4, THERM_5, THERM_6, THERM_7, THERM_8, THERM_9,
//                                                     SUS_POT_3, SUS_POT_4, RL_LOAD_CELL, RR_LOAD_CELL});

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
void parse_telem_CAN_msg(const CAN_message_t &RX_msg);
void update_all_CAN_msg();
void send_sab_CAN_msg();
// void process_ring_buffer(CANBufferType &rx_buffer, unsigned long curr_millis);
// void send_all_CAN_msg(CANBufferType &tx_buffer, FlexCAN_T4_Base *can_interface);
void init_all_adcs();
void init_all_iir_filters();
void tick_all_interfaces(const SysTick_s &curr_tick);
void tick_all_systems(const SysTick_s &curr_tick);

void setup() {

  // Tick system clock
  SysTick_s curr_tick = sys_clock.tick(micros());

  // Debug print
  Serial.begin(DEBUG_PRINT_BAUDRATE);
  Serial.println("Starting...");

  // Initialize CAN
  init_all_CAN_devices();

  // Initialize all SPI devices
  init_all_adcs();
  Serial.println("SPI initialized");

  // Set IIR filter alpha for thermistors
  init_all_iir_filters();
  Serial.println("IIR filter initialized");

  // Initialize debounced button
  btn_pi_shutdown.begin(PI_SHUTDOWN, 100);
  Serial.println("Debounce button initialized for Pi shutdown");

  // RS232
  Serial1.begin(IMU_RS232_SPEED);
  Serial.println("VectorNav initialized ... in theory ^^ coming soon");
  Serial.println();

}

void loop() {

  // Tick system clock
  SysTick_s curr_tick = sys_clock.tick(micros());

  // Process received CAN messages
  // Not currently needed
  TELEM_CAN.events();

  // Tick interfaces
  tick_all_interfaces(curr_tick);

  // Tick systems
  // Not currently needed

  // Send outbound CAN messages
  // send_all_CAN_msg(CAN2_txBuffer, &TELEM_CAN);

  // Debug prints to see if we're tripping balls
  TriggerBits_s t = curr_tick.triggers;
  if (t.trigger5) {
    Serial.println("Thermistors:");
    Serial.println(ADC3.get().conversions[THERM_3].raw);
    Serial.println(ADC3.get().conversions[THERM_4].raw);
    Serial.println(ADC3.get().conversions[THERM_5].raw);
    Serial.println(ADC3.get().conversions[THERM_6].raw);
    Serial.println(ADC3.get().conversions[THERM_7].raw);
    Serial.println(ADC3.get().conversions[THERM_8].raw);
    Serial.println(ADC3.get().conversions[THERM_9].raw);
    Serial.println();
    Serial.println("Load cells:");
    Serial.println(ADC1.get().conversions[RL_LOAD_CELL].raw);
    Serial.println(ADC2.get().conversions[RR_LOAD_CELL].raw);
    Serial.println();
    Serial.println("Sus pots:");
    Serial.println(ADC1.get().conversions[SUS_POT_3].raw);
    Serial.println(ADC2.get().conversions[SUS_POT_4].raw);
    Serial.println();
    Serial.println();
  }

}

/**
 * Initialize CAN lines 
*/
void init_all_CAN_devices() {
  // Telemetry CAN line
  TELEM_CAN.begin();
  TELEM_CAN.setBaudRate(TELEM_CAN_BAUDRATE);
  TELEM_CAN.enableMBInterrupts();
  TELEM_CAN.onReceive(parse_telem_CAN_msg);

  // delay(500);
}

/**
 * Send CAN function
*/
// void send_all_CAN_msg(CANBufferType &tx_buffer, FlexCAN_T4_Base *can_interface) {
//     while (tx_buffer.available())
//     {
//         CAN_message_t msg;
//         uint8_t buf[sizeof(CAN_message_t)];
//         tx_buffer.pop_front(buf, sizeof(CAN_message_t));
//         memmove(&msg, buf, sizeof(msg));
//         can_interface->write(msg);
//     }    
// }

/**
 * Process Rx buffer
 * Prototype. Not needed atm.
*/
// void process_ring_buffer(CANBufferType &rx_buffer, unsigned long curr_millis) {
//   while (rx_buffer.available()) {
//     CAN_message_t recvd_msg;
//     uint8_t buf[sizeof(CAN_message_t)];
//     rx_buffer.pop_front(buf, sizeof(CAN_message_t));
//     memmove(&recvd_msg, buf, sizeof(recvd_msg));
//     switch (recvd_msg.id)
//     {
//     case 0:
//       /* code */
//       break;
    
//     default:
//       break;
//     }
//   }
// }

/**
 * Initialize all SPI devices
 * MCP ADCs
*/
void init_all_adcs() {

  SPI.begin();

  ADC1.init();
  ADC2.init();
  ADC3.init();

}

/**
 * Initialize all IIR filters
*/
void init_all_iir_filters() {
  for (int i = 0; i < TOTAL_THERMISTOR_COUNT; i++) {
    thermistor_iir[i].set_alpha(THERM_ALPHA);
  }
}

/* Prototype */
void parse_telem_CAN_msg(const CAN_message_t &RX_msg) {
  CAN_message_t rx_msg = RX_msg;
  switch (rx_msg.id) {
    case ID_MCU_STATUS:              
      mcu_status.load(rx_msg.buf);
      break;
  }
}

void update_all_CAN_msg() {

  sab_thermistors_1.set_thermistor3(thermistor_iir[THERM_3].get_prev_reading());
  sab_thermistors_1.set_thermistor4(thermistor_iir[THERM_4].get_prev_reading());
  sab_thermistors_1.set_thermistor5(thermistor_iir[THERM_5].get_prev_reading());
  sab_thermistors_1.set_thermistor6(thermistor_iir[THERM_6].get_prev_reading());

  sab_thermistors_2.set_thermistor7(thermistor_iir[THERM_7].get_prev_reading());
  sab_thermistors_2.set_thermistor8(thermistor_iir[THERM_8].get_prev_reading());
  sab_thermistors_2.set_thermistor9(thermistor_iir[THERM_9].get_prev_reading());

  sab_cb.set_pot3(ADC1.get().conversions[SUS_POT_3].raw);
  sab_cb.set_pot4(ADC2.get().conversions[SUS_POT_4].raw);
  sab_cb.set_RL_load_cell(ADC1.get().conversions[RL_LOAD_CELL].raw);
  sab_cb.set_RR_load_cell(ADC2.get().conversions[RR_LOAD_CELL].raw);

  tcu_status.set_shutdown_status(static_cast<uint16_t>(btn_pi_shutdown.isPressed()));

}

void send_sab_CAN_msg() {
  CAN_message_t msg;

  // Thermistors
  sab_thermistors_1.write(msg.buf);
  msg.id = ID_SAB_THERMISTORS_1;
  msg.len = sizeof(sab_thermistors_1);
  TELEM_CAN.write(msg);

  sab_thermistors_2.write(msg.buf);
  msg.id = ID_SAB_THERMISTORS_2;
  msg.len = sizeof(sab_thermistors_2);
  TELEM_CAN.write(msg);

  // Corner boards
  sab_cb.write(msg.buf);
  msg.id = ID_SAB_CB;
  msg.len = sizeof(sab_cb);
  TELEM_CAN.write(msg);

  // TCU status
  tcu_status.write(msg.buf);
  msg.id = ID_TCU_STATUS;
  msg.len = sizeof(tcu_status);
  TELEM_CAN.write(msg);
}

/**
 * Tick interfaces
*/
void tick_all_interfaces(const SysTick_s &curr_tick) {

  TriggerBits_s t = curr_tick.triggers;

  // Serial.println("Sus tick ADC");
  if (t.trigger100) { // 100Hz
    ADC1.tick();
    ADC2.tick();
    ADC3.tick();

    // Filter thermistor readings
    for (int i = 0; i < TOTAL_THERMISTOR_COUNT; i++) {
      thermistor_iir[i].filtered_result(ADC3.get().conversions[i].raw);
    }
  }
  // Serial.println("Sus after tick ADC");

  if (t.trigger50) {  // 50Hz
    // telem_interface.tick(ADC1.get(), ADC2.get(), ADC3.get(), btn_pi_shutdown.isPressed(), thermistor_iir);

    update_all_CAN_msg();
    send_sab_CAN_msg();
  }

}

/**
 * Tick systems
*/
void tick_all_systems(const SysTick_s &curr_tick) {
  
}



