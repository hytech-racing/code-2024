/**
 * @file Inverter_Control.h
 * @author Eric Galluzzi
 * @brief inverter interface for functions such as torque vectoring
 * @version 0.1
 * @date 2023-12-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once;
#include "timer_utils.h"
#include "debugging_utils.h"
#include "HT_Data.h"
#include "MCU_rev15_dfs.h"
#include "Inverter.h"
#define NUM_INVERTERS 4
class Inverter_Control
{
private:
    HT_Data *ht_data;
    bool inverter_restart;
    INVERTER_STARTUP_STATE inverter_startup_state;
    Inverter_Control();
    Inverter_Control(const Inverter_Control&);
    Inverter_Control& operator=(const Inverter_Control&);
    static Inverter_Control inverter_control;
    bool front_inv;
    bool rear_inv;
    Inverter inverters[NUM_INVERTERS];
public:
/**
 * @brief Get if there are front inverters (ht07)
 * 
 * @return true 
 * @return false 
 */
    bool get_if_fronts();
    /**
     * @brief Get if there are rear inverters
     * 
     * @return true 
     * @return false 
     */
    bool get_if_rears();
    /**
     * @brief Get a pointer to inverter
     * 
     * @param inv 
     * @return Inverter* 
     */
    Inverter* get_inverter(int inv);
    /**
     * @brief get instance of Inverter_Control object
     * 
     */
    static Inverter_Control* getInstance();
    /**
     * @brief Get the inverter state object
     * 
     * @return INVERTER_STARTUP_STATE 
     */
    INVERTER_STARTUP_STATE get_inverter_state();

    /**
     * @brief initialize inverters
     * 
     */
    void init_inverters();
    /**
     * @brief check TS is active
     * 
     * @return true 
     * @return false 
     */
    MCU_STATE check_TS_active(MCU_STATE mcu_state);
    /**
     * @brief check if TS is below or above threshold
     * 
     * @return true 
     * @return false 
     */
    bool check_TS_over_HV_threshold();
    /**
     * @brief getter method for inverter restart state
     * 
     * @return true 
     * @return false 
     */
    bool Inverter_Control::get_inverter_restart();
    /**
     * @brief setter for inverter restart
     * 
     * @param _inverter_restart true or false
     */
    void  Inverter_Control::set_inverter_restart(bool _inverter_restart);
    /**
     * @brief Set the inv state object
     * 
     * @param new_inv_state 
     */
    void set_inv_state(INVERTER_STARTUP_STATE new_inv_state);
    /**
     * @brief inverter_state_machine handler
     * 
     */
    void inv_state_machine();
    /**
     * @brief are all inverters system ready
     *
     * @return true
     * @return false
     */
    bool check_all_inverters_system_ready();

    /**
     * @brief are all inverters
     *
     * @return true
     * @return false
     */
    bool check_all_inverters_quit_dc_on();
    /**
     * @brief
     *
     * @return true
     * @return false
     */
    bool check_all_inverters_quit_inverter_on();

    /**
     * @brief check if inverters have any errors
     *
     * @return uint8_t which inverters have errors
     */
    uint8_t check_all_inverters_error();

    /**
     * @brief Set the all inverters no torque object
     *
     */
    void set_all_inverters_no_torque();

    /**
     * @brief Set the all inverters torque limit object
     *
     * @param limit torque limit
     */
    void set_all_inverters_torque_limit(int limit);

    /**
     * @brief Set the all inverters speed setpoint object
     *
     * @param setpoint
     */
    void set_all_inverters_speed_setpoint(int setpoint);

    /**
     * @brief Set the all inverters disabled object
     *
     */
    void set_all_inverters_disabled();
    /**
     * @brief Set the all inverters dc on object
     *
     * @param input
     */
    void set_all_inverters_dc_on(bool input);

    /**
     * @brief Set the all inverters driver enable object
     *
     * @param input ...
     */
    void set_all_inverters_driver_enable(bool input);
    /**
     * @brief Set the all inverters inverter enable object
     *
     * @param input ...
     */
    void set_all_inverters_inverter_enable(bool input);
    /**
     * @brief reset the inverters if flag is true
     * 
     */
    void reset_inverters();
    void power_off_inverter();
};
