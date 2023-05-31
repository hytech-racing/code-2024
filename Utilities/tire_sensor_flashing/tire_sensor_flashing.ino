
// #include <HyTech_FlexCAN.h>
#include <FlexCAN_T4.h>
#include <Metro.h>

// FlexCAN CAN(500000);
FlexCAN_T4<CAN2> CAN;
CAN_message_t msg;
Metro timer_can = Metro(1000);
//Metro timer_st = Metro(10000);
//Metro timer_up = Metro(12000);
//bool startT = 0;
//bool timeUp = 0;

void setup() {
  Serial.begin(115200); // Initialize serial for PC communication
  CAN.begin();
  CAN.setBaudRate(1000000);
  delay(200);
  Serial.println("CAN transceiver initialized");
  Serial.println("CAN TEST SENDER/RECEIVER");
  pinMode(13, OUTPUT);


}

void loop() {
//  if (timer_st.check()) {
//    startT = 1;
//    timer_up.reset();
//  }


  if (timer_can.check()) { // Send a message on CAN at 1 hz
    msg.id = 0x424; // LF
    //msg.id = 0x42A; //RF
    //msg.id = 0x430; //LR
    //msg.id = 0x436; //RR

    msg.buf[0] = 0x75;
    msg.buf[1] = 0x30;
    msg.buf[2] = (uint8_t) msg.id >> 2;
    msg.buf[3] = (uint8_t) msg.id ^ 0x400;
    msg.buf[4] = 0x1;
    msg.buf[5] = 0x2;
    msg.buf[6] = 0x1;
    msg.buf[7] = 0;
    msg.len = sizeof(msg.buf);
    CAN.write(msg);
    Serial.print("Sent 0x");
    Serial.print(msg.id, HEX);
    Serial.print(": ");
    for (unsigned int i = 0; i < msg.len; i++) {
      Serial.print(msg.buf[i]);
      Serial.print(" ");
    }
    Serial.println();
//    if (timer_up.check()) {
//      Serial.println("Done");
//      timer_up = 1;
//    }
  }



}
