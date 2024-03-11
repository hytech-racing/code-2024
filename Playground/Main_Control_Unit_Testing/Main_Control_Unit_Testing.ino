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
#include "STEERING_SPI.h"
#include "kinetis_flexcan.h"
#include "Metro.h"

// IMU
#include <ADIS16460.h>

#include "drivers.h"

// constants to define for different operation

#define DRIVER DEFAULT_DRIVER
#define TORQUE_1 70
#define TORQUE_2 85
#define TORQUE_3 100


// set to true or false for debugging
#define DEBUG true
#define BMS_DEBUG_ENABLE false

#include "MCU_rev12_dfs.h"

#include "driver_constants.h"

// Call the ADIS16460 IMU class
ADIS16460 IMU(IMU_CS, IMU_DATAREADY, IMU_RESET); // Chip Select, Data Ready, Reset Pin Assignments

// Outbound CAN messages
MCU_pedal_readings mcu_pedal_readings;
MCU_status mcu_status{};
MCU_load_cells mcu_load_cells{};
MCU_analog_readings mcu_analog_readings{};

// IMU
IMU_accelerometer imu_accelerometer;
IMU_gyroscope imu_gyroscope;

MC_status mc_status[4];
MC_temps mc_temps[4];
MC_energy mc_energy[4];
MC_setpoints_command mc_setpoints_command[4];

// Inbound CAN messages
BMS_coulomb_counts bms_coulomb_counts{};
BMS_status bms_status{};
BMS_temperatures bms_temperatures{};
BMS_voltages bms_voltages{};
Dashboard_status dashboard_status{};

//Timers
Metro timer_CAN_imu_accelerometer_send = Metro(50);
Metro timer_CAN_imu_gyroscope_send = Metro(50);
Metro timer_CAN_inverter_status_read = Metro(20);
Metro timer_CAN_inverter_temps_read = Metro(200);
Metro timer_CAN_inverter_energy_read = Metro(200);
Metro timer_CAN_inverter_setpoints_send = Metro(20);
Metro timer_CAN_inverter_torque_send = Metro(20);
Metro timer_CAN_coloumb_count_send = Metro(1000);
Metro timer_CAN_mcu_status_send = Metro(100);
Metro timer_CAN_mcu_pedal_readings_send = Metro(5);
Metro timer_CAN_mcu_load_cells_send = Metro(10);

Metro timer_CAN_mc_status_forward = Metro(100);
Metro timer_CAN_mc_temps_forward = Metro(250);
Metro timer_CAN_mc_energy_forward = Metro(250);
Metro timer_CAN_mc_setpoints_command_forward = Metro(300);
Metro timer_CAN_mc_torque_command_forward = Metro(100);

Metro timer_ready_sound = Metro(2000); // Time to play RTD sound

Metro timer_load_cells_read = Metro(100);
Metro timer_pedals_read = Metro(100);
Metro timer_steering_read = Metro(10);
Metro timer_glv_read = Metro(10);

Metro timer_inverter_enable = Metro(5000);
Metro timer_reset_inverter = Metro(200);
Metro timer_watchdog_timer = Metro(10);

// this abuses Metro timer functionality to stay faulting once a fault has occurred
// autoreset makes the timer update to the current time and not by the interval
// this allows me to set the interval as 0 once a fault has occurred, leading to continuous faulting
// until a CAN message comes in which resets the timer and the interval
Metro timer_bms_heartbeat = Metro(0, 1);

Metro timer_print = Metro(100);


bool imd_faulting = false;
bool inverter_restart = false; // True when restarting the inverter
INVERTER_STARTUP_STATE inverter_startup_state = INVERTER_STARTUP_STATE::WAIT_SYSTEM_READY;

ADC_SPI ADC1(ADC1_CS, ADC_SPI_SPEED);
ADC_SPI ADC2(ADC2_CS, ADC_SPI_SPEED);
ADC_SPI ADC3(ADC3_CS, ADC_SPI_SPEED);

STEERING_SPI STEERING(STEERING_CS, STEERING_SPI_SPEED);

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> FRONT_INV_CAN;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> REAR_INV_CAN;
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> TELEM_CAN;
CAN_message_t msg;

// coloumb counts
uint32_t total_discharge;
unsigned long previous_data_time;

