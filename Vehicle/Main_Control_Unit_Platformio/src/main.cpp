#include <Arduino.h>
#include <HyTech.h>
#include <Task_Container.h>

bool check_TS_over_HC_threshold() {
    return std::all_of(mc.energy.data.iter().begin(), mc.energy.data.iter().end(), [](auto &i) { return i.get().dc_bus_voltage >= MIN_HV_VOLTAGE; });
}

bool enable_inverters() {
    if (!std::all_of(mc.status.get_system_ready().begin(), mc.status.get_system_ready().end(), [](bool b) { return b; })) {
        return false;
    }
    mc.setpoint.set_hv_enable(true);

    if (!std::all_of(mc.status.get_quit_dc_on().begin(), mc.status.get_quit_dc_on().end(), [](bool b) { return b; })) {
        return false;
    }
    mc.setpoint.set_pos_torque_limit(0);
    mc.setpoint.set_neg_torque_limit(0);
    mc.setpoint.set_driver_enable(true);
    mc.setpoint.set_inverter_enable(true);

    if (!std::all_of(mc.status.get_quit_inverter_on().begin(), mc.status.get_quit_inverter_on().end(), [](bool b) { return b; })) {
        return false;
    }

    return true;
}

MCU_Status::MCU_STATE state_machine_transition(MCU_Status::MCU_STATE state) {
    switch (state) {
        case MCU_Status::MCU_STATE::STARTUP:
            mc.setpoint.set_speed_setpoint(0);
            mc.setpoint.set_pos_torque_limit(0);
            mc.setpoint.set_neg_torque_limit(0);
            delay(500);
            state = MCU_Status::MCU_STATE::TRACTIVE_SYSTEM_INACTIVE;
            break;

        case MCU_Status::MCU_STATE::TRACTIVE_SYSTEM_INACTIVE:
            if (check_TS_over_HC_threshold()) {
                state = MCU_Status::MCU_STATE::TRACTIVE_SYSTEM_ACTIVE;
            }
            break;

        case MCU_Status::MCU_STATE::TRACTIVE_SYSTEM_ACTIVE:
            state = MCU_Status::MCU_STATE::ENABLING_INVERTER;
            break;

        case MCU_Status::MCU_STATE::ENABLING_INVERTER:
            if (enable_inverters()) {
                state = MCU_Status::MCU_STATE::WAITING_READY_TO_DRIVE_SOUND;
            }
            break;

        case MCU_Status::MCU_STATE::WAITING_READY_TO_DRIVE_SOUND:
            state = MCU_Status::MCU_STATE::READY_TO_DRIVE;
            break;

        case MCU_Status::MCU_STATE::READY_TO_DRIVE:
            break;
    }
    return state;
}

static MCU_Status::MCU_STATE mcu_state = MCU_Status::MCU_STATE::STARTUP;

void watchdog() {
    static bool state = false;
    digitalWrite(WATCHDOG_INPUT, state);
    state = !state;
}

void setup() {
    taskContainer.emplace("Watchdog", watchdog, 500);
    mcu_state = state_machine_transition(mcu_state);
}

void loop() {
    mcu_state = state_machine_transition(mcu_state);
}