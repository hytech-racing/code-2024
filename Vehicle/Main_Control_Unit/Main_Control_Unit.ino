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
#include <time.h>
#include <deque>

#include "ADC_SPI.h"
#include "STEERING_SPI.h"
#include "kinetis_flexcan.h"
#include "Metro.h"

// IMU
#include <ADIS16460.h>

#include "drivers.h"

// constants to define for different operation

#define DRIVER DEFAULT_DRIVER
#define TORQUE_1 10
#define TORQUE_2 15
#define TORQUE_3 21
#define MAX_ALLOWED_SPEED 20000

// set to true or false for debugging
#define DEBUG false
#define BMS_DEBUG_ENABLE false

#include "MCU_rev12_dfs.h"

#include "driver_constants.h"

// Call the ADIS16460 IMU class
ADIS16460 IMU(IMU_CS, IMU_DATAREADY, IMU_RESET); // Chip Select, Data Ready, Reset Pin Assignments

// Outbound CAN messages
MCU_pedal_readings mcu_pedal_readings;
MCU_status mcu_status{};
MCU_load_cells mcu_load_cells{};
MCU_front_potentiometers mcu_front_potentiometers;
MCU_rear_potentiometers mcu_rear_potentiometers;
MCU_analog_readings mcu_analog_readings{};

// IMU
IMU_accelerometer imu_accelerometer;
IMU_gyroscope imu_gyroscope;
double imu_velocity;
double pitch_calibration_angle;

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
Metro timer_imu_integration = Metro(50);
Metro timer_CAN_imu_accelerometer_send = Metro(50);
Metro timer_CAN_imu_gyroscope_send = Metro(50);
Metro timer_CAN_inverter_setpoints_send = Metro(20);
Metro timer_CAN_coloumb_count_send = Metro(1000);
Metro timer_CAN_mcu_status_send = Metro(100);
Metro timer_CAN_mcu_pedal_readings_send = Metro(50);
Metro timer_CAN_mcu_analog_readings_send = Metro(50);
Metro timer_CAN_mcu_load_cells_send = Metro(20);

Metro timer_ready_sound = Metro(2000); // Time to play RTD sound

Metro timer_read_all_adcs = Metro(20);
Metro timer_steering_spi_read = Metro(1000);
Metro timer_read_imu = Metro(20);

Metro timer_inverter_enable = Metro(5000);
Metro timer_reset_inverter = Metro(5000);
Metro timer_watchdog_timer = Metro(10);

elapsedMillis pedal_implausability_duration;

Metro timer_debug = Metro(200);
Metro timer_debug2 = Metro(1000);

// this abuses Metro timer functionality to stay faulting once a fault has occurred
// autoreset makes the timer update to the current time and not by the interval
// this allows me to set the interval as 0 once a fault has occurred, leading to continuous faulting
// until a CAN message comes in which resets the timer and the interval
Metro timer_bms_heartbeat = Metro(0, 1);


bool imd_faulting = false;
bool inverter_restart = false; // True when restarting the inverter
INVERTER_STARTUP_STATE inverter_startup_state = INVERTER_STARTUP_STATE::WAIT_SYSTEM_READY;

ADC_SPI ADC1(ADC1_CS, ADC_SPI_SPEED, ECU_SDI, ECU_SDO, ECU_CLK);
ADC_SPI ADC2(ADC2_CS, ADC_SPI_SPEED, ECU_SDI, ECU_SDO, ECU_CLK);
ADC_SPI ADC3(ADC3_CS, ADC_SPI_SPEED, ECU_SDI, ECU_SDO, ECU_CLK);

STEERING_SPI STEERING(STEERING_CS, STEERING_SPI_SPEED);

FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> FRONT_INV_CAN;
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> REAR_INV_CAN;
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> TELEM_CAN;
CAN_message_t msg;

// coloumb counts
uint32_t total_discharge;
unsigned long previous_data_time;

int16_t torque_setpoint_array[4];
int16_t speed_setpoint_array[4] = {0, 0, 0, 0};

uint16_t prev_load_cell_readings[4] = {0, 0, 0, 0};
float load_cell_alpha = 0.95;

float filtered_min_cell_voltage = 3.5;
float cell_voltage_alpha = 0.8;

float filtered_max_cell_temp = 40.0;
float cell_temp_alpha = 0.8;

uint16_t current_read = 0;
uint16_t reference_read = 0;

float max_front_power = 0.0;
float max_rear_power = 0.0;

enum launch_states {launch_not_ready, launch_ready, launching};
launch_states launch_state = launch_not_ready;
int16_t launch_speed_target = 0;
elapsedMillis time_since_launch;
const uint16_t LAUNCH_READY_ACCEL_THRESHOLD = 100;
const uint16_t LAUNCH_READY_BRAKE_THRESHOLD = 300;
const int16_t LAUNCH_READY_SPEED_THRESHOLD = 500;
const uint16_t LAUNCH_GO_THRESHOLD = 900;
const uint16_t LAUNCH_STOP_THRESHOLD = 600;
float launch_rate_target = 0.0;

