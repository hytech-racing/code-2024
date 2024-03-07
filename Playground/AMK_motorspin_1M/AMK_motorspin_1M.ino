/*
   Test program that sends and receives CAN messages. Use this to test if CAN Bus is operational.
   Created by Nathan Cheek, December 20, 2017
*/
//#include <HyTech_FlexCAN.h>
#include <HyTech_CAN.h>

#include <FlexCAN_T4.h>
#include <Metro.h>

// Insert shutdown stuff
#include "MCU_rev15_dfs.h"

// FlexCAN CAN(500000);
FlexCAN_T4<CAN2> CAN;
CAN_message_t msg;

Metro timer_can = Metro(20);
Metro debug_print = Metro(500);
Metro timer_light = Metro(3);
Metro error_toggle = Metro (50);

// Insert shutdown stuff
Metro timer_watchdog_timer = Metro(10);

//MC_status mc_status;
//MC_setpoints_command mc_setpoints_command;
MC_status mc_status[4];
MC_setpoints_command mc_setpoints_command[4];
MC_temps mc_temps[4];
MC_energy mc_energy[4];

bool system_ready[4] = {false, false, false, false};
bool dc_done[4] = {false, false, false, false};
bool ready1[4] = {false, false, false, false};
bool error[4] = {false, false, false, false};
bool toggle[4] = {false, false, false, false};
uint32_t read_id[4] = {0x0A0, 0x0A1, 0x0A2, 0x0A3};
uint32_t send_id[4] = {0xB0, 0xB1, 0xB2, 0xB3};

int counter[4] = {0, 0, 0, 0};
int counter2[4] = {0, 0, 0, 0};

void setup() {
  Serial.begin(115200); // Initialize serial for PC communication
  CAN.begin();
  CAN.setBaudRate(1000000);
  CAN.enableMBInterrupts();
  CAN.onReceive(parse_can_message);
  delay(200);
  Serial.println("CAN transceiver initialized");
  Serial.println("CAN TEST SENDER/RECEIVER");
  pinMode(13, OUTPUT);

// find
  for (int i = 1; i < 2; i++) {
    mc_status[i].set_status_word(0x00);
    mc_setpoints_command[i].set_inverter_enable(false);
    mc_setpoints_command[i].set_hv_enable(false);
    mc_setpoints_command[i].set_driver_enable(false);
    mc_setpoints_command[i].set_speed_setpoint(0);
    mc_setpoints_command[i].set_pos_torque_limit(0);
    mc_setpoints_command[i].set_neg_torque_limit(0);
    // CAN0_MCR &= 0xFFFDFFFF; // Enable self-reception
  }



  // Insert shutdown stuff
  pinMode(BRAKE_LIGHT_CTRL, OUTPUT);
  pinMode(FR_CS, OUTPUT);
  pinMode(FL_CS, OUTPUT);
  pinMode(ADC_CS, OUTPUT);
  
  // change to input if comparator is PUSH PULL
  pinMode(INVERTER_EN, OUTPUT);
  pinMode(INVERTER_24V_EN, OUTPUT);

  pinMode(WATCHDOG_INPUT, OUTPUT);
  // the initial state of the watchdog is high
  // this is reflected in the static watchdog_state
  // starting high
  digitalWrite(WATCHDOG_INPUT, HIGH);
  pinMode(SOFTWARE_OK, OUTPUT);
  digitalWrite(SOFTWARE_OK, HIGH);
  digitalWrite(INVERTER_24V_EN, HIGH);
  digitalWrite(INVERTER_EN, HIGH);
  digitalWrite(FR_CS, HIGH);
  digitalWrite(FL_CS, HIGH);
  digitalWrite(ADC_CS, HIGH);
  delay(5000);
  
}

