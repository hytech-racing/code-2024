#include "CAN_Handler.h"

CAN_Handler::CAN_Handler()
{
    ht_data = HT_Data::getInstance();
    inverter_control = Inverter_Control::getInstance();
    initCAN();
}

void CAN_Handler::initCAN()
{
    INV_CAN.begin();
    INV_CAN.setBaudRate(TELEM_BAUD_RATE);
    TELEM_CAN.begin();
    TELEM_CAN.setBaudRate(INVERTER_BAUD_RATE);

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

    switch (rx_msg.id)
    {
    case ID_MC1_STATUS:
         inverter_control->get_inverter(0)->mc_status.load(rx_msg.buf);
        break;
    case ID_MC2_STATUS:
        inverter_control->get_inverter(1)->mc_status.load(rx_msg.buf);
        break;
    case ID_MC1_TEMPS:
        inverter_control->get_inverter(0)->mc_temp.load(rx_msg.buf);
        break;
    case ID_MC2_TEMPS:
        inverter_control->get_inverter(1)->mc_temp.load(rx_msg.buf);
        break;
    case ID_MC1_ENERGY:
        inverter_control->get_inverter(0)->mc_energy.load(rx_msg.buf);
        break;
    case ID_MC2_ENERGY:
        inverter_control->get_inverter(1)->mc_energy.load(rx_msg.buf);
        break;
    case ID_MC3_STATUS:
        inverter_control->get_inverter(2)->mc_status.load(rx_msg.buf);
        break;
    case ID_MC4_STATUS:
        inverter_control->get_inverter(3)->mc_status.load(rx_msg.buf);
        break;
    case ID_MC3_TEMPS:
        inverter_control->get_inverter(2)->mc_temp.load(rx_msg.buf);
        break;
    case ID_MC4_TEMPS:
       inverter_control->get_inverter(3)->mc_temp.load(rx_msg.buf);
        break;
    case ID_MC3_ENERGY:
        inverter_control->get_inverter(2)->mc_energy.load(rx_msg.buf);
        break;
    case ID_MC4_ENERGY:
        inverter_control->get_inverter(3)->mc_energy.load(rx_msg.buf);
        break;
    }
}
void CAN_Handler::parse_telem_can_message(const CAN_message_t &RX_msg)
{
    CAN_message_t rx_msg = RX_msg;
    switch (rx_msg.id)
    {
    case ID_BMS_TEMPERATURES:
        ht_data->bms_temperatures.load(rx_msg.buf);
        ht_data->filtered_max_cell_temp *cell_temp_alpha + (1.0 - cell_temp_alpha) * (ht_data->bms_temperatures.get_high_temperature() / 100.0);
        break;
    case ID_BMS_VOLTAGES:
        ht_data->bms_voltages.load(rx_msg.buf);
        if (ht_data->bms_voltages.get_low() < PACK_CHARGE_CRIT_LOWEST_CELL_THRESHOLD || ht_data->bms_voltages.get_total() < PACK_CHARGE_CRIT_TOTAL_THRESHOLD)
        {
            ht_data->mcu_status.set_pack_charge_critical(true);
        }
        else
            ht_data->mcu_status.set_pack_charge_critical(false);
        ht_data->filtered_min_cell_voltage = ht_data->filtered_min_cell_voltage * cell_voltage_alpha + (1.0 - cell_voltage_alpha) * (ht_data->bms_voltages.get_low() / 10000.0);
        break;
    case ID_BMS_COULOMB_COUNTS:
        ht_data->bms_coulomb_counts.load(rx_msg.buf);
        break;
    case ID_BMS_STATUS:
        ht_data->bms_status.load(rx_msg.buf);
        // BMS heartbeat timer
        timer_bms_heartbeat.reset();
        timer_bms_heartbeat.interval(BMS_HEARTBEAT_TIMEOUT);
        break;
    case ID_DASHBOARD_STATUS:
        ht_data->dashboard_status.load(rx_msg.buf);
        /* process dashboard buttons */
        if (ht_data->dashboard_status.get_torque_mode_btn())
        {
            switch (ht_data->mcu_status.get_torque_mode())
            {
            case 1:
                ht_data->mcu_status.set_max_torque(TORQUE_2);
                ht_data->mcu_status.set_torque_mode(2);
                break;
            case 2:
                ht_data->mcu_status.set_max_torque(TORQUE_3);
                ht_data->mcu_status.set_torque_mode(3);
                break;
            case 3:
                ht_data->mcu_status.set_max_torque(TORQUE_1);
                ht_data->mcu_status.set_torque_mode(1);
                break;
            }
        }
        if (ht_data->dashboard_status.get_launch_ctrl_btn())
        {
            ht_data->mcu_status.toggle_launch_ctrl_active();
        }
        if (ht_data->dashboard_status.get_mc_cycle_btn())
        {
            inverter_control->set_inverter_restart(true); // inverter data to write to
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
}

void CAN_Handler::send_CAN_inverter_setpoints()
{
    if (timer_CAN_inverter_setpoints_send.check())
    {
        for (int i = 0; i < NUM_INVERTERS; i++)
        {
            inverter_control->get_inverter(i)->mc_setpoints_command.write(msg.buf);
            switch (i + 1)
            {
            case 1:
                msg.id = ID_MC1_SETPOINTS_COMMAND;
                break;
            case 2:
                msg.id = ID_MC2_SETPOINTS_COMMAND;
                break;
            case 3:
                msg.id = ID_MC3_SETPOINTS_COMMAND;
                break;
            case 4:
                msg.id = ID_MC4_SETPOINTS_COMMAND;
                break;
            }

            msg.len = sizeof(inverter_control->get_inverter(i)->mc_setpoints_command);
            INV_CAN.write(msg);
            //need to write everything on CAN bus for TCU
            if(timer_CAN_inverter_telem_send.check()) TELEM_CAN.write(msg);
        }
    }
}
void CAN_Handler::send_CAN_mcu_status()
{
    if (timer_CAN_mcu_status_send.check())
    {
        // Send Main Control Unit status message
        ht_data->mcu_status.write(msg.buf);
        msg.id = ID_MCU_STATUS;
        msg.len = sizeof(ht_data->mcu_status);
        TELEM_CAN.write(msg);
    }
}
void CAN_Handler::send_CAN_mcu_pedal_readings()
{
    if (timer_CAN_mcu_pedal_readings_send.check())
    {
        ht_data->mcu_pedal_readings.write(msg.buf);
        msg.id = ID_MCU_PEDAL_READINGS;
        msg.len = sizeof(ht_data->mcu_pedal_readings);
        TELEM_CAN.write(msg);
    }
}

void CAN_Handler::send_CAN_mcu_load_cells()
{
    if (timer_CAN_mcu_load_cells_send.check())
    {
        ht_data->mcu_load_cells.write(msg.buf);
        msg.id = ID_MCU_LOAD_CELLS;
        msg.len = sizeof(ht_data->mcu_load_cells);
        TELEM_CAN.write(msg);
    }
}

void CAN_Handler::send_CAN_mcu_potentiometers()
{
    if (timer_CAN_mcu_potentiometers_send.check())
    {
        ht_data->mcu_potentiometers.write(msg.buf);
        msg.id = ID_MCU_POTS;
        msg.len = sizeof(ht_data->mcu_potentiometers);
        TELEM_CAN.write(msg);
    }
}

// might not be necessary (Calculations done on acu)
//  void CAN_Handler::send_CAN_bms_coulomb_counts() {
//    if (timer_CAN_coloumb_count_send.check()) {
//      ht_data->bms_coulomb_counts.write(msg.buf);
//      msg.id = ID_BMS_COULOMB_COUNTS;
//      msg.len = sizeof(ht_data->bms_coulomb_counts);
//      TELEM_CAN.write(msg);
//    }
//  }

void CAN_Handler::send_CAN_mcu_analog_readings()
{
    if (timer_CAN_mcu_analog_readings_send.check())
    {
        ht_data->mcu_analog_readings.write(msg.buf);
        msg.id = ID_MCU_ANALOG_READINGS;
        msg.len = sizeof(ht_data->mcu_analog_readings);
        TELEM_CAN.write(msg);
    }
}

void CAN_Handler::send_CAN_buzzer_data()
{
    ht_data->mcu_status.set_activate_buzzer(false);
    ht_data->mcu_status.write(msg.buf);
    msg.id = ID_MCU_STATUS;
    msg.len = sizeof(ht_data->mcu_status);
    TELEM_CAN.write(msg);
}