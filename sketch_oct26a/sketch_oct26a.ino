#include <FlexCAN_T4.h>
#include "HyTech_CAN.h"
#include <Metro.h>

FlexCAN_T4<CAN2> CAN;
CAN_message_t msg;
MCU_pedal_readings pedal{};
SAB_lap_times lap_times{};
int state = 0;

Metro state_change(5000);
Metro send_can(500);

void setup() {
  // Serial.begin(115200);

  CAN.begin();
  CAN.setBaudRate(500000);
  CAN.enableMBInterrupts();
}

void loop() {
  CAN.events();

  // if (state_change.check()) {
  //   state++;
  //   if (state >= 3) {
  //     state = 0;
  //   }
  // }

  // int time_1_type = 1;
  // int time_2_type = 2;
  // int time_1 = 1000 + millis();
  // lap_times.set_time_1(time_1);
  // int time_2 = millis();
  // lap_times.set_time_2(time_2);

  // lap_times.set_state(state);
  // lap_times.set_time_1_type(time_1_type);
  // lap_times.set_time_2_type(time_2_type);

  // if (send_can.check()) {
  //   msg.len = sizeof(lap_times);
  //   msg.id = ID_MCU_PEDAL_READINGS;

  //   lap_times.write(msg.buf);
  //   CAN.write(msg);
  //   send_can.reset();

  //   Serial.println(lap_times.get_message(), HEX);
  //   Serial.printf("Set State: \n\tSET: %d \t READ: %d\nTime 1 type: \n\tSET: %d \t READ: %d\nTime 2 type: \n\tSET: %d \t READ: %d\nTime 1:\n\tSET: %d \t READ: %d\nTIme 2:\n\tSET: %d \t READ: %d\n",state, lap_times.get_state(),time_1_type, lap_times.get_time_1_type(),time_2_type, lap_times.get_time_2_type(),time_1, lap_times.get_time_1(),time_2, lap_times.get_time_2());

  // } 


  


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