void setup() {
  // no torque can be provided on startup
//  mcu_status.set_max_torque(0);
//  mcu_status.set_torque_mode(0);
//  mcu_status.set_software_is_ok(true);
//
//  set_all_inverters_disabled();
//
//  // IMU set up
//  IMU.regWrite(MSC_CTRL, 0xC1);  // Enable Data Ready, set polarity
//  IMU.regWrite(FLTR_CTRL, 0x500); // Set digital filter
//  IMU.regWrite(DEC_RATE, 0), // Disable decimation
//
//               pinMode(BRAKE_LIGHT_CTRL, OUTPUT);
//
//  // change to input if comparator is PUSH PULL
////  pinMode(INVERTER_EN, OUTPUT);
////  pinMode(INVERTER_24V_EN, OUTPUT);
////
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
//  FRONT_INV_CAN.begin();
//  FRONT_INV_CAN.setBaudRate(1000000);
//  REAR_INV_CAN.begin();
//  REAR_INV_CAN.setBaudRate(1000000);
  TELEM_CAN.begin();
  TELEM_CAN.setBaudRate(1000000);
//  FRONT_INV_CAN.enableMBInterrupts();
//  REAR_INV_CAN.enableMBInterrupts();
    TELEM_CAN.enableMBInterrupts();
//  FRONT_INV_CAN.onReceive(parse_front_inv_can_message);
//  REAR_INV_CAN.onReceive(parse_rear_inv_can_message);
  TELEM_CAN.onReceive(parse_telem_can_message);
  delay(500);

#if DEBUG
  Serial.println("CAN system anwd serial communication initialized");
#endif


  // these are false by default
//  mcu_status.set_bms_ok_high(false);
//  mcu_status.set_imd_ok_high(false);
//
//  digitalWrite(INVERTER_24V_EN, HIGH);
//  mcu_status.set_inverter_powered(true);
//


  // present action for 5s
  delay(5000);

//  set_state(MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE);
//  mcu_status.set_max_torque(TORQUE_1);
//  mcu_status.set_torque_mode(1);

  /* Set up total discharge readings */
  //setup_total_discharge();

}
void canSniff(const CAN_message_t &msg) {
  Serial.print("MB "); Serial.print(msg.mb);
  Serial.print("  OVERRUN: "); Serial.print(msg.flags.overrun);
  Serial.print("  LEN: "); Serial.print(msg.len);
  Serial.print(" EXT: "); Serial.print(msg.flags.extended);
  Serial.print(" TS: "); Serial.print(msg.timestamp);
  Serial.print(" ID: "); Serial.print(msg.id, HEX);
  Serial.print(" Buffer: ");
  for ( uint8_t i = 0; i < msg.len; i++ ) {
    Serial.print(msg.buf[i], HEX); Serial.print(" ");
  } Serial.println();
}
void loop() {
//  FRONT_INV_CAN.events();
//  REAR_INV_CAN.events();
  TELEM_CAN.events();
 
//  read_pedal_values();
//  read_load_cell_values()
calibrate_load_cell_values();
//  read_steering_values();
//  read_status_values();
//
//  send_CAN_mcu_status();
//  send_CAN_mcu_pedal_readings();
//  //send_CAN_bms_coulomb_counts();
//
//  send_CAN_inverter_setpoints();
//
//  forward_CAN_mc_status();
//  forward_CAN_mc_temps();
//  forward_CAN_mc_energy();
//  forward_CAN_mc_setpoints_command();
//  /* Finish restarting the inverter when timer expires */
//  reset_inverters();
//  
//  /* handle state functionality */
//  state_machine();
    software_shutdown();
//    Serial.println("ok");
//    read_dashboard();
//    write_dashboard();

}

inline void read_dashboard() {
//  if(timer_print.check()) {
//     Serial.println(dashboard_status.get_dial_state());
//  }
 
 
}
inline void write_dashboard() {
  mcu_status.set_bms_ok_high(1);
  mcu_status.set_imd_ok_high(1);
  mcu_status.set_state(MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE);
  mcu_status.set_pack_charge_critical(3);
  mcu_status.set_torque_mode(2);
  mcu_status.set_mech_brake_active(1);
  mcu_status.set_launch_ctrl_active(1);
    if (timer_CAN_mcu_status_send.check()) {
    // Send Main Control Unit status message
    mcu_status.write(msg.buf);
    msg.id = ID_MCU_STATUS;
    msg.len = sizeof(mcu_status);
    TELEM_CAN.write(msg);
  }
  
}

