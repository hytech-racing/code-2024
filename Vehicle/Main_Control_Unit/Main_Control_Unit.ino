/*
   Teensy 4.1 Main Control Unit code
   Written by Liwei Sun which is why the code is so bad

   Rev 11
*/

#include <stdint.h>
#include <FlexCAN_T4.h>
#define _USE_MATH_DEFINES
#include <cmath>

#include "ADC_SPI.h"
#include "HyTech_CAN.h"
#include "kinetis_flexcan.h"
#include "Metro.h"

#include "drivers.h"

// constants to define for different operation

#define DRIVER DAVID
#define TORQUE_1 100
#define TORQUE_2 130
#define TORQUE_3 160


// set to true or false for debugging
#define DEBUG true
#define BMS_DEBUG_ENABLE false

#include "MCU_rev11_dfs.h"

#include "driver_constants.h"

// Outbound CAN messages
MCU_pedal_readings mcu_pedal_readings{};
MCU_status mcu_status{};
MCU_wheel_speed mcu_wheel_speed{};

MC_status mc_status[4];
MC_temps mc_temps[4];
MC_energy mc_energy[4];
MC_setpoints_command mc_setpoints_command[4];
MC_torque_command mc_torque_command[4];

// Inbound CAN messages
BMS_coulomb_counts bms_coulomb_counts{};
BMS_status bms_status{};
BMS_temperatures bms_temperatures{};
BMS_voltages bms_voltages{};
Dashboard_status dashboard_status{};

//Timers
Metro timer_CAN_inverter_status_read = Metro(50); 
Metro timer_CAN_inverter_temps_read = Metro(400); 
Metro timer_CAN_inverter_energy_read = Metro(500); 
Metro timer_CAN_inverter_setpoints_send = Metro(200);
Metro timer_CAN_inverter_torque_send = Metro(50); 
Metro timer_CAN_coloumb_count_send = Metro(1000);
Metro timer_ready_sound = Metro(2000); // Time to play RTD sound
Metro timer_CAN_mcu_status_send = Metro(100);
Metro timer_CAN_mcu_pedal_readings_send = Metro(5);
Metro timer_restart_inverter = Metro(500, 1); // Allow the MCU to restart the inverter
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
float filtered_accel1_reading{};
float filtered_accel2_reading{};
float filtered_brake1_reading{};
float filtered_brake2_reading{};

bool imd_faulting = false;
bool inverter_restart = false; // True when restarting the inverter

uint32_t total_charge_amount = 0;
uint32_t total_discharge_amount = 0;

ADC_SPI ADC(ADC_CS, ADC_SPI_SPEED);
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> FRONT_INV_CAN;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> REAR_INV_CAN;
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> TELEM_CAN;
CAN_message_t msg;

// coloumb counts
uint32_t total_discharge;
unsigned long previous_data_time;

void setup() {
  // no torque can be provided on startup
  mcu_status.set_max_torque(0);
  mcu_status.set_torque_mode(0);
  mcu_status.set_software_is_ok(true);

  pinMode(BRAKE_LIGHT_CTRL, OUTPUT);

  // change to input if comparator is PUSH PULL
  pinMode(INVERTER_CTRL, OUTPUT);

  pinMode(WATCHDOG_INPUT, OUTPUT);
  // the initial state of the watchdog is high
  // this is reflected in the static watchdog_state
  // starting high
  digitalWrite(WATCHDOG_INPUT, HIGH);
  pinMode(TEENSY_OK, OUTPUT);
  digitalWrite(TEENSY_OK, HIGH);

#if DEBUG
  Serial.begin(115200);
#endif
  FRONT_INV_CAN.begin();
  FRONT_INV_CAN.setBaudRate(1000000);
  REAR_INV_CAN.begin();
  REAR_INV_CAN.setBaudRate(1000000);
  TELEM_CAN.begin();
  TELEM_CAN.setBaudRate(1000000);
  delay(500);

#if DEBUG
  Serial.println("CAN system and serial communication initialized");
#endif


  // these are false by default
  mcu_status.set_bms_ok_high(false);
  mcu_status.set_imd_ok_high(false);

  digitalWrite(INVERTER_CTRL, HIGH);
  mcu_status.set_inverter_powered(true);
  send_CAN_disable_all_inverters();


  // present action for 5s
  delay(5000);

  set_state(MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE);
  mcu_status.set_max_torque(TORQUE_1);
  mcu_status.set_torque_mode(1);

  /* Set up total discharge readings */
  //setup_total_discharge();

}

