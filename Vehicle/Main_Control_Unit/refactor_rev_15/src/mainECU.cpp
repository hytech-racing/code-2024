/**
 * @file mainECU.cpp
 * @author Eric Galluzzi
 * @brief Main ECU code
 * @version 0.1
 * @date 2023-12-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include "mainECU.h"
MainECU::MainECU() : sensor_aq(), can_handler(), torque_vectoring(), status_manager()
{
    ht_data = HT_Data::getInstance();
    inverter_control = Inverter_Control::getInstance();
    mcu_state = MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE;
    ht_data->mcu_status.set_state(mcu_state);
    Debug_begin(115200);
}

void MainECU::state_machine()
{
    switch (mcu_state)
    {
    case MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE:
    {
        Debug_println("TS NOT ACTIVE");
        if (inverter_control->check_TS_over_HV_threshold())
        {

            Debug_println("Setting state to TS Active from TS Not Active");
            set_state(MCU_STATE::TRACTIVE_SYSTEM_ACTIVE);
        }
        break;
    }

    case MCU_STATE::TRACTIVE_SYSTEM_ACTIVE:
    {
        set_state(inverter_control->check_TS_active(mcu_state));
        if (status_manager.ready_to_drive())
        {
            Debug_println("Setting state to Enabling Inverter");
            set_state(MCU_STATE::ENABLING_INVERTER);
        }
        break;
    }

    case MCU_STATE::ENABLING_INVERTER:
    {
        set_state(inverter_control->check_TS_active(mcu_state));
        if (timer_inverter_enable.check())
        {
            Debug_println("Setting state to TS Active from Enabling Inverter");
            set_state(MCU_STATE::TRACTIVE_SYSTEM_ACTIVE);
        }
        if (inverter_control->get_inverter_state() == INVERTER_STARTUP_STATE::ALL_INVERTERS_QUIT_INVERTER_ON)
        {
            Debug_println("Setting state to Waiting Ready to Drive Sound");
            set_state(MCU_STATE::WAITING_READY_TO_DRIVE_SOUND);
        }
        break;
    }

    case MCU_STATE::WAITING_READY_TO_DRIVE_SOUND:
    {
        set_state(inverter_control->check_TS_active(mcu_state));
        if (timer_ready_sound.check())
        {

            Debug_println("Setting state to Ready to Drive");

            set_state(MCU_STATE::READY_TO_DRIVE);
        }
        break;
    }

    case MCU_STATE::READY_TO_DRIVE:
    {
        set_state(inverter_control->check_TS_active(mcu_state));
        bool pedal_implausability = status_manager.valid_pedals();
        bool hv_signals = status_manager.valid_HV();
        if (pedal_implausability && hv_signals) ;//set_inverter_torques
        else if (hv_signals) ;//inverter_control->set_inverter_torques_regen_only();
        else {
            Debug_println("not calculating torque");
            Debug_printf("no brake implausibility: %d\n", ht_data->mcu_status.get_no_brake_implausability());
            Debug_printf("no accel implausibility: %d\n", ht_data->mcu_status.get_no_accel_implausability());
            Debug_printf("no accel brake implausibility: %d\n", ht_data->mcu_status.get_no_accel_brake_implausability());
            Debug_printf("software is ok: %d\n", ht_data->mcu_status.get_software_is_ok());
            Debug_printf("get bms ok high: %d\n", ht_data->mcu_status.get_bms_ok_high());
            Debug_printf("get imd ok high: %d\n", ht_data->mcu_status.get_imd_ok_high());
            //set_all_inverters_no_torque();
        }
        break;
        // state management
    }
    }
}

void MainECU::set_state(MCU_STATE new_state)
{
    if (mcu_state == new_state)
    {
        return;
    }
    // exit logic
    switch (mcu_state)
    {
    case MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE:
    {
        break;
    }

    case MCU_STATE::TRACTIVE_SYSTEM_ACTIVE:
    {
        break;
    }

    case MCU_STATE::ENABLING_INVERTER:
    {
        timer_inverter_enable.reset();
        break;
    }

    case MCU_STATE::WAITING_READY_TO_DRIVE_SOUND:
    {
        status_manager.activate_buzzer(false);
        can_handler.send_CAN_buzzer_data();
        break;
    }

    case MCU_STATE::READY_TO_DRIVE:
    {
        inverter_control->set_inv_state(INVERTER_STARTUP_STATE::ALL_INVERTERS_DISABLED);
        torque_vectoring.set_inverter_torques(ht_data->dashboard_status.get_dial_state());
        break;
    }
    }

    mcu_state = new_state;
    ht_data->mcu_status.set_state(mcu_state);

    // entry logic
    switch (mcu_state)
    {
    case MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE:
    {
        break;
    }

    case MCU_STATE::TRACTIVE_SYSTEM_ACTIVE:
    {
        inverter_control->set_inv_state(INVERTER_STARTUP_STATE::WAIT_SYSTEM_READY);
        break;
    }

    case MCU_STATE::ENABLING_INVERTER:
    {
        Debug_println("MCU Sent enable command");
        timer_inverter_enable.reset();
        break;
    }

    case MCU_STATE::WAITING_READY_TO_DRIVE_SOUND:
    {
        status_manager.activate_buzzer(false);
        can_handler.send_CAN_buzzer_data();
        timer_ready_sound.reset();
        Debug_println("RTDS enabled");

        break;
    }

    case MCU_STATE::READY_TO_DRIVE:
    {
        Debug_println("Ready to drive");
        break;
    }
    }
}
void MainECU::loop() {
    sensor_aq.sampleADCs();
    sensor_aq.sampleSteeringTop();
    can_handler.CAN_poll();
    can_handler.CAN_write();
    status_manager.status_poll();
    state_machine();
    inverter_control->inv_state_machine();
    if (timer_debug.check())
  {
    inverter_control->debugInverters();
    torque_vectoring.debugPedals();
    torque_vectoring.debugLoadCells();
    status_manager.debugPOTS();
    torque_vectoring.debugTorque();
    
  }
    
}