inline void forward_CAN_mc_status() {
  if (timer_CAN_mc_status_forward.check()) {
    mc_status[0].write(msg.buf);
    msg.id = ID_MC1_STATUS;
    msg.len = sizeof(mc_status[0]);
    TELEM_CAN.write(msg);

    mc_status[1].write(msg.buf);
    msg.id = ID_MC2_STATUS;
    msg.len = sizeof(mc_status[1]);
    TELEM_CAN.write(msg);

    mc_status[2].write(msg.buf);
    msg.id = ID_MC3_STATUS;
    msg.len = sizeof(mc_status[2]);
    TELEM_CAN.write(msg);

    mc_status[3].write(msg.buf);
    msg.id = ID_MC4_STATUS;
    msg.len = sizeof(mc_status[3]);
    TELEM_CAN.write(msg);
  }
}

inline void forward_CAN_mc_temps() {
  if (timer_CAN_mc_temps_forward.check()) {
    mc_temps[0].write(msg.buf);
    msg.id = ID_MC1_TEMPS;
    msg.len = sizeof(mc_temps[0]);
    TELEM_CAN.write(msg);

    mc_temps[1].write(msg.buf);
    msg.id = ID_MC2_TEMPS;
    msg.len = sizeof(mc_temps[1]);
    TELEM_CAN.write(msg);

    mc_temps[2].write(msg.buf);
    msg.id = ID_MC3_TEMPS;
    msg.len = sizeof(mc_temps[2]);
    TELEM_CAN.write(msg);

    mc_temps[3].write(msg.buf);
    msg.id = ID_MC4_TEMPS;
    msg.len = sizeof(mc_temps[3]);
    TELEM_CAN.write(msg);
  }
}

inline void forward_CAN_mc_energy() {
  if (timer_CAN_mc_energy_forward.check()) {
    mc_energy[0].write(msg.buf);
    msg.id = ID_MC1_ENERGY;
    msg.len = sizeof(mc_energy[0]);
    TELEM_CAN.write(msg);

    mc_energy[1].write(msg.buf);
    msg.id = ID_MC2_ENERGY;
    msg.len = sizeof(mc_energy[1]);
    TELEM_CAN.write(msg);

    mc_energy[2].write(msg.buf);
    msg.id = ID_MC3_ENERGY;
    msg.len = sizeof(mc_energy[2]);
    TELEM_CAN.write(msg);

    mc_energy[3].write(msg.buf);
    msg.id = ID_MC4_ENERGY;
    msg.len = sizeof(mc_energy[3]);
    TELEM_CAN.write(msg);
  }
}

