/*
 * Teensy 4.0 code for rear Sensor Acquisition Board rev5
 */
#include <Arduino.h>

#include "rear_sab_dfs.h"

#include <FlexCAN_T4.h>
#include <HyTech_CAN.h>
#include "Metro.h"
#include "DebouncedButton.h"

FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> TELEM_CAN;
CAN_message_t msg;

DebouncedButton btn_pi_shutdown;



void setup() {


  // Initialize debounced button
  btn_pi_shutdown.begin(PI_SHUTDOWN, 100);

  // RS485
  Serial3.begin();

  // RS232
  Serial1.begin();

  // CAN
  TELEM_CAN.begin();
  TELEM_CAN.setBaudRate(500000);
  TELEM_CAN.enableMBInterrupts();
  TELEM_CAN.onReceive(parse_telem_can_message);

}

void loop() {
  // CAN
  TELEM_CAN.events();

}

void parse_telem_can_message(const CAN_message_t &RX_msg)
{
  CAN_message_t rx_msg = RX_msg;
  switch (rx_msg.id) {
    case 
  }
}