void loop() {
  read_pedal_values();
  read_status_values();

  send_CAN_mcu_status();
  send_CAN_mcu_pedal_readings();
  //send_CAN_bms_coulomb_counts();


  /* Finish restarting the inverter when timer expires */
  if (timer_restart_inverter.check() && inverter_restart) {
    inverter_restart = false;
    digitalWrite(INVERTER_CTRL, HIGH);
    mcu_status.set_inverter_powered(true);
  }

  /* handle state functionality */
  state_machine();
  software_shutdown();
}

inline void send_CAN_mcu_status() {
  if (timer_CAN_mcu_status_send.check()) {
    // Send Main Control Unit status message
    mcu_status.write(msg.buf);
    msg.id = ID_MCU_STATUS;
    msg.len = sizeof(mcu_status);
    TELEM_CAN.write(msg);
  }
}

inline void send_CAN_mcu_pedal_readings() {
  if (timer_CAN_mcu_pedal_readings_send.check()) {
    // Update the pedal readings to send over CAN
    mcu_pedal_readings.set_accelerator_pedal_1(filtered_accel1_reading);
    mcu_pedal_readings.set_accelerator_pedal_2(filtered_accel2_reading);
    mcu_pedal_readings.set_brake_transducer_1(filtered_brake1_reading);
    mcu_pedal_readings.set_brake_transducer_2(filtered_brake2_reading);

    // Send Main Control Unit pedal reading message
    mcu_pedal_readings.write(msg.buf);
    msg.id = ID_MCU_PEDAL_READINGS;
    msg.len = sizeof(mcu_pedal_readings);
    TELEM_CAN.write(msg);
  }
}

inline void send_CAN_bms_coulomb_counts() {
  if (timer_CAN_bms_coloumb_count_send.check()) {
    bms_coulomb_counts.write(msg.buf);
    msg.id = ID_BMS_COULOMB_COUNTS;
    msg.len = sizeof(bms_coulomb_counts);
    TELEM_CAN.write(msg);
  }
}
//inline void setup_total_discharge() {
//  total_discharge = 0;
//  previous_data_time = millis();
//  bms_coulomb_counts.set_total_discharge(total_discharge);
//}
//
//inline void process_total_discharge() {
//  unsigned long current_time = millis();
//  double new_current = mc_current_information.get_dc_bus_current() / 10;
//  uint32_t added_Ah = new_current * ((current_time - previous_data_time) * 10000 / (1000 * 60 * 60 )); //scaled by 10000 for telemetry parsing
//  previous_data_time = current_time;
//  total_discharge += added_Ah;
//  bms_coulomb_counts.set_total_discharge(total_discharge);
//}

