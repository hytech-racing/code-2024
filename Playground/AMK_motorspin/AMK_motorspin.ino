/*
   Test program that sends and receives CAN messages. Use this to test if CAN Bus is operational.
   Created by Nathan Cheek, December 20, 2017
*/
//#include <HyTech_FlexCAN.h>
#include <HyTech_CAN.h>

#include <FlexCAN_T4.h>
#include <Metro.h>

// FlexCAN CAN(500000);
FlexCAN_T4<CAN2> CAN;
CAN_message_t msg;

Metro timer_can = Metro(20);
Metro debug_print = Metro(500);
Metro timer_light = Metro(3);
Metro error_toggle = Metro (50);

MC_status mc_status;
MC_setpoints_command mc_setpoints_command;

bool system_ready = false;
bool dc_done = false;
bool ready1 = false;
bool error = false;
bool toggle = false;

int counter = 0;
int counter2 = 0;

void setup() {
  Serial.begin(115200); // Initialize serial for PC communication
  CAN.begin();
  CAN.setBaudRate(500000);
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
  //  if(/*mc_status.get_error() &&*/ !toggle){
  //    mc_setpoints_command.set_remove_error(true);
  //  }
  //  else{
  //  mc_setpoints_command.set_remove_error(false);
  //  }

  if (system_ready) {
    mc_setpoints_command.set_hv_enable(true);
  }
  else {
    mc_setpoints_command.set_hv_enable(false);
    mc_setpoints_command.set_driver_enable(false);
    mc_setpoints_command.set_inverter_enable(false);
    mc_setpoints_command.set_speed_setpoint(0);
    mc_setpoints_command.set_pos_torque_limit(0);
    mc_setpoints_command.set_neg_torque_limit(0);
  }


  if (dc_done) {
    mc_setpoints_command.set_driver_enable(true);
    mc_setpoints_command.set_inverter_enable(true);
  }
  else {
    mc_setpoints_command.set_driver_enable(false);
    mc_setpoints_command.set_inverter_enable(false);
  }

  if (ready1) {
    mc_setpoints_command.set_speed_setpoint(120);
    mc_setpoints_command.set_pos_torque_limit(1000);
    mc_setpoints_command.set_neg_torque_limit(0);
  } else {
    mc_setpoints_command.set_speed_setpoint(0);
    mc_setpoints_command.set_pos_torque_limit(0);
    mc_setpoints_command.set_neg_torque_limit(0);
  }

  if (debug_print.check()) {
    Serial.print("System Ready: ");
    Serial.println(mc_status.get_system_ready());
    //    Serial.print("Quit DC ON: ");
    //    Serial.println(mc_status.get_quit_dc_on());
    //    Serial.print("DC ON: ");
    //    Serial.println(mc_status.get_dc_on());
    //    Serial.print("Quit Inverter ON: ");
    //    Serial.println(mc_status.get_quit_inverter_on());
    //    Serial.print("Error: ");
    //    Serial.println(mc_status.get_error());
    //    Serial.print("toogle: ");
    //    Serial.println(toggle);
    Serial.print("error: ");
    Serial.println(mc_status.get_error());
    Serial.print("speed: ");
    Serial.println(mc_status.get_speed());
    Serial.print("torque current: ");
    Serial.println(mc_status.get_torque_current());
    Serial.print("status: ");
    Serial.println(mc_status.get_status_word());
    Serial.println();

  }
  if (mc_status.get_quit_dc_on()) {
    dc_done = true;
  } else {
    dc_done = false;
  }
  if (mc_status.get_system_ready()) {
    system_ready = true;
  } else {
    system_ready = false;
  }
  if (mc_status.get_quit_inverter_on()) {
    ready1 = true;
  } else {
    ready1 = false;
  }
  if (mc_status.get_error()) {
    error = true;
  } else {
    error = false;
  }


  if (timer_can.check()) { // Send a message on CAN


    msg.id = 0xB3;
    msg.len = sizeof(mc_setpoints_command);
    mc_setpoints_command.write(msg.buf);
    CAN.write(msg);



    counter++;
    if (counter == 55) {
      toggle = !toggle;
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
    counter2++;
    if (counter2 == 501) {
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

    if (msg.id == 0x0A3) {
      mc_status.load(msg.buf);
    }

    digitalWrite(13, HIGH);
    timer_light.reset();
  }
  if (timer_light.check()) { // Turn off LED
    digitalWrite(13, LOW);
  }
}
