#include "DashboardInterface.h"

void DashboardInterface::read(const CAN_message_t &can_msg)
{

    DASHBOARD_STATE_t dash_state;
    Unpack_DASHBOARD_STATE_hytech(&dash_state, can_msg.buf, can_msg.len);

    _data.dial_mode = static_cast<DialMode_e>(dash_state.dial_state);
    
    _data.ssok = dash_state.ssok_above_threshold;
    _data.shutdown = dash_state.shutdown_h_above_threshold;

    _data.button.start = dash_state.start_button;
    _data.button.mark = dash_state.mark_button;
    _data.button.mode = dash_state.mode_button;
    _data.button.mc_cycle = dash_state.motor_controller_cycle_button;
    _data.button.launch_ctrl = dash_state.launch_ctrl_button;
    _data.button.torque_mode = dash_state.torque_mode_button;
    _data.button.led_dimmer = dash_state.led_dimmer_button;
    _data.button.left_shifter = dash_state.left_shifter_button;
    _data.button.right_shifter = dash_state.right_shifter_button;

    _data.buzzer_state = dash_state.drive_buzzer;

}

CAN_message_t DashboardInterface::write()
{   

    DASHBOARD_MCU_STATE_t dash_mcu_state;
    dash_mcu_state.drive_buzzer = _data.buzzer_cmd;

    // TODO: use logic as to not write data for LEDs that have not changed
    dash_mcu_state.bots_led = _data.LED[static_cast<int>(DashLED_e::BOTS_LED)];
    dash_mcu_state.launch_control_led = _data.LED[static_cast<int>(DashLED_e::LAUNCH_CONTROL_LED)];
    dash_mcu_state.mode_led = _data.LED[static_cast<int>(DashLED_e::MODE_LED)];
    dash_mcu_state.mechanical_brake_led = _data.LED[static_cast<int>(DashLED_e::MECH_BRAKE_LED)];
    dash_mcu_state.cockpit_brb_led = _data.LED[static_cast<int>(DashLED_e::COCKPIT_BRB_LED)];
    dash_mcu_state.inertia_status_led = _data.LED[static_cast<int>(DashLED_e::INERTIA_LED)];
    dash_mcu_state.start_status_led = _data.LED[static_cast<int>(DashLED_e::START_LED)];
    dash_mcu_state.motor_controller_error_led = _data.LED[static_cast<int>(DashLED_e::MC_ERROR_LED)];
    dash_mcu_state.imd_led = _data.LED[static_cast<int>(DashLED_e::IMD_LED)];
    dash_mcu_state.ams_led = _data.LED[static_cast<int>(DashLED_e::AMS_LED)];

    dash_mcu_state.glv_led = _data.LED[static_cast<int>(DashLED_e::GLV_LED)];
    dash_mcu_state.pack_charge_led = _data.LED[static_cast<int>(DashLED_e::CRIT_CHARGE_LED)];
    
    CAN_message_t can_msg;
    auto id = Pack_DASHBOARD_MCU_STATE_hytech(&dash_mcu_state, can_msg.buf, &can_msg.len, (uint8_t*) &can_msg.flags.extended);
    can_msg.id = id;
    // this circular buffer implementation requires that you push your data in a array buffer
    // all this does is put the msg into a uint8_t buffer and pushes it onto the queue
    uint8_t buf[sizeof(CAN_message_t)] = {};
    memmove(buf, &can_msg, sizeof(CAN_message_t));
    msg_queue_->push_back(buf, sizeof(CAN_message_t));

    return can_msg;

}

//figure out how to set enumed led colors or send (0,255 value)
void DashboardInterface::setLED(DashLED_e led, LEDColors_e color)
{

    _data.LED[static_cast<uint8_t>(led)] = static_cast<uint8_t>(color);
}

void DashboardInterface::tick10(bool buzzer, bool ams_ok, bool imd_ok, bool bots)
{
    soundBuzzer(buzzer);

    if (ams_ok) setLED(DashLED_e::AMS_LED, LEDColors_e::ON);
    else setLED(DashLED_e::AMS_LED, LEDColors_e::RED);

    if (imd_ok) setLED(DashLED_e::IMD_LED, LEDColors_e::ON);
    else setLED(DashLED_e::IMD_LED, LEDColors_e::RED);

    if (bots) setLED(DashLED_e::BOTS_LED, LEDColors_e::ON);
    else setLED(DashLED_e::BOTS_LED, LEDColors_e::RED);

    write();
}

DialMode_e DashboardInterface::getDialMode() {return _data.dial_mode;}

bool DashboardInterface::startButtonPressed() {return _data.button.start;}
bool DashboardInterface::specialButtonPressed() {return _data.button.mark;}
bool DashboardInterface::torqueButtonPressed() {return _data.button.mode;}
bool DashboardInterface::inverterResetButtonPressed() {return _data.button.mc_cycle;}
bool DashboardInterface::launchControlButtonPressed() {return _data.button.launch_ctrl;}
bool DashboardInterface::torqueLoadingButtonPressed() {return _data.button.torque_mode;}
bool DashboardInterface::nightModeButtonPressed() {return _data.button.led_dimmer;}
bool DashboardInterface::leftShifterButtonPressed() {return _data.button.left_shifter;}
bool DashboardInterface::rightShifterButtonPressed() {return _data.button.right_shifter;}

bool DashboardInterface::safetySystemOK() {return _data.ssok;}
bool DashboardInterface::shutdownHAboveThreshold() {return _data.shutdown;}

void DashboardInterface::soundBuzzer(bool state) {_data.buzzer_cmd = state;}
bool DashboardInterface::checkBuzzer() {return _data.buzzer_state;}