void setup() {
  // no torque can be provided on startup


  mcu_status.set_max_torque(0);
  mcu_status.set_torque_mode(0);
  mcu_status.set_software_is_ok(true);

  set_all_inverters_disabled();


  //   IMU set up
  IMU.regWrite(MSC_CTRL, 0xC1);  // Enable Data Ready, set polarity
  delay(20);
  IMU.regWrite(FLTR_CTRL, 0x504); // Set digital filter
  delay(20);
  IMU.regWrite(DEC_RATE, 0), // Disable decimation
               delay(20);

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

  pinMode(ECU_CLK, OUTPUT);
  pinMode(ECU_SDI, INPUT);
  pinMode(ECU_SDO, OUTPUT);

#if DEBUG
  Serial.begin(115200);
#endif
  FRONT_INV_CAN.begin();
  FRONT_INV_CAN.setBaudRate(500000);
  REAR_INV_CAN.begin();
  REAR_INV_CAN.setBaudRate(500000);
  TELEM_CAN.begin();
  TELEM_CAN.setBaudRate(500000);
  FRONT_INV_CAN.enableMBInterrupts();
  REAR_INV_CAN.enableMBInterrupts();
  TELEM_CAN.enableMBInterrupts();
  FRONT_INV_CAN.onReceive(parse_front_inv_can_message);
  REAR_INV_CAN.onReceive(parse_rear_inv_can_message);
  TELEM_CAN.onReceive(parse_telem_can_message);
  delay(500);

#if DEBUG
  Serial.println("CAN system and serial communication initialized");
#endif


  // these are false by default
  mcu_status.set_bms_ok_high(false);
  mcu_status.set_imd_ok_high(false);

  digitalWrite(INVERTER_24V_EN, HIGH);
  digitalWrite(INVERTER_EN, HIGH);
  mcu_status.set_inverters_error(false);



  // present action for 5s
  delay(5000);

  set_state(MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE);
  mcu_status.set_max_torque(TORQUE_3);
  mcu_status.set_torque_mode(3);

  /* Set up total discharge readings */
  //setup_total_discharge();

}