inline void state_machine() {
  switch (mcu_status.get_state()) {
    case MCU_STATE::STARTUP: break;
    
    case MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE:
      inverter_heartbeat(0);
      #if DEBUG
        Serial.println("TS NOT ACTIVE");  
      #endif
      // if TS is above HV threshold, move to Tractive System Active
      if (mc_voltage_information.get_dc_bus_voltage() >= MIN_HV_VOLTAGE) {
        #if DEBUG
          Serial.println("Setting state to TS Active from TS Not Active");
        #endif
        set_state(MCU_STATE::TRACTIVE_SYSTEM_ACTIVE);
      }
      break;
  
    case MCU_STATE::TRACTIVE_SYSTEM_ACTIVE:
      check_TS_active();
      send_CAN_disable_all_inverters();
      if (dashboard_status.get_start_btn() && mcu_status.get_brake_pedal_active()) {
        #if DEBUG
          Serial.println("Setting state to Enabling Inverter");
        #endif
        set_state(MCU_STATE::ENABLING_INVERTER);
      }
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
      // motor controller indicates that inverter has enabled within timeout period
      if () {
        #if DEBUG
          Serial.println("Setting state to Waiting Ready to Drive Sound");
        #endif
        set_state(MCU_STATE::WAITING_READY_TO_DRIVE_SOUND);
      }
      break;

    case MCU_STATE::WAITING_READY_TO_DRIVE_SOUND:
      check_TS_active();
      check_inverter_disabled();
      inverter_heartbeat(1);

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
      check_inverter_disabled();

      if (timer_motor_controller_send.check()) {
        MC_command_message mc_command_message(0, 0, 0, 1, 0, 0);

        // FSAE EV.5.5
        // FSAE T.4.2.10
        if (filtered_accel1_reading < MIN_ACCELERATOR_PEDAL_1 || filtered_accel1_reading > MAX_ACCELERATOR_PEDAL_1) {
          mcu_status.set_no_accel_implausability(false);
          #if DEBUG
            Serial.println("T.4.2.10 1");
          #endif
        }
        else if (filtered_accel2_reading > MAX_ACCELERATOR_PEDAL_2 || filtered_accel2_reading < MIN_ACCELERATOR_PEDAL_2) {
          mcu_status.set_no_accel_implausability(false);
#if DEBUG
          Serial.println("T.4.2.10 2");
#endif
        }
        // check that the pedals are reading within 10% of each other
        // T.4.2.4
        else if (fabs((filtered_accel1_reading - START_ACCELERATOR_PEDAL_1) / (END_ACCELERATOR_PEDAL_1 - START_ACCELERATOR_PEDAL_1) -
                      (filtered_accel2_reading - START_ACCELERATOR_PEDAL_2) / (END_ACCELERATOR_PEDAL_2 - START_ACCELERATOR_PEDAL_2)) > 0.1) {
#if DEBUG
          Serial.println("T.4.2.4");
          Serial.printf("pedal 1 - %f\n", (filtered_accel1_reading - START_ACCELERATOR_PEDAL_1) / (END_ACCELERATOR_PEDAL_1 - START_ACCELERATOR_PEDAL_1));
          Serial.printf("pedal 2 - %f\n", (filtered_accel2_reading - START_ACCELERATOR_PEDAL_2) / (END_ACCELERATOR_PEDAL_2 - START_ACCELERATOR_PEDAL_2));
#endif
          mcu_status.set_no_accel_implausability(false);
        }
        else {
          mcu_status.set_no_accel_implausability(true);
        }

        // BSE check
        // EV.5.6
        // FSAE T.4.3.4
        if (filtered_brake1_reading < 409 || filtered_brake1_reading > 3687) {
          mcu_status.set_no_brake_implausability(false);
        }
        else {
          mcu_status.set_no_brake_implausability(true);
        }

        // FSAE EV.5.7
        // APPS/Brake Pedal Plausability Check
        if  (
          (
            (filtered_accel1_reading > ((END_ACCELERATOR_PEDAL_1 - START_ACCELERATOR_PEDAL_1) / 4 + START_ACCELERATOR_PEDAL_1))
            ||
            (filtered_accel2_reading > ((END_ACCELERATOR_PEDAL_2 - START_ACCELERATOR_PEDAL_2) / 4 + START_ACCELERATOR_PEDAL_2))
          )
          && mcu_status.get_brake_pedal_active()
        )
        {
          mcu_status.set_no_accel_brake_implausability(false);
        }
        else if
        (
          (filtered_accel1_reading < ((END_ACCELERATOR_PEDAL_1 - START_ACCELERATOR_PEDAL_1) / 20 + START_ACCELERATOR_PEDAL_1))
          &&
          (filtered_accel2_reading < ((END_ACCELERATOR_PEDAL_2 - START_ACCELERATOR_PEDAL_2) / 20 + START_ACCELERATOR_PEDAL_2))
        )
        {
          mcu_status.set_no_accel_brake_implausability(true);
        }

        int calculated_torque = 0;

        if (
          mcu_status.get_no_brake_implausability() &&
          mcu_status.get_no_accel_implausability() &&
          mcu_status.get_no_accel_brake_implausability() &&
          mcu_status.get_bms_ok_high() &&
          mcu_status.get_imd_ok_high()
        ) {
          calculated_torque = calculate_torque();
        } else {
          Serial.println("not calculating torque");
          Serial.printf("no brake implausibility: %d\n", mcu_status.get_no_brake_implausability());
          Serial.printf("no accel implausibility: %d\n", mcu_status.get_no_accel_implausability());
          Serial.printf("no accel brake implausibility: %d\n", mcu_status.get_no_accel_brake_implausability());
          Serial.printf("software is ok: %d\n", mcu_status.get_software_is_ok());
          Serial.printf("get bms ok high: %d\n", mcu_status.get_bms_ok_high());
          Serial.printf("get imd ok high: %d\n", mcu_status.get_imd_ok_high());

        }
        // Implausibility exists, command 0 torque

        mc_command_message.set_torque_command(calculated_torque);

        mc_command_message.write(tx_msg.buf);
        tx_msg.id = ID_MC_COMMAND_MESSAGE;
        tx_msg.len = 8;
        CAN.write(tx_msg);
      }
      break;
  }
}