inline void forward_CAN_mc_setpoints_command() {
  if (timer_CAN_mc_setpoints_command_forward.check()) {
    mc_setpoints_command[0].write(msg.buf);
    msg.id = ID_MC1_SETPOINTS_COMMAND;
    msg.len = sizeof(mc_setpoints_command[0]);
    TELEM_CAN.write(msg);

    mc_setpoints_command[1].write(msg.buf);
    msg.id = ID_MC2_SETPOINTS_COMMAND;
    msg.len = sizeof(mc_setpoints_command[1]);
    TELEM_CAN.write(msg);

    mc_setpoints_command[2].write(msg.buf);
    msg.id = ID_MC3_SETPOINTS_COMMAND;
    msg.len = sizeof(mc_setpoints_command[2]);
    TELEM_CAN.write(msg);

    mc_setpoints_command[3].write(msg.buf);
    msg.id = ID_MC4_SETPOINTS_COMMAND;
    msg.len = sizeof(mc_setpoints_command[3]);
    TELEM_CAN.write(msg);
  }
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
inline void send_CAN_mcu_status() {
  if (timer_CAN_mcu_status_send.check()) {
    // Send Main Control Unit status message
    mcu_status.write(msg.buf);
    msg.id = ID_MCU_STATUS;
    msg.len = sizeof(mcu_status);
    TELEM_CAN.write(msg);
    Serial.println("sent");
  }
}

inline void send_CAN_mcu_pedal_readings() {
  if (timer_CAN_mcu_pedal_readings_send.check()) {
    mcu_pedal_readings.write(msg.buf);
    msg.id = ID_MCU_PEDAL_READINGS;
    msg.len = sizeof(mcu_pedal_readings);
    TELEM_CAN.write(msg);
  }
}

inline void send_CAN_mcu_load_cells() {
  if (timer_CAN_mcu_load_cells_send.check()) {
    mcu_load_cells.write(msg.buf);
    msg.id = ID_MCU_LOAD_CELLS;
    msg.len = sizeof(mcu_load_cells);
    TELEM_CAN.write(msg);
  }
}

inline void send_CAN_bms_coulomb_counts() {
  if (timer_CAN_coloumb_count_send.check()) {
    bms_coulomb_counts.write(msg.buf);
    msg.id = ID_BMS_COULOMB_COUNTS;
    msg.len = sizeof(bms_coulomb_counts);
    TELEM_CAN.write(msg);
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

      // FSAE EV.5.5
      // FSAE T.4.2.10
      if (mcu_pedal_readings.get_accelerator_pedal_1() < MIN_ACCELERATOR_PEDAL_1 || mcu_pedal_readings.get_accelerator_pedal_1() > MAX_ACCELERATOR_PEDAL_1) {
        mcu_status.set_no_accel_implausability(false);
#if DEBUG
        Serial.println("T.4.2.10 1");
#endif
      }
      else if (mcu_pedal_readings.get_accelerator_pedal_2() > MAX_ACCELERATOR_PEDAL_2 || mcu_pedal_readings.get_accelerator_pedal_2() < MIN_ACCELERATOR_PEDAL_2) {
        mcu_status.set_no_accel_implausability(false);
#if DEBUG
        Serial.println("T.4.2.10 2");
#endif
      }
      // check that the pedals are reading within 10% of each other
      // T.4.2.4
      else if (fabs((mcu_pedal_readings.get_accelerator_pedal_1() - START_ACCELERATOR_PEDAL_1) / (END_ACCELERATOR_PEDAL_1 - START_ACCELERATOR_PEDAL_1) -
                    (mcu_pedal_readings.get_accelerator_pedal_2() - START_ACCELERATOR_PEDAL_2) / (END_ACCELERATOR_PEDAL_2 - START_ACCELERATOR_PEDAL_2)) > 0.1) {
#if DEBUG
        Serial.println("T.4.2.4");
        Serial.printf("pedal 1 - %f\n", (mcu_pedal_readings.get_accelerator_pedal_1() - START_ACCELERATOR_PEDAL_1) / (END_ACCELERATOR_PEDAL_1 - START_ACCELERATOR_PEDAL_1));
        Serial.printf("pedal 2 - %f\n", (mcu_pedal_readings.get_accelerator_pedal_2() - START_ACCELERATOR_PEDAL_2) / (END_ACCELERATOR_PEDAL_2 - START_ACCELERATOR_PEDAL_2));
#endif
        mcu_status.set_no_accel_implausability(false);
      }
      else {
        mcu_status.set_no_accel_implausability(true);
      }

      // BSE check
      // EV.5.6
      // FSAE T.4.3.4
      if (mcu_pedal_readings.get_brake_pedal_1() < MIN_BRAKE_PEDAL_1 || mcu_pedal_readings.get_brake_pedal_1() > MAX_BRAKE_PEDAL_1) {
        mcu_status.set_no_brake_implausability(false);
      }
      else {
        mcu_status.set_no_brake_implausability(true);
      }

      // FSAE EV.5.7
      // APPS/Brake Pedal Plausability Check
      if  (
        (
          (mcu_pedal_readings.get_accelerator_pedal_1() > ((END_ACCELERATOR_PEDAL_1 - START_ACCELERATOR_PEDAL_1) / 4 + START_ACCELERATOR_PEDAL_1))
          ||
          (mcu_pedal_readings.get_accelerator_pedal_2() > ((END_ACCELERATOR_PEDAL_2 - START_ACCELERATOR_PEDAL_2) / 4 + START_ACCELERATOR_PEDAL_2))
        )
        && mcu_status.get_brake_pedal_active()
      )
      {
        mcu_status.set_no_accel_brake_implausability(false);
      }
      else if
      (
        (mcu_pedal_readings.get_accelerator_pedal_1() < ((END_ACCELERATOR_PEDAL_1 - START_ACCELERATOR_PEDAL_1) / 20 + START_ACCELERATOR_PEDAL_1))
        &&
        (mcu_pedal_readings.get_accelerator_pedal_2() < ((END_ACCELERATOR_PEDAL_2 - START_ACCELERATOR_PEDAL_2) / 20 + START_ACCELERATOR_PEDAL_2))
      )
      {
        mcu_status.set_no_accel_brake_implausability(true);
      }

      if (
        mcu_status.get_no_brake_implausability() &&
        mcu_status.get_no_accel_implausability() &&
        mcu_status.get_no_accel_brake_implausability() &&
        mcu_status.get_bms_ok_high() &&
        mcu_status.get_imd_ok_high()
      ) {
        set_inverter_torques();
      } else {
        Serial.println("not calculating torque");
        Serial.printf("no brake implausibility: %d\n", mcu_status.get_no_brake_implausability());
        Serial.printf("no accel implausibility: %d\n", mcu_status.get_no_accel_implausability());
        Serial.printf("no accel brake implausibility: %d\n", mcu_status.get_no_accel_brake_implausability());
        Serial.printf("software is ok: %d\n", mcu_status.get_software_is_ok());
        Serial.printf("get bms ok high: %d\n", mcu_status.get_bms_ok_high());
        Serial.printf("get imd ok high: %d\n", mcu_status.get_imd_ok_high());

      }

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
    inverter_startup_state = INVERTER_STARTUP_STATE::WAIT_SYSTEM_READY;
  }
}


/* Implementation of software shutdown */
inline void software_shutdown() {

  mcu_status.set_software_is_ok(true);

  // check inputs
  // BMS heartbeat has not arrived within time interval
//  if (timer_bms_heartbeat.check()) {
//    timer_bms_heartbeat.interval(0);
//#if DEBUG
//    Serial.println("no bms heartbeat");
//#endif
//    mcu_status.set_software_is_ok(false);
//  }

  // add BMS software checks
  // software ok/not ok action
  if (mcu_status.get_software_is_ok()) {
    digitalWrite(SOFTWARE_OK, HIGH); //eventually make this HIGH only if software is ok
  } else {
    digitalWrite(SOFTWARE_OK, LOW);
  }
  /* Watchdog timer */
  if (timer_watchdog_timer.check()) {
    static bool watchdog_state = HIGH;
    watchdog_state = !watchdog_state;
    digitalWrite(WATCHDOG_INPUT, watchdog_state);
  }

}

/* Parse incoming CAN messages */
void parse_telem_can_message(const CAN_message_t &RX_msg) {
   CAN_message_t rx_msg = RX_msg;
  
   Serial.println(rx_msg.buf[2]);
    switch (rx_msg.id) {
      case ID_BMS_TEMPERATURES:              bms_temperatures.load(rx_msg.buf);              break;
      case ID_BMS_VOLTAGES:
        bms_voltages.load(rx_msg.buf);
        if (bms_voltages.get_low() < PACK_CHARGE_CRIT_LOWEST_CELL_THRESHOLD || bms_voltages.get_total() < PACK_CHARGE_CRIT_TOTAL_THRESHOLD) { 
          mcu_status.set_pack_charge_critical(true);
        } else mcu_status.set_pack_charge_critical(false);
        break;
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
          inverter_restart = true;
          timer_reset_inverter.reset();
        }
        
        // eliminate all action buttons to not process twice
        dashboard_status.set_button_flags(0);
        break;
    }
  
}

