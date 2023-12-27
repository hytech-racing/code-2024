
// /**
//  * @file Main_Control_Unit.ino
//  * @author Liwei Sun, Mikhail Allen, Eric Galluzzi
//  * @brief Teensy 4.1 Main Control Unit code Rev15
//  * @version 0.1
//  * @date 2023-12-21
//  *
//  * @copyright Copyright (c) 2023
//  *
//  *
//  */

// #pragma once

// #include <stdint.h>
// #include <FlexCAN_T4.h>
// #include <HyTech_CAN.h>
// #define _USE_MATH_DEFINES
// #include <cmath>
// #include <time.h>
// #include <deque>
// #include <Mcp320x.h>
// #include "ADC_SPI.h"
// #include "STEERING_SPI.h"
// #include "kinetis_flexcan.h"
// #include "timer_utils.h"
// #include "debugging_utils.h"
// #include "HT_Data.h"
// #include "MCU_rev15_dfs.h"
// #include "driver_constants.h"
// #include "drivers.h"

// // bool imd_faulting = false;
// bool inverter_restart = false; // True when restarting the inverter, local variable for inverter
// INVERTER_STARTUP_STATE inverter_startup_state = INVERTER_STARTUP_STATE::WAIT_SYSTEM_READY;

// // MCP3208 SPI
// // RS422 Steering

// FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> INV_CAN;
// FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> TELEM_CAN;
// CAN_message_t msg;

// // coloumb counts
// uint32_t total_discharge;
// unsigned long previous_data_time;

// int16_t torque_setpoint_array[4];
// int16_t speed_setpoint_array[4] = {0, 0, 0, 0};

// uint16_t prev_load_cell_readings[4] = {0, 0, 0, 0};
// float load_cell_alpha = 0.95;

// float filtered_min_cell_voltage = 3.5;
// float cell_voltage_alpha = 0.8;

// float filtered_max_cell_temp = 40.0;
// float cell_temp_alpha = 0.8;

// // shunt current and mag current
// uint16_t shunt_current_read = 0;
// uint16_t shunt_reference_read = 0;
// uint16_t hall_current_read = 0;
// uint16_t hall_reference_read = 0;

// float max_front_power = 0.0;
// float max_rear_power = 0.0;

// enum launch_states
// {
//   launch_not_ready,
//   launch_ready,
//   launching
// };
// launch_states launch_state = launch_not_ready;
// int16_t launch_speed_target = 0;
// elapsedMillis time_since_launch;
// const uint16_t LAUNCH_READY_ACCEL_THRESHOLD = 100;
// const uint16_t LAUNCH_READY_BRAKE_THRESHOLD = 300;
// const int16_t LAUNCH_READY_SPEED_THRESHOLD = 500;
// const uint16_t LAUNCH_GO_THRESHOLD = 900;
// const uint16_t LAUNCH_STOP_THRESHOLD = 600;
// float launch_rate_target = 0.0;

// inline void set_all_inverters_dc_on(bool input);
// inline void calculate_pedal_implausibilities();

// inline float max_allowed_torque(float maxwatts, float rpm);
// inline void set_inverter_torques_regen_only();
// inline void set_all_inverters_disabled();
// inline void set_all_inverters_no_torque();
// bool check_all_inverters_quit_dc_on();
// inline void check_TS_active();
// bool check_all_inverters_system_ready();

// inline void set_inverter_torques();
// uint8_t check_all_inverters_error();
// bool check_all_inverters_quit_inverter_on();
// inline void set_all_inverters_inverter_enable(bool input);
// inline void set_all_inverters_no_torque();

// inline void set_all_inverters_driver_enable(bool input);

// bool check_TS_over_HV_threshold();

// void set_state(MCU_STATE new_state);

// // comms cpp

// // SM cpp

// /* Shared state functinality */




// // comms.cpp
// /* Parse incoming CAN messages */


// // FIXME

// /* Handle changes in state */



// // write me (sensors/comms)
// inline void read_steering_rs422();

// // write me
// inline void get_thermistor_temperature();

// // monitoring cpp
// /* Read shutdown system values */

// // math used by torque vectoring

// void setup()
// {
//   // no torque can be provided on startup

//   mcu_status.set_max_torque(0);
//   mcu_status.set_torque_mode(0);
//   mcu_status.set_software_is_ok(true);

//   set_all_inverters_disabled();

//   pinMode(BRAKE_LIGHT_CTRL, OUTPUT);

//   // change to input if comparator is PUSH PULL