void loop() {
  FRONT_INV_CAN.events();
  REAR_INV_CAN.events();
  TELEM_CAN.events();

  read_all_adcs();
  //read_steering_spi_values();
  read_status_values();
  read_imu();

  send_CAN_mcu_status();
  send_CAN_mcu_pedal_readings();
  send_CAN_mcu_load_cells();
  send_CAN_mcu_potentiometers();
  send_CAN_mcu_analog_readings();
  send_CAN_imu_accelerometer();
  send_CAN_imu_gyroscope();
  send_CAN_inverter_setpoints();

  //  /* Finish restarting the inverter when timer expires */
  check_all_inverters_error();
  reset_inverters();
  //
  //  /* handle state functionality */
  state_machine();
  software_shutdown();



  if (timer_debug.check()) {
    Serial.println("ERROR");
    Serial.println(check_all_inverters_error());
    Serial.println(mc_energy[0].get_dc_bus_voltage());
    Serial.println(mc_temps[0].get_diagnostic_number());
    Serial.println(mc_temps[1].get_diagnostic_number());
    Serial.println(mc_temps[2].get_diagnostic_number());
    Serial.println(mc_temps[3].get_diagnostic_number());
    Serial.println();
    Serial.println(mcu_pedal_readings.get_accelerator_pedal_1());
    Serial.println(mcu_pedal_readings.get_accelerator_pedal_2());
    Serial.println(mcu_pedal_readings.get_brake_pedal_1());
    Serial.println(mcu_pedal_readings.get_brake_pedal_2());
    //calculate_pedal_implausibilities();
//    Serial.println(mcu_status.get_no_accel_implausability());
//    Serial.println(mcu_status.get_no_brake_implausability());
//    Serial.println(mcu_status.get_no_accel_brake_implausability());
//    int brake1 = map(round(mcu_pedal_readings.get_brake_pedal_1()), START_BRAKE_PEDAL_1, END_BRAKE_PEDAL_1, 0, 2140);
//    int brake2 = map(round(mcu_pedal_readings.get_brake_pedal_2()), START_BRAKE_PEDAL_2, END_BRAKE_PEDAL_2, 0, 2140);
//    Serial.println(brake1);
//    Serial.println(brake2);
    Serial.println();
    Serial.println("LOAD CELLS");
    Serial.println(mcu_load_cells.get_FL_load_cell());
    Serial.println(mcu_load_cells.get_FR_load_cell());
    Serial.println(mcu_load_cells.get_RL_load_cell());
    Serial.println(mcu_load_cells.get_RR_load_cell());
    Serial.println(torque_setpoint_array[0]);
    Serial.println(torque_setpoint_array[1]);
    Serial.println(torque_setpoint_array[2]);
    Serial.println(torque_setpoint_array[3]);
    Serial.println("MOTOR TEMPS");
    Serial.println(mc_temps[0].get_motor_temp());
    Serial.println(mc_temps[1].get_motor_temp());
    Serial.println(mc_temps[2].get_motor_temp());
    Serial.println(mc_temps[3].get_motor_temp());
    Serial.println(mc_temps[3].get_igbt_temp());
    Serial.println("IMU");
    Serial.println(imu_accelerometer.get_vert_accel());
    Serial.println(imu_gyroscope.get_yaw());
    Serial.println("dial");
    Serial.println(dashboard_status.get_dial_state());
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

inline void send_CAN_mcu_potentiometers() {
  if (timer_CAN_mcu_load_cells_send.check()) {
    mcu_front_potentiometers.write(msg.buf);
    msg.id = ID_MCU_FRONT_POTS;
    msg.len = sizeof(mcu_front_potentiometers);
    TELEM_CAN.write(msg);
    mcu_rear_potentiometers.write(msg.buf);
    msg.id = ID_MCU_REAR_POTS;
    msg.len = sizeof(mcu_rear_potentiometers);
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

inline void send_CAN_mcu_analog_readings() {
  if (timer_CAN_mcu_analog_readings_send.check()) {
    mcu_analog_readings.write(msg.buf);
    msg.id = ID_MCU_ANALOG_READINGS;
    msg.len = sizeof(mcu_analog_readings);
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
      if (check_all_inverters_system_ready()) {
        set_all_inverters_dc_on(true);
        inverter_startup_state = INVERTER_STARTUP_STATE::WAIT_QUIT_DC_ON;
      }
      if (dashboard_status.get_start_btn() && mcu_status.get_mech_brake_active()) {
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
            set_all_inverters_inverter_enable(true);
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

      calculate_pedal_implausibilities();

      if (
        pedal_implausability_duration <= 100 &&
        mcu_status.get_bms_ok_high() &&
        mcu_status.get_imd_ok_high()

      ) {
        set_inverter_torques();
      } else if (mcu_status.get_bms_ok_high() && mcu_status.get_imd_ok_high()) {
        set_inverter_torques_regen_only();
      } else {
        Serial.println("not calculating torque");
        Serial.printf("no brake implausibility: %d\n", mcu_status.get_no_brake_implausability());
        Serial.printf("no accel implausibility: %d\n", mcu_status.get_no_accel_implausability());
        Serial.printf("no accel brake implausibility: %d\n", mcu_status.get_no_accel_brake_implausability());
        Serial.printf("software is ok: %d\n", mcu_status.get_software_is_ok());
        Serial.printf("get bms ok high: %d\n", mcu_status.get_bms_ok_high());
        Serial.printf("get imd ok high: %d\n", mcu_status.get_imd_ok_high());
        set_all_inverters_no_torque();
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
  switch (rx_msg.id) {
    case ID_BMS_TEMPERATURES:              
      bms_temperatures.load(rx_msg.buf);
      filtered_max_cell_temp  = filtered_max_cell_temp * cell_temp_alpha + (1.0 - cell_temp_alpha) * (bms_temperatures.get_high_temperature() / 100.0) ;              
      break;
    case ID_BMS_VOLTAGES:
      bms_voltages.load(rx_msg.buf);
      if (bms_voltages.get_low() < PACK_CHARGE_CRIT_LOWEST_CELL_THRESHOLD || bms_voltages.get_total() < PACK_CHARGE_CRIT_TOTAL_THRESHOLD) {
        mcu_status.set_pack_charge_critical(true);
      } else mcu_status.set_pack_charge_critical(false);
      filtered_min_cell_voltage = filtered_min_cell_voltage * cell_voltage_alpha + (1.0 - cell_voltage_alpha) * (bms_voltages.get_low() / 10000.0);
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
      if (dashboard_status.get_torque_mode_btn()) {
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
  CAN_message_t rx_msg = RX_msg;
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
  CAN_message_t rx_msg = RX_msg;

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
    case MCU_STATE::READY_TO_DRIVE: {
        inverter_startup_state = INVERTER_STARTUP_STATE::WAIT_SYSTEM_READY;
        break;
      }
  }

  mcu_status.set_state(new_state);

  // entry logic
  switch (new_state) {
    case MCU_STATE::STARTUP: break;
    case MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE: break;
    case MCU_STATE::TRACTIVE_SYSTEM_ACTIVE: {
        inverter_startup_state = INVERTER_STARTUP_STATE::WAIT_SYSTEM_READY;
        break;
      }
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

inline float float_map(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

inline void set_inverter_torques_regen_only() {
  int brake1 = map(round(mcu_pedal_readings.get_brake_pedal_1()), START_BRAKE_PEDAL_1, END_BRAKE_PEDAL_1, 0, 2140);
  int brake2 = map(round(mcu_pedal_readings.get_brake_pedal_2()), START_BRAKE_PEDAL_2, END_BRAKE_PEDAL_2, 0, 2140);
  int avg_brake = (brake1 + brake2) / 2;
  if (avg_brake > 1500) {
    avg_brake = 1500;
  }
  if (avg_brake < 0) {
    avg_brake = 0;
  }
  torque_setpoint_array[0] = - avg_brake;
  torque_setpoint_array[1] = -  avg_brake;
  torque_setpoint_array[2] =  - avg_brake;
  torque_setpoint_array[3] =  - avg_brake;
  for (int i = 0; i < 4; i++) {
    if (i < 2) {
      torque_setpoint_array[i] = (int16_t)(torque_setpoint_array[i] * 1.33);
    } else {
      torque_setpoint_array[i] = (int16_t)(torque_setpoint_array[i] * 0.66);
    }

  }
  for (int i = 0; i < 4; i++) {
    if (torque_setpoint_array[i] >= 0) {
    }
    else {
      int16_t max_speed_regen = 0;
      for (int i = 0; i < sizeof(torque_setpoint_array); i++) {

        max_speed_regen = (max_speed_regen < mc_status[i].get_speed()) ? mc_status[i].get_speed() : max_speed_regen;

      }
      float scale_down = 1;
      if (max_speed_regen < 770) {
        scale_down = 0;
      } else if (max_speed_regen > REGEN_OFF_START_THRESHOLD) {
        scale_down = 1;
      } else {
        scale_down = map(max_speed_regen, 770, REGEN_OFF_START_THRESHOLD, 0, 1);
      }
      mc_setpoints_command[i].set_speed_setpoint(0);
      mc_setpoints_command[i].set_pos_torque_limit(0);
      mc_setpoints_command[i].set_neg_torque_limit(max(((int16_t)(torque_setpoint_array[i]) * scale_down) , -2140) );

    }
  }
}

inline void set_inverter_torques() {

  float max_torque = mcu_status.get_max_torque() / 0.0098; // max possible value for torque multiplier, unit in 0.1% nominal torque
  int accel1 = map(round(mcu_pedal_readings.get_accelerator_pedal_1()), START_ACCELERATOR_PEDAL_1, END_ACCELERATOR_PEDAL_1, 0, 2140);
  int accel2 = map(round(mcu_pedal_readings.get_accelerator_pedal_2()), START_ACCELERATOR_PEDAL_2, END_ACCELERATOR_PEDAL_2, 0, 2140);

  int brake1 = map(round(mcu_pedal_readings.get_brake_pedal_1()), START_BRAKE_PEDAL_1, END_BRAKE_PEDAL_1, 0, 2140);
  int brake2 = map(round(mcu_pedal_readings.get_brake_pedal_2()), START_BRAKE_PEDAL_2, END_BRAKE_PEDAL_2, 0, 2140);


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
  if (avg_accel > max_torque) {
    avg_accel = max_torque;
  }
  if (avg_accel < 0) {
    avg_accel = 0;
  }
  if (avg_brake > 1500) {
    avg_brake = 1500;
  }
  if (avg_brake < 0) {
    avg_brake = 0;
  }

  float rear_power_balance = 0.66;
  float front_power_balance = 1.0 - rear_power_balance;
  float rear_brake_balance = 0.1;
  float front_brake_balance = 1.0 - rear_brake_balance;

  int32_t total_torque = 0;
  int32_t total_load_cells = 0;
  float front_load_total;
  float rear_load_total;

  float attesa_def_split = 0.85;
  float attesa_alt_split = 0.5;
  float fr_slip_clamped;
  float fr_slip_factor = 2.5; // Factor of 5 causes 50/50 split at 20% r/f slip. Lower values allow more slip
  float f_torque;
  float r_torque;
  float rear_lr_slip_clamped;
  float lsd_right_split; // Fraction of rear axle torque going to rear right wheel
  float lsd_slip_factor = 0.5;

  float avg_speed = 0.0;
  for (int i = 0; i < 4; i++)
    avg_speed += ((float) mc_status[i].get_speed()) / 4.0;
  int16_t start_derating_rpm = 2000;
  int16_t end_derating_rpm = 20000;

  const float hairpin_rpm_limit = 5600.0;
  const float hairpin_rpm_full = 2800.0;
  float hairpin_rpm_factor = 0.0;
  const float hairpin_steering_min = 80.0; // degrees
  const float hairpin_steering_max = 120.0; // degrees
  float steering_calibration_slope = -0.111;
  float steering_calibration_offset = 260.0;
  // positive steering angle is to the left
  float steering_angle = mcu_analog_readings.get_steering_2() * steering_calibration_slope + steering_calibration_offset;
  float hairpin_reallocation = 0.0; 
  float hairpin_steering_factor = 0.0;

  int16_t max_speed;

  switch (dashboard_status.get_dial_state()) {
    case 0:
      for (int i = 0; i < 4; i++) {
        speed_setpoint_array[i] = MAX_ALLOWED_SPEED;
      }
      launch_state = launch_not_ready;
      // standard no torque vectoring

      max_front_power = 19000.0;
      max_rear_power = 36000.0;

      torque_setpoint_array[0] = avg_accel -  avg_brake;
      torque_setpoint_array[1] = avg_accel -  avg_brake;
      torque_setpoint_array[2] = avg_accel - avg_brake;
      torque_setpoint_array[3] = avg_accel - avg_brake;

      for (int i = 0; i < 4; i++) {
        if (torque_setpoint_array[i] >= 0) {
          if (i < 2) {
            torque_setpoint_array[i] = (int16_t)(torque_setpoint_array[i] * front_power_balance);
          } else {
            torque_setpoint_array[i] = (int16_t)(torque_setpoint_array[i] * rear_power_balance);
          }
        } else {
          if (i < 2) {
            torque_setpoint_array[i] = (int16_t)(torque_setpoint_array[i] * front_brake_balance);
          } else {
            torque_setpoint_array[i] = (int16_t)(torque_setpoint_array[i] * rear_brake_balance);
          }
        }
      }
      break;
    case 1:
      for (int i = 0; i < 4; i++) {
        speed_setpoint_array[i] = MAX_ALLOWED_SPEED;
      }
      launch_state = launch_not_ready;
      // Original load cell torque vectoring

    
      max_front_power = 19000.0;
      max_rear_power = 36000.0;

      
      load_cell_alpha = 0.95;
      total_torque = 4 * (avg_accel - avg_brake) ;
      total_load_cells = mcu_load_cells.get_FL_load_cell() + mcu_load_cells.get_FR_load_cell() + mcu_load_cells.get_RL_load_cell() + mcu_load_cells.get_RR_load_cell();
      if (avg_accel >= avg_brake) {
        torque_setpoint_array[0] = (int16_t)((float)mcu_load_cells.get_FL_load_cell() / (float)total_load_cells * (float)total_torque);
        torque_setpoint_array[1] = (int16_t)((float)mcu_load_cells.get_FR_load_cell() / (float)total_load_cells * (float)total_torque);
        torque_setpoint_array[2] = (int16_t)((float)mcu_load_cells.get_RL_load_cell() / (float)total_load_cells * (float)total_torque);
        torque_setpoint_array[3] = (int16_t)((float)mcu_load_cells.get_RR_load_cell() / (float)total_load_cells * (float)total_torque);
      } else {
        torque_setpoint_array[0] = (int16_t)((float)mcu_load_cells.get_FL_load_cell() / (float)total_load_cells * (float)total_torque);
        torque_setpoint_array[1] = (int16_t)((float)mcu_load_cells.get_FR_load_cell() / (float)total_load_cells * (float)total_torque);
        torque_setpoint_array[2] = (int16_t)((float)mcu_load_cells.get_RL_load_cell() / (float)total_load_cells * (float)total_torque / 2.0);
        torque_setpoint_array[3] = (int16_t)((float)mcu_load_cells.get_RR_load_cell() / (float)total_load_cells * (float)total_torque / 2.0);
      }
      break;
    case 2:
      max_speed = 0;
      launch_rate_target = 11.76;
      for (int i = 0; i < 4; i++) {
        max_speed = max(max_speed, mc_status[i].get_speed());
      }
//      max_front_power = 19000.0;
//      max_rear_power = 36000.0;
      max_front_power = 21760.0;
      max_rear_power = 41230.0;

      switch (launch_state) {
        case launch_not_ready:
          for (int i = 0; i < 4; i++) {
            torque_setpoint_array[i] = (int16_t)(-1 * avg_brake);
            speed_setpoint_array[i] = 0;
          }
          time_since_launch = 0;
          launch_speed_target = 0;

          // To enter launch_ready, the following conditions must be true:
          // 1. Pedals are not pressed
          // 2. Speed is zero
          if (avg_accel < LAUNCH_READY_ACCEL_THRESHOLD && avg_brake < LAUNCH_READY_BRAKE_THRESHOLD && max_speed < LAUNCH_READY_SPEED_THRESHOLD) {
            launch_state = launch_ready;
          }
          break;
        case launch_ready:
          for (int i = 0; i < 4; i++) {
            torque_setpoint_array[i] = 0;
            speed_setpoint_array[i] = 0;
          }
          time_since_launch = 0;
          launch_speed_target = 0;

          // Revert to launch_not_ready if brake is pressed or speed is too high
          if (avg_brake >= LAUNCH_READY_BRAKE_THRESHOLD || max_speed >= LAUNCH_READY_SPEED_THRESHOLD) {
            launch_state = launch_not_ready;
          } else {
            // Otherwise, check if launch should begin
            if (avg_accel >= LAUNCH_GO_THRESHOLD) {
              launch_state = launching;
            }
          }

          break;
        case launching:
          // Exit launch if accel pedal goes past STOP threshold or brake pedal is pressed
          if (avg_accel <= LAUNCH_STOP_THRESHOLD || avg_brake >= LAUNCH_READY_BRAKE_THRESHOLD) {
            launch_state = launch_not_ready;
            break;
          }

          launch_speed_target = (int16_t)((float) time_since_launch / 1000.0 * launch_rate_target * 60.0 / 1.2767432544 * 11.86);
          launch_speed_target += 1500;
          launch_speed_target = min(20000, max(0, launch_speed_target));

          for (int i = 0; i < 4; i++) {
            torque_setpoint_array[i] = 2142;
            speed_setpoint_array[i] = launch_speed_target;
          }
          break;
        default:
          break;
      }

      break;
    case 3:
      max_speed = 0;
      launch_rate_target = 12.74;
      for (int i = 0; i < 4; i++) {
        max_speed = max(max_speed, mc_status[i].get_speed());
      }
//      max_front_power = 19000.0;
//      max_rear_power = 36000.0;
      max_front_power = 21760.0;
      max_rear_power = 41230.0;

      switch (launch_state) {
        case launch_not_ready:
          for (int i = 0; i < 4; i++) {
            torque_setpoint_array[i] = (int16_t)(-1 * avg_brake);
            speed_setpoint_array[i] = 0;
          }
          time_since_launch = 0;
          launch_speed_target = 0;

          // To enter launch_ready, the following conditions must be true:
          // 1. Pedals are not pressed
          // 2. Speed is zero
          if (avg_accel < LAUNCH_READY_ACCEL_THRESHOLD && avg_brake < LAUNCH_READY_BRAKE_THRESHOLD && max_speed < LAUNCH_READY_SPEED_THRESHOLD) {
            launch_state = launch_ready;
          }
          break;
        case launch_ready:
          for (int i = 0; i < 4; i++) {
            torque_setpoint_array[i] = 0;
            speed_setpoint_array[i] = 0;
          }
          time_since_launch = 0;
          launch_speed_target = 0;

          // Revert to launch_not_ready if brake is pressed or speed is too high
          if (avg_brake >= LAUNCH_READY_BRAKE_THRESHOLD || max_speed >= LAUNCH_READY_SPEED_THRESHOLD) {
            launch_state = launch_not_ready;
          } else {
            // Otherwise, check if launch should begin
            if (avg_accel >= LAUNCH_GO_THRESHOLD) {
              launch_state = launching;
            }
          }

          break;
        case launching:
          // Exit launch if accel pedal goes past STOP threshold or brake pedal is pressed
          if (avg_accel <= LAUNCH_STOP_THRESHOLD || avg_brake >= LAUNCH_READY_BRAKE_THRESHOLD) {
            launch_state = launch_not_ready;
            break;
          }

          launch_speed_target = (int16_t)((float) time_since_launch / 1000.0 * launch_rate_target * 60.0 / 1.2767432544 * 11.86);
          launch_speed_target += 1500;
          launch_speed_target = min(20000, max(0, launch_speed_target));

          for (int i = 0; i < 4; i++) {
            torque_setpoint_array[i] = 2142;
            speed_setpoint_array[i] = launch_speed_target;
          }
          break;
        default:
          break;
      }

      break;
    case 4:
      // Copy pasted from mode 2 with additional derating for endurance
      for (int i = 0; i < 4; i++) {
        speed_setpoint_array[i] = MAX_ALLOWED_SPEED;
      }
      max_front_power = 19000.0;
      max_rear_power = 36000.0;
      launch_state = launch_not_ready;
      // Original load cell torque vectoring
      load_cell_alpha = 0.95;
      total_torque = 4 * (avg_accel - avg_brake) ;
      total_load_cells = mcu_load_cells.get_FL_load_cell() + mcu_load_cells.get_FR_load_cell() + mcu_load_cells.get_RL_load_cell() + mcu_load_cells.get_RR_load_cell();

      // Derating
      float derating_factor = float_map(avg_speed, start_derating_rpm, end_derating_rpm, 1.0, 0.0);
      derating_factor = min(1.0, max(0.0, derating_factor));

      if (avg_accel >= avg_brake) {
        torque_setpoint_array[0] = (int16_t)((float)mcu_load_cells.get_FL_load_cell() / (float)total_load_cells * (float)total_torque * derating_factor);
        torque_setpoint_array[1] = (int16_t)((float)mcu_load_cells.get_FR_load_cell() / (float)total_load_cells * (float)total_torque * derating_factor);
        torque_setpoint_array[2] = (int16_t)((float)mcu_load_cells.get_RL_load_cell() / (float)total_load_cells * (float)total_torque * derating_factor);
        torque_setpoint_array[3] = (int16_t)((float)mcu_load_cells.get_RR_load_cell() / (float)total_load_cells * (float)total_torque * derating_factor);
      } else {
        torque_setpoint_array[0] = (int16_t)((float)mcu_load_cells.get_FL_load_cell() / (float)total_load_cells * (float)total_torque);
        torque_setpoint_array[1] = (int16_t)((float)mcu_load_cells.get_FR_load_cell() / (float)total_load_cells * (float)total_torque);
        torque_setpoint_array[2] = (int16_t)((float)mcu_load_cells.get_RL_load_cell() / (float)total_load_cells * (float)total_torque / 2.0);
        torque_setpoint_array[3] = (int16_t)((float)mcu_load_cells.get_RR_load_cell() / (float)total_load_cells * (float)total_torque / 2.0);
      }
      break;
    case 5:
      for (int i = 0; i < 4; i++) {
        speed_setpoint_array[i] = MAX_ALLOWED_SPEED;
      }
      launch_state = launch_not_ready;
      // Original load cell torque vectoring

    
      max_front_power = 21760.0;
      max_rear_power = 41240.0;

      
      load_cell_alpha = 0.95;
      total_torque = 4 * (avg_accel - avg_brake) ;
      total_load_cells = mcu_load_cells.get_FL_load_cell() + mcu_load_cells.get_FR_load_cell() + mcu_load_cells.get_RL_load_cell() + mcu_load_cells.get_RR_load_cell();
      if (avg_accel >= avg_brake) {
        torque_setpoint_array[0] = (int16_t)((float)mcu_load_cells.get_FL_load_cell() / (float)total_load_cells * (float)total_torque);
        torque_setpoint_array[1] = (int16_t)((float)mcu_load_cells.get_FR_load_cell() / (float)total_load_cells * (float)total_torque);
        torque_setpoint_array[2] = (int16_t)((float)mcu_load_cells.get_RL_load_cell() / (float)total_load_cells * (float)total_torque);
        torque_setpoint_array[3] = (int16_t)((float)mcu_load_cells.get_RR_load_cell() / (float)total_load_cells * (float)total_torque);
      } else {
        torque_setpoint_array[0] = (int16_t)((float)mcu_load_cells.get_FL_load_cell() / (float)total_load_cells * (float)total_torque);
        torque_setpoint_array[1] = (int16_t)((float)mcu_load_cells.get_FR_load_cell() / (float)total_load_cells * (float)total_torque);
        torque_setpoint_array[2] = (int16_t)((float)mcu_load_cells.get_RL_load_cell() / (float)total_load_cells * (float)total_torque / 2.0);
        torque_setpoint_array[3] = (int16_t)((float)mcu_load_cells.get_RR_load_cell() / (float)total_load_cells * (float)total_torque / 2.0);
      }
      break;
  }




  //very start check if mc_energy.get_feedback_torque > 0
  //power limit to 80kW
  //look at all torques
  //look at motor speeds / convert from rpm to angular speed
  //torque * speed / 1000 (kW)
  // scale down by m/e limits
  //lots of variables for documentation purposes
  //since torque unit to nominal torque and power conversion are linear, the diff can be applied directly to the torque setpoint value.
  if (mc_setpoints_command[0].get_pos_torque_limit() > 0 && mc_setpoints_command[1].get_pos_torque_limit() > 0
      && mc_setpoints_command[2].get_pos_torque_limit() > 0 && mc_setpoints_command[3].get_pos_torque_limit() > 0) {
    float mech_power = 0;
    float mdiff = 1;
    //float ediff = 1;
    // float pw_lim_factor = 1.0;

    float voltage_lim_factor = 1.0;
    float temp_lim_factor = 1.0;
    float accu_lim_factor = 1.0;

    for (int i = 0; i < 4; i++) {
      float torque_in_nm = 9.8 * ((float) mc_setpoints_command[i].get_pos_torque_limit()) / 1000.0;
      float speed_in_rpm = (float) mc_status[i].get_speed();
      mech_power += 2 * 3.1415 * torque_in_nm * speed_in_rpm / 60.0;
    }

    // pw_lim_factor = float_map(mech_power, 40000.0, 55000.0, 1.0, 0);
    // pw_lim_factor = max(min(1.0, pw_lim_factor), 0.0);

    voltage_lim_factor = float_map(filtered_min_cell_voltage, 3.5, 3.2, 1.0, 0.2);
    voltage_lim_factor = max(min(1.0, voltage_lim_factor), 0.2);

    temp_lim_factor = float_map(filtered_max_cell_temp, 50.0, 58.0, 1.0, 0.2);
    temp_lim_factor = max(min(1.0, temp_lim_factor), 0.2);

    accu_lim_factor = min(temp_lim_factor, voltage_lim_factor);
    //mech_power /= 1000.0;

    //      float current = (ADC1.read_channel(ADC_CURRENT_CHANNEL) - ADC1.read_channel(ADC_REFERENCE_CHANNEL));
    //      current = ((((current / 819.0) / .1912) / 4.832) - 2.5) * 1000) / 6.67;

    //
    //      float dc_power = (mc_energy[0].get_dc_bus_voltage() * current) / 1000; //mc dc bus voltage

    //sum up kilowatts to align
    //if mech_power is at 63 kW, it's requesting 80 kW from the motor
    //2 kW safety factor for the more accurate motor readings.
    //as our effiecency increases say 68 kW would be drawing 80kW from the motor
    //as our efficiency decreases say 60 kW would be drawing 80kW from the motor
    //so if efficency is at 60kW and we want 63, we'd be drawing more from the battery triggering a safety problem
    //so if efficency is at 68 kW, 63 would be drawing less power, which is fine but wasted power.
    //if HV DC bus is over 80 kW, it's a violation!
    // 1 kW as a second safety factor.
    //if (mech_power > MECH_POWER_LIMIT) {
      // mdiff = MECH_POWER_LIMIT / mech_power;
      // diff = MECH_POWER_LIMIT / mech_power;
    //}
    //      if (dc_power > DC_POWER_LIMIT) {
    //        ediff = DC_POWER_LIMIT / dc_power;
    //      }
    //      if (mech_power > MECH_POWER_LIMIT && dc_power > DC_POWER_LIMIT) {
    //        diff = (ediff <= mdiff) ? ediff : mdiff;
    //      }
    torque_setpoint_array[0] = (uint16_t) (min((float) torque_setpoint_array[0], max_allowed_torque(max_front_power / 2.0, (float) mc_status[0].get_speed())) * accu_lim_factor);
    torque_setpoint_array[1] = (uint16_t) (min((float) torque_setpoint_array[1], max_allowed_torque(max_front_power / 2.0, (float) mc_status[1].get_speed())) * accu_lim_factor);
    torque_setpoint_array[2] = (uint16_t) (min((float) torque_setpoint_array[2], max_allowed_torque(max_rear_power / 2.0, (float) mc_status[2].get_speed())) * accu_lim_factor);
    torque_setpoint_array[3] = (uint16_t) (min((float) torque_setpoint_array[3], max_allowed_torque(max_rear_power / 2.0, (float) mc_status[3].get_speed())) * accu_lim_factor);
  }


  int16_t max_speed_regen = 0;
  for (int i = 0; i < sizeof(torque_setpoint_array); i++) {

    max_speed_regen = (max_speed_regen < mc_status[i].get_speed()) ? mc_status[i].get_speed() : max_speed_regen;

  }

  for (int i = 0; i < 4; i++) {
    torque_setpoint_array[i] = max(-2140, min(2140, torque_setpoint_array[i]));
  }

  for (int i = 0; i < 4; i++) {
    if (torque_setpoint_array[i] >= 0) {
      mc_setpoints_command[i].set_speed_setpoint(speed_setpoint_array[i]);
      mc_setpoints_command[i].set_pos_torque_limit(min(torque_setpoint_array[i] , 2140));
      mc_setpoints_command[i].set_neg_torque_limit(0);

    }
    else {

      float scale_down = 1;
      if (max_speed_regen < 770) {
        scale_down = 0;
      } else if (max_speed_regen > REGEN_OFF_START_THRESHOLD) {
        scale_down = 1;
      } else {
        scale_down = map(max_speed_regen, 770, REGEN_OFF_START_THRESHOLD, 0, 1);
      }
      mc_setpoints_command[i].set_speed_setpoint(0);
      mc_setpoints_command[i].set_pos_torque_limit(0);
      mc_setpoints_command[i].set_neg_torque_limit(max(((int16_t)(torque_setpoint_array[i]) * scale_down) , -2140) );

    }
  }

}

inline void read_all_adcs() {
  if (timer_read_all_adcs.check()) {

    prev_load_cell_readings[0] = mcu_load_cells.get_FL_load_cell();
    prev_load_cell_readings[1] = mcu_load_cells.get_FR_load_cell();
    prev_load_cell_readings[2] = mcu_load_cells.get_RL_load_cell();
    prev_load_cell_readings[3] = mcu_load_cells.get_RR_load_cell();

    uint16_t adc1_inputs[8];
    ADC1.read_all_channels(&adc1_inputs[0]);
    mcu_pedal_readings.set_accelerator_pedal_1(adc1_inputs[ADC_ACCEL_1_CHANNEL]);
    mcu_pedal_readings.set_accelerator_pedal_2(adc1_inputs[ADC_ACCEL_2_CHANNEL]);
    mcu_pedal_readings.set_brake_pedal_1(adc1_inputs[ADC_BRAKE_1_CHANNEL]);
    mcu_pedal_readings.set_brake_pedal_2(adc1_inputs[ADC_BRAKE_2_CHANNEL]);
    mcu_analog_readings.set_steering_2(adc1_inputs[ADC_STEERING_2_CHANNEL]);
    current_read = adc1_inputs[ADC_CURRENT_CHANNEL] - adc1_inputs[ADC_REFERENCE_CHANNEL];
    float current = ((((current_read / 819.0) / .1912) / 4.832) * 1000) / 6.67;
    if (current > 300) {
      current = 300;
    } else if (current < -300) {
      current = -300;
    }
    mcu_analog_readings.set_hall_effect_current((uint16_t)current * 100);



    mcu_load_cells.set_RL_load_cell((uint16_t)((adc1_inputs[ADC_RL_LOAD_CELL_CHANNEL]*LOAD_CELL3_SLOPE + LOAD_CELL3_OFFSET) * (1 - load_cell_alpha) + prev_load_cell_readings[2]*load_cell_alpha));

    mcu_status.set_brake_pedal_active(mcu_pedal_readings.get_brake_pedal_1() >= BRAKE_ACTIVE);
    digitalWrite(BRAKE_LIGHT_CTRL, mcu_status.get_brake_pedal_active());

    mcu_status.set_mech_brake_active(mcu_pedal_readings.get_brake_pedal_1() >= BRAKE_THRESHOLD_MECH_BRAKE_1); //define in driver_constraints.h (70%)

    uint16_t adc2_inputs[8];
    ADC2.read_all_channels(&adc2_inputs[0]);
    mcu_load_cells.set_RR_load_cell((uint16_t)((adc2_inputs[ADC_RR_LOAD_CELL_CHANNEL]*LOAD_CELL4_SLOPE + LOAD_CELL4_OFFSET) * (1 - load_cell_alpha) + prev_load_cell_readings[3]*load_cell_alpha));
    mcu_load_cells.set_FL_load_cell((uint16_t)((adc2_inputs[ADC_FL_LOAD_CELL_CHANNEL]*LOAD_CELL1_SLOPE + LOAD_CELL1_OFFSET) * (1 - load_cell_alpha) + prev_load_cell_readings[0]*load_cell_alpha));
    mcu_load_cells.set_FR_load_cell((uint16_t)((adc2_inputs[ADC_FR_LOAD_CELL_CHANNEL]*LOAD_CELL2_SLOPE + LOAD_CELL2_OFFSET) * (1 - load_cell_alpha) + prev_load_cell_readings[1]*load_cell_alpha));
    mcu_rear_potentiometers.set_pot4(adc2_inputs[SUS_POT_RL]);
    mcu_rear_potentiometers.set_pot6(adc2_inputs[SUS_POT_RR]);
    mcu_front_potentiometers.set_pot1(adc2_inputs[SUS_POT_FL]);

    uint16_t adc3_inputs[8];
    ADC3.read_all_channels(&adc3_inputs[0]);
    mcu_analog_readings.set_glv_battery_voltage(adc3_inputs[ADC_GLV_READ_CHANNEL]);
    mcu_front_potentiometers.set_pot3(adc3_inputs[SUS_POT_FR]);
  }
}

inline void read_steering_spi_values() {
  if (timer_steering_spi_read.check()) {
    mcu_analog_readings.set_steering_1(STEERING.read_steering());

  }
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
  if (error_list) {
    mcu_status.set_inverters_error(true);
  } else {
    mcu_status.set_inverters_error(false);
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

inline void set_all_inverters_inverter_enable(bool input) {
  for (uint8_t inv = 0; inv < 4; inv++) {
    mc_setpoints_command[inv].set_inverter_enable(input);
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
  if (timer_read_imu.check()) {
    double sinAngle = sin(VEHICLE_TILT_ANGLE_X);
    double cosAngle = cos(VEHICLE_TILT_ANGLE_X);
    double accel_x = IMU.regRead(X_ACCL_OUT) * 0.00245; // 0.00245 is the scale
    double accel_y = IMU.regRead(Y_ACCL_OUT) * 0.00245; // 0.00245 is the scale
    double accel_z = IMU.regRead(Z_ACCL_OUT) * 0.00245; // 0.00245 is the scale
    double x_gyro = IMU.regRead(X_GYRO_OUT) * 0.005; // 0.005 is the scale
    double y_gyro = IMU.regRead(Y_GYRO_OUT) * 0.005; // 0.005 is the scale
    double z_gyro = IMU.regRead(Z_GYRO_OUT) * 0.005; // 0.005 is the scale
    double long_accel = ((-accel_y) * cosAngle) + (accel_z * sinAngle);
    double lat_accel = accel_x;
    double vert_accel = -((accel_z * cosAngle) - (-accel_y * sinAngle));
    double pitch = (y_gyro * cosAngle) + (z_gyro * sinAngle);
    double roll = y_gyro;
    double yaw = (z_gyro * cosAngle) - (x_gyro * sinAngle);
    imu_accelerometer.set_lat_accel((int16_t)(lat_accel * 1000)); // * 0.00245); // 0.00245 is the scale
    imu_accelerometer.set_long_accel((int16_t)(long_accel * 1000)); // * 0.00245); // 0.00245 is the scale
    imu_accelerometer.set_vert_accel((int16_t)(vert_accel * 1000)); // * 0.00245); // 0.00245 is the scale
    // question about yaw, pitch and roll rates?
    imu_gyroscope.set_pitch((int16_t)(pitch * 100)); // * 0.005); // 0.005 is the scale,
    imu_gyroscope.set_yaw((int16_t)(yaw * 100 )); // * 0.005);  // 0.005 is the scale
    imu_gyroscope.set_roll((int16_t)(roll * 100)); // * 0.005); // 0.005 is the scale
  }
}

inline void send_CAN_imu_accelerometer() {
  if (timer_CAN_imu_accelerometer_send.check()) {
    imu_accelerometer.write(msg.buf);
    msg.id = ID_IMU_ACCELEROMETER;
    msg.len = sizeof(imu_accelerometer);
    TELEM_CAN.write(msg);
  }
}

inline void send_CAN_imu_gyroscope() {
  if (timer_CAN_imu_gyroscope_send.check()) {
    imu_gyroscope.write(msg.buf);
    msg.id = ID_IMU_GYROSCOPE;
    msg.len = sizeof(imu_gyroscope);
    TELEM_CAN.write(msg);
  }
}

inline void calibrate_imu_velocity(double calibrate_to) {
  imu_velocity = calibrate_to;
}

inline void pitch_angle_calibration() {
  //  double z_accl_sum = 0.0;
  //  int ctr = 0;
  //  time_t start_time, current_time;
  //  double elapsed_time;
  //  start_time = time(NULL);
  //  // Serial.println("Calibration Starts Now"); FOR DEBUGING PURPOSES
  //  while (1) {
  //    delay(50);
  //    z_accl_sum += ((IMU.regRead(Z_ACCL_OUT) * 0.00245));
  //    ctr++;
  //    current_time = time(NULL);
  //    elapsed_time = difftime(current_time, start_time);
  //    if (elapsed_time >= 5) break; // Code runs for 5 seconds. Change for desired duration.
  //  }
  //  // Serial.println("Calibration Has Ended");
  //  double avg_z_accl = z_accl_sum / ctr;
  //  pitch_calibration_angle = std::acos(avg_z_accl / ACCL_DUE_TO_GRAVITY);
}

inline void calculate_pedal_implausibilities() {
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
  else if (mcu_pedal_readings.get_brake_pedal_2() > MIN_BRAKE_PEDAL_2 || mcu_pedal_readings.get_brake_pedal_2() < MAX_BRAKE_PEDAL_2) { //negative slope for brake 2
    mcu_status.set_no_brake_implausability(false);
  } else if (fabs((mcu_pedal_readings.get_brake_pedal_1() - START_BRAKE_PEDAL_1) / (END_BRAKE_PEDAL_1 - START_BRAKE_PEDAL_1) -
                  (START_BRAKE_PEDAL_2 - mcu_pedal_readings.get_brake_pedal_2()) / (START_BRAKE_PEDAL_2 - END_BRAKE_PEDAL_2)) > 0.25) {
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
    && mcu_status.get_mech_brake_active()
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
  if (mcu_status.get_no_accel_implausability() && mcu_status.get_no_brake_implausability() && mcu_status.get_no_accel_brake_implausability()) {
    pedal_implausability_duration = 0;
  }
}

inline float max_allowed_torque(float maxwatts, float rpm) {
  float angularspeed = (abs(rpm) + 1) / 60 * 2 * 3.1415;
  float maxnm = min(maxwatts / angularspeed, 20);
  return maxnm / 9.8 * 1000;
}
