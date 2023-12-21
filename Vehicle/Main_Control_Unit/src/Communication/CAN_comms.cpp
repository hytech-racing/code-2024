/**
 * @file CAN_comms.cpp
 * @author Eric Galluzzi
 * @brief CAN handling
 * @version 0.1
 * @date 2023-12-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
//this wont work in IDE
#include "../SensorAquisition/HT_Data.h"
#include "Metro.h"
#include "../ErrorHandling/debug.h"
#include "../Timing.h"

#define INV_BAUD 500000
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> INV_CAN;
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> TELEM_CAN;
CAN_message_t msg;

/**
 * @brief initialize can bus
 * 
 * @param telemBaud 
 */
void initCAN(int telemBaud = 500000) {
    INV_CAN.begin();
    INV_CAN.setBaudRate(INV_BAUD);
    TELEM_CAN.begin();
    TELEM_CAN.setBaudRate(telemBaud);
    INV_CAN.enableMBInterrupts();
    TELEM_CAN.enableMBInterrupts();
    INV_CAN.onReceive(parse_inv_can_message);
    TELEM_CAN.onReceive(parse_telem_can_message);
    delay(500);
    Debug_println("CAN system and serial communication initialized");
}
inline void send_CAN_inverter_setpoints() {
  if (timer_CAN_inverter_setpoints_send.check()) {
    ht_data.mc_setpoints_command[0].write(msg.buf);
    msg.id = ID_MC1_SETPOINTS_COMMAND;
    msg.len = sizeof(ht_data.mc_setpoints_command[0]);
    INV_CAN.write(msg);

    ht_data.mc_setpoints_command[1].write(msg.buf);
    msg.id = ID_MC2_SETPOINTS_COMMAND;
    msg.len = sizeof(ht_data.mc_setpoints_command[1]);
    INV_CAN.write(msg);

    ht_data.mc_setpoints_command[2].write(msg.buf);
    msg.id = ID_MC3_SETPOINTS_COMMAND;
    msg.len = sizeof(ht_data.mc_setpoints_command[2]);
    INV_CAN.write(msg);

    ht_data.mc_setpoints_command[3].write(msg.buf);
    msg.id = ID_MC4_SETPOINTS_COMMAND;
    msg.len = sizeof(ht_data.mc_setpoints_command[3]);
    INV_CAN.write(msg);
  }
}
inline void send_CAN_mcu_status() {
  if (timer_CAN_mcu_status_send.check()) {
    // Send Main Control Unit status message
    ht_data.mcu_status.write(msg.buf);
    msg.id = ID_MCU_STATUS;
    msg.len = sizeof(ht_data.mcu_status);
    TELEM_CAN.write(msg);
  }
}
inline void send_CAN_mcu_pedal_readings() {
  if (timer_CAN_mcu_pedal_readings_send.check()) {
    ht_data.mcu_pedal_readings.write(msg.buf);
    msg.id = ID_MCU_PEDAL_READINGS;
    msg.len = sizeof(ht_data.mcu_pedal_readings);
    TELEM_CAN.write(msg);
  }
}

inline void send_CAN_mcu_load_cells() {
  if (timer_CAN_mcu_load_cells_send.check()) {
    ht_data.mcu_load_cells.write(msg.buf);
    msg.id = ID_MCU_LOAD_CELLS;
    msg.len = sizeof(ht_data.mcu_load_cells);
    TELEM_CAN.write(msg);
  }
}

inline void send_CAN_mcu_potentiometers() {
  if (timer_CAN_mcu_potentiometers_send.check()) {
    ht_data.mcu_potentiometers.write(msg.buf);
    msg.id = ID_MCU_POTS;
    msg.len = sizeof(ht_data.mcu_potentiometers);
    TELEM_CAN.write(msg);

  }
}

//might not be necessary (Calculations done on acu)
inline void send_CAN_bms_coulomb_counts() {
  if (timer_CAN_coloumb_count_send.check()) {
    ht_data.bms_coulomb_counts.write(msg.buf);
    msg.id = ID_BMS_COULOMB_COUNTS;
    msg.len = sizeof(ht_data.bms_coulomb_counts);
    TELEM_CAN.write(msg);
  }
}

inline void send_CAN_mcu_analog_readings() {
  if (timer_CAN_mcu_analog_readings_send.check()) {
    ht_data.mcu_analog_readings.write(msg.buf);
    msg.id = ID_MCU_ANALOG_READINGS;
    msg.len = sizeof(ht_data.mcu_analog_readings);
    TELEM_CAN.write(msg);
  }
}

