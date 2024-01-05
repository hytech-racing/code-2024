#include <Arduino.h>

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

void setup();
void loop();
void send_CAN_inverter_setpoints();
void send_CAN_mcu_status();
void send_CAN_mcu_pedal_readings();
void send_CAN_mcu_load_cells();
void send_CAN_mcu_potentiometers();
void send_CAN_bms_coulomb_counts();
void send_CAN_mcu_analog_readings();
void state_machine();
bool check_TS_over_HV_threshold();
void check_TS_active();
void software_shutdown();
void parse_telem_can_message(const CAN_message_t &RX_msg);
void parse_front_inv_can_message(const CAN_message_t &RX_msg);
void parse_rear_inv_can_message(const CAN_message_t &RX_msg);
void power_off_inverter();
void set_state(MCU_STATE state);
float float_map(float, float, float, float, float);
void set_inverter_torques_regen_only();
void set_inverter_torques();
void read_all_adcs();
void read_steering_spi_values();
bool check_all_inverters_system_ready();
bool check_all_inverters_quit_dc_on();
bool check_all_inverters_quit_inverter_on();
uint8_t check_all_inverters_error();
void set_all_inverters_no_torque();
void set_all_inverters_torque_limit(int);
void set_all_inverters_speed_setpoint(int);
void set_all_inverters_disabled();
void set_all_inverters_dc_on(bool);
void set_all_inverters_driver_enable(bool);
void set_all_inverters_inverter_enable(bool);
void reset_inverters();
void read_status_values();
void read_imu();
void send_CAN_imu_accelerometer();
void send_CAN_imu_gyroscope();
void calibrate_imu_velocity(double);
void pitch_angle_calibration();
void calculate_pedal_implausibilities();
float max_allowed_torque(float, float);
