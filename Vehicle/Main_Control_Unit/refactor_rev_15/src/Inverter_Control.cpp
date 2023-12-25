#include "Inverter_Control.h"
/**
 * @brief Construct a new Inverter_Control object
 *
 * @param _ht_data pointer to ht_data struct
 */
Inverter_Control::Inverter_Control() : inverters {Inverter(true, true), Inverter(true, false), Inverter(false, true), Inverter(false, false)}
{
    ht_data = HT_Data::getInstance();
    inverter_restart = false;
    inverter_startup_state = INVERTER_STARTUP_STATE::ALL_INVERTERS_DISABLED;
    front_inv = true;
    rear_inv = true;
    
}
Inverter_Control Inverter_Control::inverter_control;
Inverter_Control *Inverter_Control::getInstance()
{
    return &inverter_control;
}

void Inverter_Control::init_inverters()
{
    set_all_inverters_disabled();
// change to input if comparator is PUSH PULL
    pinMode(INVERTER_EN, OUTPUT);
    pinMode(INVERTER_24V_EN, OUTPUT);
    digitalWrite(INVERTER_24V_EN, HIGH);
    digitalWrite(INVERTER_EN, HIGH);
    ht_data->mcu_status.set_inverters_error(false);
}

bool Inverter_Control::get_inverter_restart()
{
    return inverter_restart;
}
void Inverter_Control::set_inverter_restart(bool _inverter_restart)
{
    inverter_restart = _inverter_restart;
}
void Inverter_Control::set_inv_state(INVERTER_STARTUP_STATE new_inv_state) {
    if (new_inv_state == inverter_startup_state) return;
    inverter_startup_state = new_inv_state;
    if (new_inv_state == INVERTER_STARTUP_STATE::ALL_INVERTERS_DISABLED) {
        set_all_inverters_disabled();
    }
    
}
void Inverter_Control::inv_state_machine()
{
    switch (inverter_startup_state)
    {
    case INVERTER_STARTUP_STATE::WAIT_SYSTEM_READY:
        if (check_all_inverters_system_ready())
        {
            set_all_inverters_dc_on(true);
            set_inv_state(INVERTER_STARTUP_STATE::WAIT_QUIT_DC_ON);
        }
        break;

    case INVERTER_STARTUP_STATE::WAIT_QUIT_DC_ON:
        if (check_all_inverters_quit_dc_on())
        {
            set_all_inverters_no_torque();
            set_all_inverters_driver_enable(true);
            set_all_inverters_inverter_enable(true);
            set_inv_state(INVERTER_STARTUP_STATE::WAIT_QUIT_INVERTER_ON);
        }
        break;

    case INVERTER_STARTUP_STATE::WAIT_QUIT_INVERTER_ON:
        if (check_all_inverters_quit_inverter_on())
        {
            set_inv_state(INVERTER_STARTUP_STATE::ALL_INVERTERS_QUIT_INVERTER_ON);
        }
        break;
    case INVERTER_STARTUP_STATE::ALL_INVERTERS_QUIT_INVERTER_ON:
        if (check_all_inverters_error())
           set_inv_state(INVERTER_STARTUP_STATE::ALL_INVERTERS_DISABLED); 
        break;
    case INVERTER_STARTUP_STATE::ALL_INVERTERS_DISABLED:
        //entry logic could be fixed a lil
        break;
    }
}
/**
 * @brief are all inverters system ready
 *
 * @return true
 * @return false
 */