void parse_front_inv_can_message(const CAN_message_t &RX_msg) {
  static CAN_message_t rx_msg = RX_msg;

    switch (rx_msg.id) {
      case ID_MC1_STATUS:       mc_status[0].load(rx_msg.buf);    break;
      case ID_MC2_STATUS:       mc_status[1].load(rx_msg.buf);    break;
      case ID_MC1_TEMPS:        mc_temps[0].load(rx_msg.buf);    break;
      case ID_MC2_TEMPS:        mc_temps[1].load(rx_msg.buf);    break;
      case ID_MC1_ENERGY:       mc_energy[0].load(rx_msg.buf);    break;
      case ID_MC2_ENERGY:       mc_energy[1].load(rx_msg.buf);    break;
    }
  
}

void parse_rear_inv_can_message(const CAN_message_t &RX_msg) {
  static CAN_message_t rx_msg = RX_msg;

    switch (rx_msg.id) {
      case ID_MC3_STATUS:       mc_status[2].load(rx_msg.buf);    break;
      case ID_MC4_STATUS:       mc_status[3].load(rx_msg.buf);    break;
      case ID_MC3_TEMPS:        mc_temps[2].load(rx_msg.buf);    break;
      case ID_MC4_TEMPS:        mc_temps[3].load(rx_msg.buf);    break;
      case ID_MC3_ENERGY:       mc_energy[2].load(rx_msg.buf);    break;
      case ID_MC4_ENERGY:       mc_energy[3].load(rx_msg.buf);    break;
    }
  
}
//FIXME
inline void power_off_inverter() {
    digitalWrite(INVERTER_24V_EN, LOW);
  mcu_status.set_inverter_powered(false);

#if DEBUG
  Serial.println("INVERTER POWER OFF");
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


inline void set_inverter_torques() {
  int16_t torque_setpoint_array[4];
  int16_t speed_setpoint_array[4];

  int calculated_torque = 0;
  const int max_torque_Nm = mcu_status.get_max_torque();
  const float max_torque = max_torque_Nm / 0.0098; // max possible value for torque multiplier, unit in 0.1% nominal torque
  int accel1 = map(round(mcu_pedal_readings.get_accelerator_pedal_1()), START_ACCELERATOR_PEDAL_1, END_ACCELERATOR_PEDAL_1, 0, 1000);
  int accel2 = map(round(mcu_pedal_readings.get_accelerator_pedal_2()), START_ACCELERATOR_PEDAL_2, END_ACCELERATOR_PEDAL_2, 0, 1000);

  int brake1 = map(round(mcu_pedal_readings.get_brake_pedal_1()), START_BRAKE_PEDAL_1, BRAKE_THRESHOLD_MECH_BRAKE_1, 0, 1000);
  int brake2 = map(round(mcu_pedal_readings.get_brake_pedal_2()), START_BRAKE_PEDAL_2, BRAKE_THRESHOLD_MECH_BRAKE_1, 0, 1000);


  // torque values are greater than the max possible value, set them to max
  if (accel1 > max_torque) {
    accel1 = max_torque;
  }
  if (accel2 > max_torque) {
    accel2 = max_torque;
  }
  int avg_accel = (accel1 + accel2) / 2;
  int avg_brake = (brake1 + brake2) / 2;
  if (avg_accel > max_torque) {
    avg_accel = max_torque;
  }
  if (avg_accel < 0) {
    avg_accel = 0;
  }
  if (avg_accel > 1000) {
    avg_accel = max_torque;
  }
  if (avg_accel < 0) {
    avg_accel = 0;
  }
  



  if (mcu_status.get_launch_ctrl_active()) {

  } else {
    //currently in debug mode, no torque vectoring

    torque_setpoint_array[0] = avg_accel -  avg_brake * 2;
    torque_setpoint_array[1] = avg_accel -  avg_brake * 2;
    torque_setpoint_array[2] = avg_accel -  avg_brake * 2;
    torque_setpoint_array[3] = avg_accel -  avg_brake * 2;


  }
  for (int i = 0; i < sizeof(torque_setpoint_array); i++) {
    if (torque_setpoint_array[i] >= 0) {
      mc_setpoints_command[i].set_speed_setpoint(MC_MAX_SPEED);
      mc_setpoints_command[i].set_pos_torque_limit(torque_setpoint_array[i]);
      mc_setpoints_command[i].set_neg_torque_limit(0);
    }
    else {
      mc_setpoints_command[i].set_speed_setpoint(-MC_MAX_SPEED);
      mc_setpoints_command[i].set_pos_torque_limit(0);
      mc_setpoints_command[i].set_neg_torque_limit(torque_setpoint_array[i]);
    }
  }



  //power limit to 80kW
  //add this plz

}
/* Read 24_Sense_ECU which checks GLV voltage */
inline void read_glv_value() {
  if (timer_glv_read.check()) {
    mcu_analog_readings.set_glv_battery_voltage(ADC3.read_channel(ADC_GLV_READ_CHANNEL));
  }

}

/* Read pedal sensor values */
inline void read_pedal_values() {
  if (timer_pedals_read.check()) {
//        Serial.println(ADC1.read_channel(ADC_BRAKE_1_CHANNEL));
//        Serial.println(ADC1.read_channel(ADC_BRAKE_2_CHANNEL));
 
  mcu_pedal_readings.set_accelerator_pedal_1(ADC1.read_channel(ADC_ACCEL_1_CHANNEL));
  mcu_pedal_readings.set_accelerator_pedal_2(ADC1.read_channel(ADC_ACCEL_2_CHANNEL));
  mcu_pedal_readings.set_brake_pedal_1(ADC1.read_channel(ADC_BRAKE_1_CHANNEL));
  mcu_pedal_readings.set_brake_pedal_2(ADC1.read_channel(ADC_BRAKE_2_CHANNEL));

  

#if DEBUG
  int accel1 = map(round(mcu_pedal_readings.get_accelerator_pedal_1()), START_ACCELERATOR_PEDAL_1, END_ACCELERATOR_PEDAL_1, 0, 1000);
  int accel2 = map(round(mcu_pedal_readings.get_accelerator_pedal_2()), START_ACCELERATOR_PEDAL_2, END_ACCELERATOR_PEDAL_2, 0, 1000);

  int brake1 = map(round(mcu_pedal_readings.get_brake_pedal_1()), START_BRAKE_PEDAL_1, BRAKE_THRESHOLD_MECH_BRAKE_1, 0, 1000);
  int brake2 = map(round(mcu_pedal_readings.get_brake_pedal_2()), START_BRAKE_PEDAL_2, BRAKE_THRESHOLD_MECH_BRAKE_2, 0, 1000);
//   Serial.print("ACCEL 1: "); Serial.println(mcu_pedal_readings.get_accelerator_pedal_1());
//   Serial.print("ACCEL 2: "); Serial.println(mcu_pedal_readings.get_accelerator_pedal_2());
//    Serial.print("BRAKE 1: "); Serial.println(mcu_pedal_readings.get_brake_pedal_1());
//    Serial.print("BRAKE 2: "); Serial.println(mcu_pedal_readings.get_brake_pedal_2());

   Serial.print("ACCEL 1: "); Serial.println(accel1);
   Serial.print("ACCEL 2: "); Serial.println(accel2);
    Serial.print("BRAKE 1: "); Serial.println(brake1);
    Serial.print("BRAKE 2: "); Serial.println(brake2);

#endif

  // only uses front brake pedal
  mcu_status.set_brake_pedal_active(mcu_pedal_readings.get_brake_pedal_1() >= BRAKE_ACTIVE);
  digitalWrite(BRAKE_LIGHT_CTRL, mcu_status.get_brake_pedal_active());

  mcu_status.set_mech_brake_active(mcu_pedal_readings.get_brake_pedal_1() >= BRAKE_THRESHOLD_MECH_BRAKE_1); //define in driver_constraints.h (70%)
  }

}

inline void read_load_cell_values() {
  if (timer_load_cells_read.check()) {
    //load cell is 2mV/V, 10V excitation, 1000lb max
    //goes through 37.5x gain of INA823, 21x gain of OPA991, +0.314V offset, 0.1912x reduction on ECU and MAX7400 before reaching ADC
    mcu_load_cells.set_FL_load_cell((uint16_t) (((ADC2.read_channel(ADC_FL_LOAD_CELL_CHANNEL) / 0.1912) - 0.314) / 787.5 * 50));
    mcu_load_cells.set_FR_load_cell((uint16_t) (((ADC2.read_channel(ADC_FR_LOAD_CELL_CHANNEL) / 0.1912) - 0.314) / 787.5 * 50));
    mcu_load_cells.set_RL_load_cell((uint16_t) (((ADC1.read_channel(ADC_RL_LOAD_CELL_CHANNEL) / 0.1912) - 0.314) / 787.5 * 50));
    mcu_load_cells.set_RR_load_cell((uint16_t) (((ADC2.read_channel(ADC_RR_LOAD_CELL_CHANNEL) / 0.1912) - 0.314) / 787.5 * 50));
  }
}
inline void calibrate_load_cell_values() {
  //min is 45, calib is 15
  //270 89 calibration
   if (timer_load_cells_read.check()) {
    uint16_t dummy = ADC2.read_channel(ADC_FR_LOAD_CELL_CHANNEL);
    Serial.print("RAW: "); Serial.println(dummy);
    int mapCalib = map(dummy, 45, 270, 0, 100);
    Serial.print("Map Calib: "); Serial.println(mapCalib);
    
    //load cell is 2mV/V, 10V excitation, 1000lb max
    //goes through 37.5x gain of INA823, 21x gain of OPA991, +0.314V offset, 0.1912x reduction on ECU and MAX7400 before reaching ADC
//    mcu_load_cells.set_FL_load_cell((uint16_t) (((ADC2.read_channel(ADC_FL_LOAD_CELL_CHANNEL) / 0.1912) - 0.314) / 787.5 * 50));
    mcu_load_cells.set_FR_load_cell((uint16_t) (((dummy / 0.1912) - 0.314) / (787.5 * 50)));
//    mcu_load_cells.set_RL_load_cell((uint16_t) (((ADC1.read_channel(ADC_RL_LOAD_CELL_CHANNEL) / 0.1912) - 0.314) / 787.5 * 50));
//    mcu_load_cells.set_RR_load_cell((uint16_t) (((ADC2.read_channel(ADC_RR_LOAD_CELL_CHANNEL) / 0.1912) - 0.314) / 787.5 * 50));
//    Serial.print("Calibration: "); Serial.println(mcu_load_cells.get_FR_load_cell()); 

      Serial.print("Calibration: "); Serial.println((((dummy / 819.0 ) / 0.1912) - 0.314) / (787.5) * 50000);
      
  }
}

inline void read_steering_values() {
  if (timer_steering_read.check()) {
    mcu_analog_readings.set_steering_1(STEERING.read_steering());
    mcu_analog_readings.set_steering_2(ADC1.read_channel(ADC_STEERING_CHANNEL));

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
      mc_setpoints_command[inv].set_neg_torque_limit(limit);
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
  if (inverter_restart) {
    uint8_t reset_bit = !mc_setpoints_command[0].get_remove_error();
    if (timer_reset_inverter.check()) {
      inverter_restart = false;
      reset_bit = 0;
    }
    for (uint8_t inv = 0; inv < 4; inv++) {
      mc_setpoints_command[inv].set_remove_error(reset_bit);
    }
  }
}

/* Read shutdown system values */
inline void read_status_values() {
  /* Measure shutdown circuits' input */
  mcu_status.set_bms_ok_high(digitalRead(BMS_OK_READ));
  mcu_status.set_imd_ok_high(digitalRead(IMD_OK_READ));
  mcu_status.set_bspd_ok_high(digitalRead(BSPD_OK_READ));
  mcu_status.set_software_ok_high(digitalRead(SOFTWARE_OK_READ));

  /* Measure shutdown circuits' voltages */
  mcu_status.set_shutdown_b_above_threshold(digitalRead(BOTS_OK_READ));
  mcu_status.set_shutdown_c_above_threshold(digitalRead(IMD_OK_READ));
  mcu_status.set_shutdown_d_above_threshold(digitalRead(BMS_OK_READ));
  mcu_status.set_shutdown_e_above_threshold(digitalRead(BSPD_OK_READ));
}

// IMU functions
inline void read_imu() {
  imu_accelerometer.set_lat_accel(IMU.regRead(X_ACCL_OUT)); // * 0.00245); // 0.00245 is the scale, Left is positive
  imu_accelerometer.set_long_accel(IMU.regRead(Y_ACCL_OUT)); // * 0.00245); // 0.00245 is the scale, Backwards is positive, need to fix?
  imu_accelerometer.set_vert_accel(IMU.regRead(Z_ACCL_OUT)); // * 0.00245); // 0.00245 is the scale, Up is positive
  // question about yaw, pitch and roll rates?
  imu_gyroscope.set_pitch(IMU.regRead(X_GYRO_OUT)); // * 0.005); // 0.005 is the scale,
  imu_gyroscope.set_yaw(IMU.regRead(Z_GYRO_OUT)); // * 0.005);  // 0.005 is the scale
  imu_gyroscope.set_roll(IMU.regRead(Y_GYRO_OUT)); // * 0.005); // 0.005 is the scale
}

inline void send_CAN_IMU_accelerometer() {
  if (timer_CAN_imu_accelerometer_send.check()) {
    imu_accelerometer.write(msg.buf);
    msg.id = ID_IMU_ACCELEROMETER;
    msg.len = sizeof(imu_accelerometer);
    TELEM_CAN.write(msg);
  }
}

inline void send_CAN_IMU_gyroscope() {
  if (timer_CAN_imu_gyroscope_send.check()) {
    imu_gyroscope.write(msg.buf);
    msg.id = ID_IMU_GYROSCOPE;
    msg.len = sizeof(imu_gyroscope);
    TELEM_CAN.write(msg);
  }
}

inline void read_all_adcs(){
  
}
//#include <FlexCAN_T4.h>
//FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> Can0;
//
//
//void setup(void) {
//  Serial.begin(115200); delay(400);
//  Can0.begin();
//  Can0.setBaudRate(1000000);
//
//  
//  Can0.setMaxMB(16);
//  Can0.enableMBInterrupts();
//  Can0.onReceive(MB0,canSniff);
//
//  
//  Can0.mailboxStatus();
//}
//
//void canSniff(const CAN_message_t &msg) {
//  Serial.print("MB "); Serial.print(msg.mb);
//  Serial.print("  OVERRUN: "); Serial.print(msg.flags.overrun);
//  Serial.print("  LEN: "); Serial.print(msg.len);
//  Serial.print(" EXT: "); Serial.print(msg.flags.extended);
//  Serial.print(" TS: "); Serial.print(msg.timestamp);
//  Serial.print(" ID: "); Serial.print(msg.id, HEX);
//  Serial.print(" Buffer: ");
//  for ( uint8_t i = 0; i < msg.len; i++ ) {
//    Serial.print(msg.buf[i], HEX); Serial.print(" ");
//  } Serial.println();
//}
//
//void loop() {
//  Can0.events();
//}