/* Shared state functinality */

// if TS is below HV threshold, return to Tractive System Not Active
inline void check_TS_active() {
  if (mc_voltage_information.get_dc_bus_voltage() < MIN_HV_VOLTAGE) {
    #if DEBUG
      Serial.println("Setting state to TS Not Active, because TS is below HV threshold");
    #endif
    set_state(MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE);
  }
}


// Send a message to the Motor Controller over CAN when vehicle is not ready to drive
inline void send_CAN_disable_all_inverters(){
  if (timer_motor_controller_send.check()) {
  }
}

/* Implementation of software shutdown */
inline void software_shutdown() {

  mcu_status.set_software_is_ok(true);

  // check inputs
  // BMS heartbeat has not arrived within time interval
  if (timer_bms_heartbeat.check()) {
    timer_bms_heartbeat.interval(0);
#if DEBUG
    Serial.println("no bms heartbeat");
#endif
    mcu_status.set_software_is_ok(false);
  }

  // add BMS software checks
  // software ok/not ok action
  if (mcu_status.get_software_is_ok()) {
    digitalWrite(TEENSY_OK, HIGH); //eventually make this HIGH only if software is ok
  } else {
    digitalWrite(TEENSY_OK, LOW);
  }
  /* Watchdog timer */
  if (timer_watchdog_timer.check()) {
    static bool watchdog_state = HIGH;
    watchdog_state = !watchdog_state;
    digitalWrite(WATCHDOG_INPUT, watchdog_state);
  }

}

/* Parse incoming CAN messages */
void parse_can_message() {
  static CAN_message_t rx_msg;
  while (CAN.read(rx_msg)) {
    switch (rx_msg.id) {
      case ID_MC_VOLTAGE_INFORMATION:        mc_voltage_information.load(rx_msg.buf);        break;
      case ID_MC_INTERNAL_STATES:            mc_internal_states.load(rx_msg.buf);            break;
      case ID_MC_CURRENT_INFORMATION:
        mc_current_information.load(rx_msg.buf);
        process_total_discharge();
        break;
      case ID_MC_MOTOR_POSITION_INFORMATION: mc_motor_position_information.load(rx_msg.buf); break;
      case ID_BMS_TEMPERATURES:              bms_temperatures.load(rx_msg.buf);              break;
      case ID_BMS_VOLTAGES:                  bms_voltages.load(rx_msg.buf);                  break;
      case ID_BMS_COULOMB_COUNTS:            bms_coulomb_counts.load(rx_msg.buf);            break;
      case ID_BMS_STATUS:
        bms_status.load(rx_msg.buf);
        // BMS heartbeat timer
        timer_bms_heartbeat.reset();
        timer_bms_heartbeat.interval(BMS_HEARTBEAT_TIMEOUT);
        break;
      case ID_DASHBOARD_STATUS:
        dashboard_status.load(rx_msg.buf);
        /* process dashboard buttons */
        if (dashboard_status.get_mode_btn()) {
          switch (mcu_status.get_torque_mode()) {
            case 1:
              mcu_status.set_max_torque(TORQUE_2);
              mcu_status.set_torque_mode(2); break;
            case 2:
              mcu_status.set_max_torque(TORQUE_3);
              mcu_status.set_torque_mode(3); break;
            case 3:
              mcu_status.set_max_torque(TORQUE_1);
              mcu_status.set_torque_mode(1); break;
          }
        }
        if (dashboard_status.get_launch_ctrl_btn()) {
          mcu_status.toggle_launch_ctrl_active();
        }
        if (dashboard_status.get_mc_cycle_btn()) {
          reset_inverter();
        }
        // eliminate all action buttons to not process twice
        dashboard_status.set_button_flags(0);
        break;
    }
  }
}

