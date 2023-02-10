/*
   Test program that sends and receives CAN messages. Use this to test if CAN Bus is operational.
   Created by Nathan Cheek, December 20, 2017
*/
#include <HyTech_FlexCAN.h>
#include <HyTech_CAN.h>
#include <Metro.h>

FlexCAN CAN(500000);
CAN_message_t msg;
Metro timer_can = Metro(20);
Metro debug_print = Metro(2000);
Metro timer_light = Metro(3);

MC_status mc_status;
MC_setpoints_command mc_setpoints_command;

bool system_ready = false;
bool dc_done = false;
bool ready = false;

int counter = 0;
int counter2 = 0;

void setup() {
  Serial.begin(115200); // Initialize serial for PC communication
  CAN.begin();
  delay(200);
  Serial.println("CAN transceiver initialized");
  Serial.println("CAN TEST SENDER/RECEIVER");
  pinMode(13, OUTPUT);
  mc_status.set_status_word(0x00);
  mc_setpoints_command.set_inverter_enable(false);
  mc_setpoints_command.set_hv_enable(false);
  mc_setpoints_command.set_driver_enable(false);
  mc_setpoints_command.set_speed_setpoint(0);
  mc_setpoints_command.set_pos_torque_limit(0);
  mc_setpoints_command.set_neg_torque_limit(0);
  // CAN0_MCR &= 0xFFFDFFFF; // Enable self-reception
}

void loop() {


  if (system_ready) {
    mc_setpoints_command.set_hv_enable(true);
  }
  if (dc_done) {
    mc_setpoints_command.set_driver_enable(true);
    mc_setpoints_command.set_inverter_enable(true);
  }
  if (ready) {
    mc_setpoints_command.set_speed_setpoint(60);
    mc_setpoints_command.set_pos_torque_limit(100);
    mc_setpoints_command.set_neg_torque_limit(0);
  }

  if (debug_print.check()) {
    Serial.print("System Ready: ");
    Serial.println(mc_status.get_system_ready());
    Serial.print("Quit DC ON: ");
    Serial.println(mc_status.get_quit_dc_on());
    Serial.print("DC ON: ");
    Serial.println(mc_status.get_dc_on());
    Serial.print("Quit Inverter ON: ");
    Serial.println(mc_status.get_quit_inverter_on());
  }
  if (mc_status.get_quit_dc_on()) {
    dc_done = true;
  } else {
    dc_done = false;
  }
  if (mc_status.get_system_ready()) {
    system_ready = true;
  }else{
    system_ready = false;
  }
  if (mc_status.get_quit_inverter_on()) {
    ready = true;
  }else{
    ready = false;
  }


  if (timer_can.check()) { // Send a message on CAN


    msg.id = 0x184;
    msg.len = sizeof(mc_setpoints_command);
    mc_setpoints_command.write(msg.buf);
    CAN.write(msg);

    counter++;
    if (counter == 10) {
      Serial.print("Sent 0x");
      Serial.print(msg.id, HEX);
      Serial.print(": ");
      for (unsigned int i = 0; i < msg.len; i++) {
        Serial.print(msg.buf[i]);
        Serial.print(" ");
      }
      Serial.println();
      counter = 0;
    }



  }

  while (CAN.read(msg)) { // Receive a message on CAN
    if (msg.id == 0x283) {
      mc_status.load(msg.buf);
      counter2++;
      if (counter2 == 90) {
        Serial.print("Received 0x");
        Serial.print(msg.id, HEX);
        Serial.print(": ");
        for (unsigned int i = 0; i < msg.len; i++) {
          Serial.print(msg.buf[i], HEX);
          Serial.print(" ");
        }
        Serial.println();
        counter2 = 0;
      }
    }



    digitalWrite(13, HIGH);
    timer_light.reset();
  }
  if (timer_light.check()) { // Turn off LED
    digitalWrite(13, LOW);
  }
}
