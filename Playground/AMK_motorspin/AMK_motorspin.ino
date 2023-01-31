/*
   Teensy 4.1 Main Control Unit code
   Written by Liwei Sun which is why the code is so bad

   Rev 12
*/

#include <stdint.h>
#include <FlexCAN_T4.h>
#include <HyTech_CAN.h>
#define _USE_MATH_DEFINES
#include <cmath>

#include "ADC_SPI.h"
#include "kinetis_flexcan.h"
#include "Metro.h"


#include "drivers.h"

// constants to define for different operation

#define DRIVER DAVID
#define TORQUE_1 15
#define TORQUE_2 18
#define TORQUE_3 21


// set to true or false for debugging
#define DEBUG true
#define BMS_DEBUG_ENABLE false

#include "MCU_rev12_dfs.h"

#include "driver_constants.h"

MCU_status mcu_status;

MC_status mc_status[4];
MC_temps mc_temps[4];
MC_energy mc_energy[4];
MC_setpoints_command mc_setpoints_command[4];

//Timers
Metro timer_CAN_imu_accelerometer_send = Metro(50);
Metro timer_CAN_imu_gyroscope_send = Metro(50);
Metro timer_CAN_inverter_status_read = Metro(20);
Metro timer_CAN_inverter_temps_read = Metro(200);
Metro timer_CAN_inverter_energy_read = Metro(200);
Metro timer_CAN_inverter_setpoints_send = Metro(20);

Metro timer_CAN_coloumb_count_send = Metro(1000);
Metro timer_CAN_mc_status_forward = Metro(100);
Metro timer_CAN_mc_temps_forward = Metro(250);
Metro timer_CAN_mc_energy_forward = Metro(250);
Metro timer_CAN_mc_setpoints_command_forward = Metro(300);
Metro timer_CAN_mc_torque_command_forward = Metro(100);
Metro timer_ready_sound = Metro(2000); // Time to play RTD sound
Metro timer_CAN_mcu_status_send = Metro(100);
Metro timer_CAN_mcu_pedal_readings_send = Metro(5);
Metro timer_CAN_mcu_load_cells_send = Metro(10);
Metro timer_load_cells_read = Metro(5);
Metro timer_restart_inverter = Metro(500, 1); // Allow the MCU to restart the inverter
Metro timer_inverter_enable = Metro(5000);
Metro timer_status_send = Metro(100);
Metro timer_watchdog_timer = Metro(500);

// this abuses Metro timer functionality to stay faulting once a fault has occurred
// autoreset makes the timer update to the current time and not by the interval
// this allows me to set the interval as 0 once a fault has occurred, leading to continuous faulting
// until a CAN message comes in which resets the timer and the interval
Metro timer_bms_heartbeat = Metro(0, 1);

/*
   Variables to store filtered values from ADC channels
*/

bool imd_faulting = false;
bool inverter_restart = false; // True when restarting the inverter
INVERTER_STARTUP_STATE inverter_startup_state = INVERTER_STARTUP_STATE::WAIT_SYSTEM_READY;

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> FRONT_INV_CAN;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> REAR_INV_CAN;
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> TELEM_CAN;
CAN_message_t msg;

// coloumb counts
uint32_t total_discharge;
unsigned long previous_data_time;

void setup() {
  // no torque can be provided on startup

  set_all_inverters_disabled();

  mcu_status.set_max_torque(0);
  mcu_status.set_torque_mode(0);
  mcu_status.set_software_is_ok(true);

  set_state(MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE);

  pinMode(BRAKE_LIGHT_CTRL, OUTPUT);

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

#if DEBUG
  Serial.begin(115200);
#endif
  FRONT_INV_CAN.begin();
  FRONT_INV_CAN.setBaudRate(1000000);
  REAR_INV_CAN.begin();
  REAR_INV_CAN.setBaudRate(1000000);
  FRONT_INV_CAN.enableMBInterrupts();
  REAR_INV_CAN.enableMBInterrupts();
  FRONT_INV_CAN.onReceive(parse_front_inv_can_message);
  REAR_INV_CAN.onReceive(parse_rear_inv_can_message);
  delay(500);

#if DEBUG
  Serial.println("CAN system and serial communication initialized");
#endif


  digitalWrite(INVERTER_24V_EN, HIGH);



  // present action for 5s
  delay(5000);

  set_state(MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE);

  /* Set up total discharge readings */
  //setup_total_discharge();

}