inline void reset_inverter() {
  inverter_restart = true;
  digitalWrite(INVERTER_CTRL, LOW);
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
    case MCU_STATE::ENABLING_INVERTER: break;
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


int calculate_torque() {
  int calculated_torque = 0;
  int16_t mc_rpm = abs(mc_motor_position_information.get_motor_speed());
  if (mcu_status.get_launch_ctrl_active()) {

  } else {
    const int max_torque = mcu_status.get_max_torque() * 10;
    int torque1 = map(round(filtered_accel1_reading), START_ACCELERATOR_PEDAL_1, END_ACCELERATOR_PEDAL_1, 0, max_torque);
    int torque2 = map(round(filtered_accel2_reading), START_ACCELERATOR_PEDAL_2, END_ACCELERATOR_PEDAL_2, 0, max_torque);

    // torque values are greater than the max possible value, set them to max
    if (torque1 > max_torque) {
      torque1 = max_torque;
    }
    if (torque2 > max_torque) {
      torque2 = max_torque;
    }
    // compare torques to check for accelerator implausibility
    calculated_torque = (torque1 + torque2) / 2;

    if (calculated_torque > max_torque) {
      calculated_torque = max_torque;
    }
  }

  if (calculated_torque < 0) {
    calculated_torque = 0;
  }


  //power limit to 80kW
  //add this plz

  return calculated_torque;
}

/* Read pedal sensor values */
inline void read_pedal_values() {
  /* Filter ADC readings */
  filtered_accel1_reading = ALPHA * filtered_accel1_reading + (1 - ALPHA) * ADC.read_adc(ADC_ACCEL_1_CHANNEL);
  filtered_accel2_reading = ALPHA * filtered_accel2_reading + (1 - ALPHA) * ADC.read_adc(ADC_ACCEL_2_CHANNEL);
  filtered_brake1_reading = ALPHA * filtered_brake1_reading + (1 - ALPHA) * ADC.read_adc(ADC_BRAKE_1_CHANNEL);
  filtered_brake2_reading = ALPHA * filtered_brake2_reading + (1 - ALPHA) * ADC.read_adc(ADC_BRAKE_2_CHANNEL);

#if DEBUG
  // Serial.print("ACCEL 1: "); Serial.println(filtered_accel1_reading);
  // Serial.print("ACCEL 2: "); Serial.println(filtered_accel2_reading);
  //  Serial.print("BRAKE 1: "); Serial.println(filtered_brake1_reading);
  //  Serial.print("BRAKE 2: "); Serial.println(filtered_brake2_reading);
#endif

  // only uses front brake pedal
  mcu_status.set_brake_pedal_active(filtered_brake1_reading >= BRAKE_ACTIVE);
  digitalWrite(BRAKE_LIGHT_CTRL, mcu_status.get_brake_pedal_active());

}

/* Read shutdown system values */
inline void read_status_values() {
  /* Measure shutdown circuits' input */
  mcu_status.set_bms_ok_high(analogRead(BMS_OK_READ) > SHUTDOWN_HIGH);
  mcu_status.set_imd_ok_high(analogRead(IMD_OK_READ) > SHUTDOWN_HIGH);
  mcu_status.set_bspd_ok_high(analogRead(BSPD_OK_READ) > SHUTDOWN_HIGH);
  mcu_status.set_software_ok_high(analogRead(SOFTWARE_OK_READ) > SHUTDOWN_HIGH);

  /* Measure shutdown circuits' voltages */
  mcu_status.set_shutdown_b_above_threshold(analogRead(SHUTDOWN_B_READ) > SHUTDOWN_HIGH);
  mcu_status.set_shutdown_c_above_threshold(analogRead(SHUTDOWN_C_READ) > SHUTDOWN_HIGH);
  mcu_status.set_shutdown_d_above_threshold(analogRead(SHUTDOWN_D_READ) > SHUTDOWN_HIGH);
  mcu_status.set_shutdown_e_above_threshold(analogRead(SHUTDOWN_E_READ) > SHUTDOWN_HIGH);
}

/* Read wheel speed values */
// may need to move to interrupt based approach
inline void read_wheel_speed() {

}

/* Track how far the car has driven */
inline void update_distance_traveled() {

}