void loop() {
  //  if(/*mc_status.get_error() &&*/ !toggle){
  //    mc_setpoints_command.set_remove_error(true);
  //  }
  //  else{
  //  mc_setpoints_command.set_remove_error(false);
  //  }

  CAN.events();

  send_CAN_inverter_setpoints();

// find
  for (int i = 1; i < 2; i++) {
    if (system_ready[i]) {
      mc_setpoints_command[i].set_hv_enable(true);
    }
    else {
      mc_setpoints_command[i].set_hv_enable(false);
      mc_setpoints_command[i].set_driver_enable(false);
      mc_setpoints_command[i].set_inverter_enable(false);
      mc_setpoints_command[i].set_speed_setpoint(0);
      mc_setpoints_command[i].set_pos_torque_limit(0);
      mc_setpoints_command[i].set_neg_torque_limit(0);
    }
  
  
    if (dc_done[i]) {
      mc_setpoints_command[i].set_driver_enable(true);
      mc_setpoints_command[i].set_inverter_enable(true);
    }
    else {
      mc_setpoints_command[i].set_driver_enable(false);
      mc_setpoints_command[i].set_inverter_enable(false);
    }
  
    if (ready1[i]) {
      mc_setpoints_command[i].set_speed_setpoint(280);
      mc_setpoints_command[i].set_pos_torque_limit(2140);
      mc_setpoints_command[i].set_neg_torque_limit(0);
    } else {
      mc_setpoints_command[i].set_speed_setpoint(0);
      mc_setpoints_command[i].set_pos_torque_limit(0);
      mc_setpoints_command[i].set_neg_torque_limit(0);
    }

    if (debug_print.check()) {
      Serial.print("System Ready: ");
      Serial.print("Inv[");
      Serial.print(i);
      Serial.println("]:");
      Serial.println("Pos torque limit:");
      Serial.println(mc_setpoints_command[i].get_pos_torque_limit());
//      Serial.println();
      Serial.print("System ready: ");
      Serial.println(mc_status[i].get_system_ready());
      Serial.print("Quit DC ON: ");
      Serial.println(mc_status[i].get_quit_dc_on());
      Serial.print("DC ON: ");
      Serial.println(mc_status[i].get_dc_on());
      Serial.print("Quit Inverter ON: ");
      Serial.println(mc_status[i].get_quit_inverter_on());
      Serial.print("Error: ");
      Serial.println(mc_status[i].get_error());
//      Serial.print("toogle: ");
//      Serial.println(toggle);
      Serial.print("error: ");
      Serial.println(mc_status[i].get_error());
      Serial.print("speed: ");
      Serial.println(mc_status[i].get_speed());
      Serial.print("torque current: ");
      Serial.println(mc_status[i].get_torque_current());
      Serial.print("status: ");
      Serial.println(mc_status[i].get_status_word());
      Serial.print("igbt temperature: ");
      Serial.println((uint16_t)((mc_temps[i].get_igbt_temp() - 48) * 0.1));
      Serial.println();
  
      if (mc_status[i].get_quit_dc_on()) {
        dc_done[i] = true;
      } else {
        dc_done[i] = false;
      }
      if (mc_status[i].get_system_ready()) {
        system_ready[i] = true;
      } else {
        system_ready[i] = false;
      }
      if (mc_status[i].get_quit_inverter_on()) {
        ready1[i] = true;
      } else {
        ready1[i] = false;
      }
      if (mc_status[i].get_error()) {
        error[i] = true;
      } else {
        error[i] = false;
      }
    }
  


//  if (timer_can.check()) { // Send a message on CAN
//
//
//    msg.id = send_id[i];
//    msg.len = sizeof(mc_setpoints_command[i]);
//    mc_setpoints_command[i].write(msg.buf);
//    CAN.write(msg);
//
//
//
//    counter[i]++;
//    if (counter[i] == 55) {
//      toggle[i] = !toggle[i];
//      Serial.print("Sent 0x");
//      Serial.print(msg.id, HEX);
//      Serial.print(": ");
//      for (unsigned int i = 0; i < msg.len; i++) {
//        Serial.print(msg.buf[i]);
//        Serial.print(" ");
//      }
//      Serial.println();
//      counter[i] = 0;
//    }
//
//
//
//  }

//  while (CAN.read(msg)) { // Receive a message on CAN
//    counter2[i]++;
//    if (counter2[i] == 501) {
//      Serial.print("Received 0x");
//      Serial.print(msg.id, HEX);
//      Serial.print(": ");
//      for (unsigned int i = 0; i < msg.len; i++) {
//        Serial.print(msg.buf[i], HEX);
//        Serial.print(" ");
//      }
//      Serial.println();
//      counter2[i] = 0;
//    }
//
//    if (msg.id == read_id[i]) {
//      mc_status[i].load(msg.buf);
//    }
//
//    digitalWrite(13, HIGH);
//    timer_light.reset();
//  }
}
  if (timer_light.check()) { // Turn off LED
    digitalWrite(13, LOW);
  }

  // Insert shutdown stuff
  software_shutdown();
}