void parse_telem_can_message(const CAN_message_t &RX_msg) {
  CAN_message_t rx_msg = RX_msg;
  switch (rx_msg.id) {
    case ID_BMS_TEMPERATURES:              
      ht_data.bms_temperatures.load(rx_msg.buf);
      filtered_max_cell_temp  = filtered_max_cell_temp * cell_temp_alpha + (1.0 - cell_temp_alpha) * (ht_data.bms_temperatures.get_high_temperature() / 100.0) ;              
      break;
    case ID_BMS_VOLTAGES:
      ht_data.bms_voltages.load(rx_msg.buf);
      if (ht_data.bms_voltages.get_low() < PACK_CHARGE_CRIT_LOWEST_CELL_THRESHOLD || ht_data.bms_voltages.get_total() < PACK_CHARGE_CRIT_TOTAL_THRESHOLD) {
        ht_data.mcu_status.set_pack_charge_critical(true);
      } else ht_data.mcu_status.set_pack_charge_critical(false);
      filtered_min_cell_voltage = filtered_min_cell_voltage * cell_voltage_alpha + (1.0 - cell_voltage_alpha) * (ht_data.bms_voltages.get_low() / 10000.0);
      break;
    case ID_BMS_COULOMB_COUNTS:            ht_data.bms_coulomb_counts.load(rx_msg.buf);            break;
    case ID_BMS_STATUS:
      ht_data.bms_status.load(rx_msg.buf);
      // BMS heartbeat timer
      timer_bms_heartbeat.reset();
      timer_bms_heartbeat.interval(BMS_HEARTBEAT_TIMEOUT);
      break;
    case ID_DASHBOARD_STATUS:
      ht_data.dashboard_status.load(rx_msg.buf);
      /* process dashboard buttons */
      if (ht_data.dashboard_status.get_torque_mode_btn()) {
        switch (ht_data.mcu_status.get_torque_mode()) {
          case 1:
            ht_data.mcu_status.set_max_torque(TORQUE_2);
            ht_data.mcu_status.set_torque_mode(2); break;
          case 2:
            ht_data.mcu_status.set_max_torque(TORQUE_3);
            ht_data.mcu_status.set_torque_mode(3); break;
          case 3:
            ht_data.mcu_status.set_max_torque(TORQUE_1);
            ht_data.mcu_status.set_torque_mode(1); break;
        }
      }
      if (ht_data.dashboard_status.get_launch_ctrl_btn()) {
        ht_data.mcu_status.toggle_launch_ctrl_active();
      }
      if (ht_data.dashboard_status.get_mc_cycle_btn()) {
        inverter_restart = true;
        timer_reset_inverter.reset();
      }
      // eliminate all action buttons to not process twice
      ht_data.dashboard_status.set_button_flags(0);
      break;
    case ID_SAB_CB
      ht_data.sab_corner_boards.load(rx.msg.buf);
      ht_data.prev_load_cell_readings[2] = ht_data.mcu_load_cells.get_RL_load_cell();
      ht_data.prev_load_cell_readings[3] = ht_data.mcu_load_cells.get_RR_load_cell();
      ht_data.mcu_load_cells.set_RL_load_cell(sab_corner_boards.get_RL_load_cell());
      ht_data.mcu_load_cells.set_RR_load_cell(sab_corner_boards.get_RR_load_cell());
      ht_data.mcu_potentiometers.set_pot3(sab_corner_boards.get_pot3());
      ht_data.mcu_potentiometers.set_pot4(sab_corner_boards.get_pot4());
  }
}

void parse_inv_message(const CAN_message_t &RX_msg) {
  CAN_message_t rx_msg = RX_msg;

  switch (rx_msg.id) {
    case ID_MC1_STATUS:       ht_data.mc_status[0].load(rx_msg.buf);    break;
    case ID_MC2_STATUS:       ht_data.mc_status[1].load(rx_msg.buf);    break;
    case ID_MC1_TEMPS:        ht_data.mc_temps[0].load(rx_msg.buf);    break;
    case ID_MC2_TEMPS:        ht_data.mc_temps[1].load(rx_msg.buf);    break;
    case ID_MC1_ENERGY:       ht_data.mc_energy[0].load(rx_msg.buf);    break;
    case ID_MC2_ENERGY:       ht_data.mc_energy[1].load(rx_msg.buf);    break;
    case ID_MC3_STATUS:       ht_data.mc_status[2].load(rx_msg.buf);    break;
    case ID_MC4_STATUS:       ht_data.mc_status[3].load(rx_msg.buf);    break;
    case ID_MC3_TEMPS:        ht_data.mc_temps[2].load(rx_msg.buf);    break;
    case ID_MC4_TEMPS:        ht_data.mc_temps[3].load(rx_msg.buf);    break;
    case ID_MC3_ENERGY:       ht_data.mc_energy[2].load(rx_msg.buf);    break;
    case ID_MC4_ENERGY:       ht_data.mc_energy[3].load(rx_msg.buf);    break;
  }
}


