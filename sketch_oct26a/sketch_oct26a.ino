#include <FlexCAN_T4.h>
#include "HyTech_CAN.h"

FlexCAN_T4<CAN2> CAN;
CAN_message_t msg;
MCU_pedal_readings pedal{};

void setup() {
  CAN.begin();
  CAN.setBaudRate(500000);
  CAN.enableMBInterrupts();
}

void loop() {
  CAN.events();
  msg.id = 0xC4; // pedal reading ID
  for(int i = 500; i < 2140; i+=10) {
    msg.len = sizeof(pedal);
    pedal.set_accelerator_pedal_1(i);
    pedal.write(msg.buf);
    CAN.write(msg);
    delay(10);
  }

  for(int i = 2140; i > 500; i-=10) {
    msg.len = sizeof(pedal);
    pedal.set_accelerator_pedal_1(i);
    pedal.write(msg.buf);
    CAN.write(msg);
    delay(10);
  }

}