void loop() {

  send_CAN_inverter_setpoints();


  /* Finish restarting the inverter when timer expires */
  if (timer_restart_inverter.check() && inverter_restart) {
    inverter_restart = false;
    digitalWrite(INVERTER_24V_EN, HIGH);
  }

  /* handle state functionality */
  state_machine();
}



inline void send_CAN_inverter_setpoints() {
  if (timer_CAN_inverter_setpoints_send.check()) {
    mc_setpoints_command[0].write(msg.buf);
    msg.id = ID_MC1_SETPOINTS_COMMAND;
    msg.len = sizeof(mc_setpoints_command[0]);
    FRONT_INV_CAN.write(msg);

    mc_setpoints_command[1].write(msg.buf);
    msg.id = ID_MC2_SETPOINTS_COMMAND;
    msg.len = sizeof(mc_setpoints_command[1]);
    FRONT_INV_CAN.write(msg);

    mc_setpoints_command[2].write(msg.buf);
    msg.id = ID_MC3_SETPOINTS_COMMAND;
    msg.len = sizeof(mc_setpoints_command[2]);
    REAR_INV_CAN.write(msg);

    mc_setpoints_command[3].write(msg.buf);
    msg.id = ID_MC4_SETPOINTS_COMMAND;
    msg.len = sizeof(mc_setpoints_command[3]);
    REAR_INV_CAN.write(msg);
  }
}




inline void state_machine() {
  switch (mcu_status.get_state()) {
    case MCU_STATE::STARTUP: break;

    case MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE:
#if DEBUG
      Serial.println("TS NOT ACTIVE");
#endif
      // if TS is above HV threshold, move to Tractive System Active
      if (check_TS_over_HV_threshold()) {
#if DEBUG
        Serial.println("Setting state to TS Active from TS Not Active");
#endif
        set_state(MCU_STATE::TRACTIVE_SYSTEM_ACTIVE);
      }
      break;

    case MCU_STATE::TRACTIVE_SYSTEM_ACTIVE:
      check_TS_active();

      set_state(MCU_STATE::ENABLING_INVERTER);
      break;

    case MCU_STATE::ENABLING_INVERTER:
      check_TS_active();
      // inverter enabling timed out
      if (timer_inverter_enable.check()) {
#if DEBUG
        Serial.println("Setting state to TS Active from Enabling Inverter");
#endif
        set_state(MCU_STATE::TRACTIVE_SYSTEM_ACTIVE);
      }

      switch (inverter_startup_state) {
        case INVERTER_STARTUP_STATE::WAIT_SYSTEM_READY:
          if (check_all_inverters_system_ready()) {
            set_all_inverters_dc_on(true);
            inverter_startup_state = INVERTER_STARTUP_STATE::WAIT_QUIT_DC_ON;
          }
          break;

        case INVERTER_STARTUP_STATE::WAIT_QUIT_DC_ON:
          if (check_all_inverters_quit_dc_on()) {
            set_all_inverters_no_torque();
            set_all_inverters_driver_enable(true);
            inverter_startup_state = INVERTER_STARTUP_STATE::WAIT_QUIT_INVERTER_ON;
          }
          break;

        case INVERTER_STARTUP_STATE::WAIT_QUIT_INVERTER_ON:
          if (check_all_inverters_quit_inverter_on()) {
#if DEBUG
            Serial.println("Setting state to Waiting Ready to Drive Sound");
#endif
            set_state(MCU_STATE::WAITING_READY_TO_DRIVE_SOUND);
          }
          break;

      }
      break;


    case MCU_STATE::WAITING_READY_TO_DRIVE_SOUND:
      check_TS_active();

      // if the ready to drive sound has been playing for long enough, move to ready to drive mode
      if (timer_ready_sound.check()) {
#if DEBUG
        Serial.println("Setting state to Ready to Drive");
#endif
        set_state(MCU_STATE::READY_TO_DRIVE);
      }
      break;

    case MCU_STATE::READY_TO_DRIVE:
      check_TS_active();

      if (check_all_inverters_error()) {
        set_all_inverters_disabled();
        set_state(MCU_STATE::TRACTIVE_SYSTEM_ACTIVE);
      }
      set_all_inverters_speed_setpoint(60);
      set_all_inverters_torque_limit(10);


      break;
  }
}