inline void software_shutdown() {
  digitalWrite(SOFTWARE_OK, HIGH);
  //mcu_status.set_software_is_ok(true);

  // check inputs
  // BMS heartbeat has not arrived within time interval

  // add BMS software checks
  // software ok/not ok action
//  if (mcu_status.get_software_is_ok()) {
//    digitalWrite(SOFTWARE_OK, HIGH); //eventually make this HIGH only if software is ok
//  } else {
//    digitalWrite(SOFTWARE_OK, LOW);
//  }
  /* Watchdog timer */
  if (timer_watchdog_timer.check()) {
    static bool watchdog_state = HIGH;
    watchdog_state = !watchdog_state;
    digitalWrite(WATCHDOG_INPUT, watchdog_state);
  }

}

inline void send_CAN_inverter_setpoints() {
  if (timer_can.check()) {
    mc_setpoints_command[0].write(msg.buf);
    msg.id = ID_MC1_SETPOINTS_COMMAND;
    msg.len = sizeof(mc_setpoints_command[0]);
    CAN.write(msg);

    mc_setpoints_command[1].write(msg.buf);
    msg.id = ID_MC2_SETPOINTS_COMMAND;
    msg.len = sizeof(mc_setpoints_command[1]);
    CAN.write(msg);

    mc_setpoints_command[2].write(msg.buf);
    msg.id = ID_MC3_SETPOINTS_COMMAND;
    msg.len = sizeof(mc_setpoints_command[2]);
    CAN.write(msg);

    mc_setpoints_command[3].write(msg.buf);
    msg.id = ID_MC4_SETPOINTS_COMMAND;
    msg.len = sizeof(mc_setpoints_command[3]);
    CAN.write(msg);
  }
}

void parse_can_message(const CAN_message_t &RX_msg) {
  CAN_message_t rx_msg = RX_msg;
  switch (rx_msg.id) {
    case ID_MC1_STATUS:       mc_status[0].load(rx_msg.buf);    break;
    case ID_MC2_STATUS:       mc_status[1].load(rx_msg.buf);    break;
    case ID_MC3_STATUS:       mc_status[2].load(rx_msg.buf);    break;
    case ID_MC4_STATUS:       mc_status[3].load(rx_msg.buf);    break;

    case ID_MC1_TEMPS:        mc_temps[0].load(rx_msg.buf);    break;
    case ID_MC2_TEMPS:        mc_temps[1].load(rx_msg.buf);    break;
    case ID_MC3_TEMPS:        mc_temps[2].load(rx_msg.buf);    break;
    case ID_MC4_TEMPS:        mc_temps[3].load(rx_msg.buf);    break;
    
    case ID_MC1_ENERGY:       mc_energy[0].load(rx_msg.buf);    break;
    case ID_MC2_ENERGY:       mc_energy[1].load(rx_msg.buf);    break;
    case ID_MC3_ENERGY:       mc_energy[2].load(rx_msg.buf);    break;
    case ID_MC4_ENERGY:       mc_energy[3].load(rx_msg.buf);    break;
  }
}