//   pinMode(WATCHDOG_INPUT, OUTPUT);
//   // the initial state of the watchdog is high
//   // this is reflected in the static watchdog_state
//   // starting high
//   digitalWrite(WATCHDOG_INPUT, HIGH);
//   pinMode(SOFTWARE_OK, OUTPUT);
//   digitalWrite(SOFTWARE_OK, HIGH);

// #if DEBUG
//   Debug_begin(115200);
// #endif


//   // these are false by default
//   mcu_status.set_bms_ok_high(false);
//   mcu_status.set_imd_ok_high(false);

//   // present action for 5s
//   delay(5000);

//   set_state(MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE);
//   mcu_status.set_max_torque(TORQUE_3);
//   mcu_status.set_torque_mode(3);

//   /* Set up total discharge readings */
//   // setup_total_discharge();
// }

// void loop()
// {
//   INV_CAN.events();
//   TELEM_CAN.events();

//   read_all_adcs();
//   // read_steering_spi_values();
//   read_status_values();

//   send_CAN_mcu_status();
//   send_CAN_mcu_pedal_readings();
//   send_CAN_mcu_load_cells();
//   send_CAN_mcu_potentiometers();
//   send_CAN_mcu_analog_readings();
//   send_CAN_inverter_setpoints();

//   //  /* Finish restarting the inverter when timer expires */
//   check_all_inverters_error();
//   reset_inverters();
//   //
//   //  /* handle state functionality */
//   state_machine();
//   software_shutdown();
//   brake_outputs();

//   if (timer_debug.check())
//   {
//     Debug_println("ERROR");
//     Debug_println(check_all_inverters_error());
//     Debug_println(mc_energy[0].get_dc_bus_voltage());
//     Debug_println(mc_temps[0].get_diagnostic_number());
//     Debug_println(mc_temps[1].get_diagnostic_number());
//     Debug_println(mc_temps[2].get_diagnostic_number());
//     Debug_println(mc_temps[3].get_diagnostic_number());
//     Debug_println();
//     Debug_println(mcu_pedal_readings.get_accelerator_pedal_1());
//     Debug_println(mcu_pedal_readings.get_accelerator_pedal_2());
//     Debug_println(mcu_pedal_readings.get_brake_pedal_1());
//     Debug_println(mcu_pedal_readings.get_brake_pedal_2());
//     // calculate_pedal_implausibilities();
//     //    Debug_println(mcu_status.get_no_accel_implausability());
//     //    Debug_println(mcu_status.get_no_brake_implausability());
//     //    Debug_println(mcu_status.get_no_accel_brake_implausability());
//     //    int brake1 = map(round(mcu_pedal_readings.get_brake_pedal_1()), START_BRAKE_PEDAL_1, END_BRAKE_PEDAL_1, 0, 2140);
//     //    int brake2 = map(round(mcu_pedal_readings.get_brake_pedal_2()), START_BRAKE_PEDAL_2, END_BRAKE_PEDAL_2, 0, 2140);
//     //    Debug_println(brake1);
//     //    Debug_println(brake2);
//     Debug_println();
//     Debug_println("LOAD CELLS");
//     Debug_println(mcu_load_cells.get_FL_load_cell());
//     Debug_println(mcu_load_cells.get_FR_load_cell());
//     Debug_println(mcu_load_cells.get_RL_load_cell());
//     Debug_println(mcu_load_cells.get_RR_load_cell());
//     Debug_println("SUS POTS");
//     Debug_println(mcu_potentiometers.get_pot1());
//     Debug_println(mcu_potentiometers.get_pot2());
//     Debug_println(mcu_potentiometers.get_pot3());
//     Debug_println(mcu_potentiometers.get_pot4());

//     Debug_println(torque_setpoint_array[0]);
//     Debug_println(torque_setpoint_array[1]);
//     Debug_println(torque_setpoint_array[2]);
//     Debug_println(torque_setpoint_array[3]);
//     Debug_println("MOTOR TEMPS");
//     Debug_println(mc_temps[0].get_motor_temp());
//     Debug_println(mc_temps[1].get_motor_temp());
//     Debug_println(mc_temps[2].get_motor_temp());
//     Debug_println(mc_temps[3].get_motor_temp());
//     Debug_println(mc_temps[3].get_igbt_temp());
//     Debug_println("dial");
//     Debug_println(dashboard_status.get_dial_state());
//   }
// }
#pragma once
#include "mainECU.h"
void setup() {

}

void loop() {
  
}