/* Shared state functinality */


bool check_TS_over_HV_threshold() {
  for (uint8_t inv = 0; inv < 4; inv++) {
    if (mc_energy[inv].get_dc_bus_voltage() < MIN_HV_VOLTAGE) {
      return false;
    }
  }
  return true;
}

// if TS is below HV threshold, return to Tractive System Not Active
inline void check_TS_active() {
  if (!check_TS_over_HV_threshold()) {
#if DEBUG
    Serial.println("Setting state to TS Not Active, because TS is below HV threshold");
#endif
    set_state(MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE);
  }
}



void parse_front_inv_can_message(const CAN_message_t &RX_msg) {
  static CAN_message_t rx_msg = RX_msg;
  while (FRONT_INV_CAN.read(rx_msg)) {
    switch (rx_msg.id) {
      case ID_MC1_STATUS:       mc_status[0].load(rx_msg.buf);    break;
      case ID_MC2_STATUS:       mc_status[1].load(rx_msg.buf);    break;
      case ID_MC1_TEMPS:        mc_temps[0].load(rx_msg.buf);    break;
      case ID_MC2_TEMPS:        mc_temps[1].load(rx_msg.buf);    break;
      case ID_MC1_ENERGY:       mc_energy[0].load(rx_msg.buf);    break;
      case ID_MC2_ENERGY:       mc_energy[1].load(rx_msg.buf);    break;
    }
  }
}

void parse_rear_inv_can_message(const CAN_message_t &RX_msg) {
  static CAN_message_t rx_msg = RX_msg;
  while (REAR_INV_CAN.read(rx_msg)) {
    switch (rx_msg.id) {
      case ID_MC3_STATUS:       mc_status[2].load(rx_msg.buf);    break;
      case ID_MC4_STATUS:       mc_status[3].load(rx_msg.buf);    break;
      case ID_MC3_TEMPS:        mc_temps[2].load(rx_msg.buf);    break;
      case ID_MC4_TEMPS:        mc_temps[3].load(rx_msg.buf);    break;
      case ID_MC3_ENERGY:       mc_energy[2].load(rx_msg.buf);    break;
      case ID_MC4_ENERGY:       mc_energy[3].load(rx_msg.buf);    break;
    }
  }
}
inline void power_off_inverter() {
  inverter_restart = true;
  digitalWrite(INVERTER_24V_EN, LOW);
  timer_restart_inverter.reset();
  mcu_status.set_inverter_powered(false);

#if DEBUG
  Serial.println("INVERTER RESET");
#endif
}


/* Handle changes in state */
void set_state(MCU_STATE new_state) {
  if (mcu_status.get_state() == new_state) {
    return;
  }

  // exit logic
  switch (mcu_status.get_state()) {
    case MCU_STATE::STARTUP: break;
    case MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE: break;
    case MCU_STATE::TRACTIVE_SYSTEM_ACTIVE: break;
    case MCU_STATE::ENABLING_INVERTER:
      timer_inverter_enable.reset();
      break;
    case MCU_STATE::WAITING_READY_TO_DRIVE_SOUND:
      // make dashboard stop buzzer
      mcu_status.set_activate_buzzer(false);
      mcu_status.write(msg.buf);
      msg.id = ID_MCU_STATUS;
      msg.len = sizeof(mcu_status);
      TELEM_CAN.write(msg);
      break;
    case MCU_STATE::READY_TO_DRIVE: break;
  }

  mcu_status.set_state(new_state);

  // entry logic
  switch (new_state) {
    case MCU_STATE::STARTUP: break;
    case MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE: break;
    case MCU_STATE::TRACTIVE_SYSTEM_ACTIVE: break;
    case MCU_STATE::ENABLING_INVERTER: {
        inverter_startup_state = INVERTER_STARTUP_STATE::WAIT_SYSTEM_READY;
        Serial.println("MCU Sent enable command");
        timer_inverter_enable.reset();
        break;
      }
    case MCU_STATE::WAITING_READY_TO_DRIVE_SOUND:
      // make dashboard sound buzzer
      mcu_status.set_activate_buzzer(true);
      mcu_status.write(msg.buf);
      msg.id = ID_MCU_STATUS;
      msg.len = sizeof(mcu_status);
      TELEM_CAN.write(msg);

      timer_ready_sound.reset();
      Serial.println("RTDS enabled");
      break;
    case MCU_STATE::READY_TO_DRIVE:
      Serial.println("Ready to drive");
      break;
  }
}




