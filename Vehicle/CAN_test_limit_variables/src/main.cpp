#include <Arduino.h>

#include "FlexCAN_T4.h"
#include "HyTech_CAN.h"

#include "rear_sab_dfs.h"

#include "SysClock.h"

/* One CAN line on rear SAB rev5 */
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> TELEM_CAN;  // Telemetry

/* Inbound CAN message */
MCU_status mcu_status;

MCP_ADC<4> ADC1 = MCP_ADC<4>(ADC1_CS);  // RL corner board
MCP_ADC<4> ADC2 = MCP_ADC<4>(ADC2_CS);  // RR corner board
MCP_ADC<8> ADC3 = MCP_ADC<8>(ADC3_CS);  // Thermistors

SysClock sys_clock;

void init_all_CAN_devices();
void send_sab_CAN_msg();
void parse_telem_CAN_msg(const CAN_message_t &RX_msg);
void init_all_adcs();
void tick_all_interfaces(const SysTick_s &curr_tick);


void setup() {

  SysTick_s curr_tick = sys_clock.tick(micros());

  // Telemetry CAN line
  init_all_CAN_devices();

  Serial.println("Init");

}

void loop() {

  SysTick_s curr_tick = sys_clock.tick(micros());
  
  // Events
  TELEM_CAN.events();

  // send_sab_CAN_msg();
  tick_all_interfaces(curr_tick);

  

}

void init_all_CAN_devices() {
  TELEM_CAN.begin();
  TELEM_CAN.setBaudRate(TELEM_CAN_BAUDRATE);
  TELEM_CAN.enableMBInterrupts();
  TELEM_CAN.onReceive(parse_telem_CAN_msg);  
}

void send_sab_CAN_msg() {

  CAN_message_t msg;

  mcu_status.write(msg.buf);
  msg.id = ID_MCU_STATUS;
  msg.len = sizeof(mcu_status);
  TELEM_CAN.write(msg);

}

void parse_telem_CAN_msg(const CAN_message_t &RX_msg) {

  CAN_message_t rx_msg = RX_msg;

  switch (rx_msg.id) {
    case ID_MCU_STATUS:              
      mcu_status.load(rx_msg.buf);
      break;
  }

}

void tick_all_interfaces(const SysTick_s &curr_tick) {

  TriggerBits_s t = curr_tick.triggers;

  if (t.trigger50) {
    // update_all_CAN_msg();
    send_sab_CAN_msg();
    Serial.println("I'm not stuck");
  }

}