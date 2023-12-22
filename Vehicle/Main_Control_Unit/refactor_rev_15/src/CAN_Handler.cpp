#include "CAN_Handler.h"

CAN_Handler::CAN_Handler(HT_Data* _ht_data, Inverter_Control* _inverter_control) {
   ht_data = _ht_data;
   inverter_control = _inverter_control;
   initCAN();
}

void CAN_Handler::initCAN() {
  INV_CAN.begin();
  INV_CAN.setBaudRate(500000);
  TELEM_CAN.begin();
  TELEM_CAN.setBaudRate(500000);
  
  INV_CAN.enableMBInterrupts();
  TELEM_CAN.enableMBInterrupts();
  INV_CAN.onReceive(parse_inv_can_message);
  TELEM_CAN.onReceive(parse_telem_can_message);
  delay(500);
  Debug_println("CAN system and serial communication initialized");
}

void CAN_Handler::parse_inv_can_message(const CAN_message_t &RX_msg)
{
  CAN_message_t rx_msg = RX_msg;

  switch (rx_msg.id) {
    case ID_MC1_STATUS:       ht_data->mc_status[0].load(rx_msg.buf);    break;
    case ID_MC2_STATUS:       ht_data->mc_status[1].load(rx_msg.buf);    break;
    case ID_MC1_TEMPS:        ht_data->mc_temps[0].load(rx_msg.buf);    break;
    case ID_MC2_TEMPS:        ht_data->mc_temps[1].load(rx_msg.buf);    break;
    case ID_MC1_ENERGY:       ht_data->mc_energy[0].load(rx_msg.buf);    break;
    case ID_MC2_ENERGY:       ht_data->mc_energy[1].load(rx_msg.buf);    break;
    case ID_MC3_STATUS:       ht_data->mc_status[2].load(rx_msg.buf);    break;
    case ID_MC4_STATUS:       ht_data->mc_status[3].load(rx_msg.buf);    break;
    case ID_MC3_TEMPS:        ht_data->mc_temps[2].load(rx_msg.buf);    break;
    case ID_MC4_TEMPS:        ht_data->mc_temps[3].load(rx_msg.buf);    break;
    case ID_MC3_ENERGY:       ht_data->mc_energy[2].load(rx_msg.buf);    break;
    case ID_MC4_ENERGY:       ht_data->mc_energy[3].load(rx_msg.buf);    break;
  }
}
void CAN_Handler::parse_telem_can_message(const CAN_message_t &RX_msg)
{
  CAN_message_t rx_msg = RX_msg;
  switch (rx_msg.id) {
    case ID_BMS_TEMPERATURES:              
      ht_data->bms_temperatures.load(rx_msg.buf);
      ht_data->filtered_max_cell_temp * cell_temp_alpha + (1.0 - cell_temp_alpha) * (ht_data->bms_temperatures.get_high_temperature() / 100.0) ;              
      break;
    case ID_BMS_VOLTAGES:
      ht_data->bms_voltages.load(rx_msg.buf);
      if (ht_data->bms_voltages.get_low() < PACK_CHARGE_CRIT_LOWEST_CELL_THRESHOLD || ht_data->bms_voltages.get_total() < PACK_CHARGE_CRIT_TOTAL_THRESHOLD) {
        ht_data->mcu_status.set_pack_charge_critical(true);
      } else ht_data->mcu_status.set_pack_charge_critical(false);
      ht_data->filtered_min_cell_voltage = ht_data->filtered_min_cell_voltage * cell_voltage_alpha + (1.0 - cell_voltage_alpha) * (ht_data->bms_voltages.get_low() / 10000.0);
      break;
    case ID_BMS_COULOMB_COUNTS:            ht_data->bms_coulomb_counts.load(rx_msg.buf);            break;
    case ID_BMS_STATUS:
      ht_data->bms_status.load(rx_msg.buf);
      // BMS heartbeat timer
      timer_bms_heartbeat.reset();
      timer_bms_heartbeat.interval(BMS_HEARTBEAT_TIMEOUT);
      break;
    case ID_DASHBOARD_STATUS:
      ht_data->dashboard_status.load(rx_msg.buf);
      /* process dashboard buttons */
      if (ht_data->dashboard_status.get_torque_mode_btn()) {
        switch (ht_data->mcu_status.get_torque_mode()) {
          case 1:
            ht_data->mcu_status.set_max_torque(TORQUE_2);
            ht_data->mcu_status.set_torque_mode(2); break;
          case 2:
            ht_data->mcu_status.set_max_torque(TORQUE_3);
            ht_data->mcu_status.set_torque_mode(3); break;
          case 3:
            ht_data->mcu_status.set_max_torque(TORQUE_1);
            ht_data->mcu_status.set_torque_mode(1); break;
        }
      }
      if (ht_data->dashboard_status.get_launch_ctrl_btn()) {
        ht_data->mcu_status.toggle_launch_ctrl_active();
      }
      if (ht_data->dashboard_status.get_mc_cycle_btn()) {
        inverter_restart = true; //inverter data to write to
        timer_reset_inverter.reset();
      }
      // eliminate all action buttons to not process twice
      ht_data->dashboard_status.set_button_flags(0);
      break;
    case ID_SAB_CB:
      ht_data->sab_cb.load(rx_msg.buf);
      ht_data->prev_load_cell_readings[2] = ht_data->mcu_load_cells.get_RL_load_cell();
      ht_data->prev_load_cell_readings[3] = ht_data->mcu_load_cells.get_RR_load_cell();
      ht_data->mcu_load_cells.set_RL_load_cell(ht_data->sab_cb.get_RL_load_cell());
      ht_data->mcu_load_cells.set_RR_load_cell(ht_data->sab_cb.get_RR_load_cell());
      ht_data->mcu_potentiometers.set_pot3(ht_data->sab_cb.get_pot3());
      ht_data->mcu_potentiometers.set_pot4(ht_data->sab_cb.get_pot4());

}