inline void clear_all_inverters_error() {

}

bool check_all_inverters_system_ready() {
  for (uint8_t inv = 0; inv < 4; inv++) {
    if (! mc_status[inv].get_system_ready()) {
      return false;
    }
  }
  return true;
}

bool check_all_inverters_quit_dc_on() {
  for (uint8_t inv = 0; inv < 4; inv++) {
    if (! mc_status[inv].get_quit_dc_on()) {
      return false;
    }
  }
  return true;
}

bool check_all_inverters_quit_inverter_on() {
  for (uint8_t inv = 0; inv < 4; inv++) {
    if (! mc_status[inv].get_quit_inverter_on()) {
      return false;
    }
  }
  return true;
}

uint8_t check_all_inverters_error() {
  uint8_t error_list = 0; //last 4 bits correspond to error bit in status word of each inverter, inverter 1 is rightmost bit;
  for (uint8_t inv = 0; inv < 4; inv++) {
    if (mc_status[inv].get_error()) {
      error_list = error_list | (0x01 << inv);
    }
  }
  return error_list;
}

inline void set_all_inverters_no_torque() {
  for (uint8_t inv = 0; inv < 4; inv++) {
    mc_setpoints_command[inv].set_speed_setpoint(0);
    mc_setpoints_command[inv].set_pos_torque_limit(0);
    mc_setpoints_command[inv].set_neg_torque_limit(0);
  }
}

inline void set_all_inverters_torque_limit(int limit) {

  //const float max_torque = max_torque_Nm / 0.0098; // max possible value for torque multiplier, unit in 0.1% nominal torque
  //inverter torque unit is in 0.1% of nominal torque (9.8Nm), max rated torque is 21Nm, so max possible output is 2142
  
  if (limit >= 0) {
    for (uint8_t inv = 0; inv < 4; inv++) {
      mc_setpoints_command[inv].set_pos_torque_limit(limit);
      mc_setpoints_command[inv].set_neg_torque_limit(0);
    }
  }
  else {
    for (uint8_t inv = 0; inv < 4; inv++) {
      mc_setpoints_command[inv].set_pos_torque_limit(0);
      mc_setpoints_command[inv].set_neg_torque_limit(abs(limit));
    }
  }

}

inline void set_all_inverters_speed_setpoint(int setpoint) {
  for (uint8_t inv = 0; inv < 4; inv++) {
    mc_setpoints_command[inv].set_speed_setpoint(setpoint);
  }
}
inline void set_all_inverters_disabled() {
  for (uint8_t inv = 0; inv < 4; inv++) {
    mc_setpoints_command[inv].set_inverter_enable(false);
    mc_setpoints_command[inv].set_hv_enable(false);
    mc_setpoints_command[inv].set_driver_enable(false);
    mc_setpoints_command[inv].set_remove_error(false);
    mc_setpoints_command[inv].set_speed_setpoint(0);
    mc_setpoints_command[inv].set_pos_torque_limit(0);
    mc_setpoints_command[inv].set_neg_torque_limit(0);
  }
}

inline void set_all_inverters_dc_on(bool input) {
  for (uint8_t inv = 0; inv < 4; inv++) {
    mc_setpoints_command[inv].set_hv_enable(input);
  }
}

inline void set_all_inverters_driver_enable(bool input) {
  for (uint8_t inv = 0; inv < 4; inv++) {
    mc_setpoints_command[inv].set_driver_enable(input);
  }
}

inline void reset_inverters() {

}