bool Inverter_Control::check_all_inverters_system_ready()
{
    for (uint8_t inv = 0; inv < NUM_INVERTERS; inv++)
    {
        if (!inverters[inv].mc_status.get_system_ready())
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief are all inverters
 *
 * @return true
 * @return false
 */
bool Inverter_Control::check_all_inverters_quit_dc_on()
{
    for (uint8_t inv = 0; inv < NUM_INVERTERS; inv++)
    {
        if (!inverters[inv].mc_status.get_quit_dc_on())
        {
            return false;
        }
    }
    return true;
}
/**
 * @brief
 *
 * @return true
 * @return false
 */
bool Inverter_Control::check_all_inverters_quit_inverter_on()
{
    for (uint8_t inv = 0; inv < NUM_INVERTERS; inv++)
    {
        if (!inverters[inv].mc_status.get_quit_inverter_on())
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief Inverter_Control::check if inverters have any errors
 *
 * @return uint8_t which inverters have errors
 */
uint8_t Inverter_Control::check_all_inverters_error()
{
    uint8_t error_list = 0; // last 4 bits correspond to error bit in status word of each inverter, inverter 1 is rightmost bit;
    for (uint8_t inv = 0; inv < NUM_INVERTERS; inv++)
    {
        if (inverters[inv].mc_status.get_error())
        {
            error_list = error_list | (0x01 << inv);
        }
    }
    if (error_list)
    {
        ht_data->mcu_status.set_inverters_error(true);
    }
    else
    {
        ht_data->mcu_status.set_inverters_error(false);
    }
    return error_list;
}

/**
 * @brief Set the all inverters no torque object
 *
 */
void Inverter_Control::set_all_inverters_no_torque()
{
    for (uint8_t inv = 0; inv < NUM_INVERTERS; inv++)
    {
        inverters[inv].mc_setpoints_command.set_speed_setpoint(0);
        inverters[inv].mc_setpoints_command.set_pos_torque_limit(0);
        inverters[inv].mc_setpoints_command.set_neg_torque_limit(0);
    }
}

/**
 * @brief Set the all inverters torque limit object
 *
 * @param limit torque limit
 */
void Inverter_Control::set_all_inverters_torque_limit(int limit)
{

    // const float max_torque = max_torque_Nm / 0.0098; // max possible value for torque multiplier, unit in 0.1% nominal torque
    // inverter torque unit is in 0.1% of nominal torque (9.8Nm), max rated torque is 21Nm, so max possible output is 2142

    if (limit >= 0)
    {
        for (uint8_t inv = 0; inv < NUM_INVERTERS; inv++)
        {
            inverters[inv].mc_setpoints_command.set_pos_torque_limit(limit);
            inverters[inv].mc_setpoints_command.set_neg_torque_limit(0);
        }
    }
    else
    {
        for (uint8_t inv = 0; inv < NUM_INVERTERS; inv++)
        {
            inverters[inv].mc_setpoints_command.set_pos_torque_limit(0);
            inverters[inv].mc_setpoints_command.set_neg_torque_limit(limit);
        }
    }
}

/**
 * @brief Set the all inverters speed setpoint object
 *
 * @param setpoint
 */
void Inverter_Control::set_all_inverters_speed_setpoint(int setpoint)
{
    for (uint8_t inv = 0; inv < NUM_INVERTERS; inv++)
    {
        inverters[inv].mc_setpoints_command.set_speed_setpoint(setpoint);
    }
}

/**
 * @brief Set the all inverters disabled object
 *
 */
void Inverter_Control::set_all_inverters_disabled()
{
    for (uint8_t inv = 0; inv < NUM_INVERTERS; inv++)
    {
        inverters[inv].mc_setpoints_command.set_inverter_enable(false);
        inverters[inv].mc_setpoints_command.set_hv_enable(false);
        inverters[inv].mc_setpoints_command.set_driver_enable(false);
        inverters[inv].mc_setpoints_command.set_remove_error(false);
        inverters[inv].mc_setpoints_command.set_speed_setpoint(0);
        inverters[inv].mc_setpoints_command.set_pos_torque_limit(0);
        inverters[inv].mc_setpoints_command.set_neg_torque_limit(0);
    }
}
/**
 * @brief Set the all inverters dc on object
 *
 * @param input
 */
void Inverter_Control::set_all_inverters_dc_on(bool input)
{
    for (uint8_t inv = 0; inv < NUM_INVERTERS; inv++)
    {
       inverters[inv].mc_setpoints_command.set_hv_enable(input);
    }
}

/**
 * @brief Set the all inverters driver enable object
 *
 * @param input ...
 */
void Inverter_Control::set_all_inverters_driver_enable(bool input)
{
    for (uint8_t inv = 0; inv < NUM_INVERTERS; inv++)
    {
        inverters[inv].mc_setpoints_command.set_driver_enable(input);
    }
}
/**
 * @brief Set the all inverters inverter enable object
 *
 * @param input ...
 */
void Inverter_Control::set_all_inverters_inverter_enable(bool input)
{
    for (uint8_t inv = 0; inv < NUM_INVERTERS; inv++)
    {
        inverters[inv]mc_setpoints_command.set_inverter_enable(input);
    }
}
/**
 * @brief reset the inverters if flag is true
 *
 */
void Inverter_Control::reset_inverters()
{
    if (inverter_restart)
    {
        uint8_t reset_bit = !inverters[0].mc_setpoints_command.get_remove_error();
        if (timer_reset_inverter.check())
        {
            inverter_restart = false;
            reset_bit = 0;
        }
        for (uint8_t inv = 0; inv < NUM_INVERTERS; inv++)
        {
            inverters[inv].mc_setpoints_command.set_remove_error(reset_bit);
        }
    }
}
inline void Inverter_Control::power_off_inverter()
{
    digitalWrite(INVERTER_24V_EN, LOW);
}
MCU_STATE Inverter_Control::check_TS_active(MCU_STATE mcu_state)
{
    if (!check_TS_over_HV_threshold())
    {
        Debug_println("Setting state to TS Not Active, because TS is below HV threshold");
        // set_state(MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE);
        // add this to main ecu (lets not hide this)
        inverter_startup_state = INVERTER_STARTUP_STATE::WAIT_SYSTEM_READY;
        return MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE; // this too
    }
    return mcu_state;
}

bool Inverter_Control::check_TS_over_HV_threshold()
{
    for (uint8_t inv = 0; inv < NUM_INVERTERS; inv++)
    {
        if (inverters[inv].mc_energy.get_dc_bus_voltage() < MIN_HV_VOLTAGE)
        {
            return false;
        }
    }
    return true;
}

INVERTER_STARTUP_STATE Inverter_Control::get_inverter_state()
{
    return inverter_startup_state;
}

bool Inverter_Control::get_if_fronts() {
    return front_inv;
}
    
    bool Inverter_Control::get_if_rears() {
        return rear_inv;
    }

    Inverter* Inverter_Control::get_inverter(int inv) {
        return &inverters[inv];
    }