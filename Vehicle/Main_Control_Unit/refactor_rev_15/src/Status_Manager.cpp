/**
 * @file Status_Manager.cpp
 * @author Eric Galluzzi
 * @brief
 * @version 0.1
 * @date 2023-12-23
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "Status_Manager.h"
Status_Manager::Status_Manager()
{
    ht_data = HT_Data::getInstance();
    ht_data->mcu_status.set_software_is_ok(true);
    pinMode(BRAKE_LIGHT_CTRL, OUTPUT);
    // the initial state of the watchdog is high
    // this is reflected in the static watchdog_state
    // starting high
    digitalWrite(WATCHDOG_INPUT, HIGH);
    pinMode(SOFTWARE_OK, OUTPUT);
    digitalWrite(SOFTWARE_OK, HIGH);
    ht_data->mcu_status.set_bms_ok_high(false);
    ht_data->mcu_status.set_imd_ok_high(false);
    delay(5000);
}
void Status_Manager::status_poll() {
    software_shutdown();
    brake_outputs();
    read_status_values();
}

void Status_Manager::activate_buzzer(bool on)
{
    ht_data->mcu_status.set_activate_buzzer(on);
}

void Status_Manager::software_shutdown()
{
    ht_data->mcu_status.set_software_is_ok(true);
    if (timer_bms_heartbeat.check())
    {
        timer_bms_heartbeat.interval(0);
        Debug_println("no bms heartbeat");
        ht_data->mcu_status.set_software_is_ok(false);
    }
    if (ht_data->mcu_status.get_software_is_ok())
    {
        digitalWrite(SOFTWARE_OK, HIGH);
    }
    else
    {
        digitalWrite(SOFTWARE_OK, LOW);
    }
    if (timer_watchdog_timer.check())
    {
        static bool watchdog_state = HIGH;
        watchdog_state = !watchdog_state;
        digitalWrite(WATCHDOG_INPUT, watchdog_state);
    }
}

bool Status_Manager::ready_to_drive() {
    return ht_data->dashboard_status.get_start_btn() && ht_data->mcu_status.get_mech_brake_active();
}
void Status_Manager::brake_outputs()
{
    ht_data->mcu_status.set_brake_pedal_active(ht_data->mcu_pedal_readings.get_brake_pedal_1() >= BRAKE_ACTIVE);
    digitalWrite(BRAKE_LIGHT_CTRL, ht_data->mcu_status.get_brake_pedal_active());
    ht_data->mcu_status.set_mech_brake_active(ht_data->mcu_pedal_readings.get_brake_pedal_1() >= BRAKE_THRESHOLD_MECH_BRAKE_1); // define in driver_constraints.h (70%)
}

void Status_Manager::read_status_values()
{
    ht_data->mcu_status.set_bms_ok_high(digitalRead(BMS_OK_READ));
    ht_data->mcu_status.set_imd_ok_high(digitalRead(IMD_OK_READ));
    ht_data->mcu_status.set_bspd_ok_high(digitalRead(BSPD_OK_READ));
    ht_data->mcu_status.set_software_ok_high(digitalRead(SOFTWARE_OK_READ));

    /* Measure shutdown circuits' voltages */
    // add more shutdowns
    ht_data->mcu_status.set_shutdown_b_above_threshold(digitalRead(BOTS_OK_READ));
    ht_data->mcu_status.set_shutdown_c_above_threshold(digitalRead(IMD_OK_READ));
    ht_data->mcu_status.set_shutdown_d_above_threshold(digitalRead(BMS_OK_READ));
    ht_data->mcu_status.set_shutdown_e_above_threshold(digitalRead(BSPD_OK_READ));
}

bool Status_Manager::valid_pedals() {
    calculate_pedal_implausibilities();
    return pedal_implausability_duration <= 100;
}
bool Status_Manager::valid_HV(){
    return ht_data->mcu_status.get_bms_ok_high() && ht_data->mcu_status.get_imd_ok_high();
}
void Status_Manager::calculate_pedal_implausibilities()
{
    calculate_accel_implausabilities();

    calculate_bse_implausabilities();

    calculate_apps_brake_implausabilities();
}

void Status_Manager::calculate_accel_implausabilities()
{
    // FSAE EV.5.5
    // FSAE T.4.2.10

    if (ht_data->mcu_pedal_readings.get_accelerator_pedal_1() < MIN_ACCELERATOR_PEDAL_1 || ht_data->mcu_pedal_readings.get_accelerator_pedal_1() > MAX_ACCELERATOR_PEDAL_1)
    {
        ht_data->mcu_status.set_no_accel_implausability(false);
        Debug_println("T.4.2.10 1");
    }
    else if (ht_data->mcu_pedal_readings.get_accelerator_pedal_2() > MAX_ACCELERATOR_PEDAL_2 || ht_data->mcu_pedal_readings.get_accelerator_pedal_2() < MIN_ACCELERATOR_PEDAL_2)
    {
        ht_data->mcu_status.set_no_accel_implausability(false);

        Debug_println("T.4.2.10 2");
    }
    // check that the pedals are reading within 10% of each other
    // T.4.2.4
    else if (fabs((ht_data->mcu_pedal_readings.get_accelerator_pedal_1() - START_ACCELERATOR_PEDAL_1) / (END_ACCELERATOR_PEDAL_1 - START_ACCELERATOR_PEDAL_1) -
                  (ht_data->mcu_pedal_readings.get_accelerator_pedal_2() - START_ACCELERATOR_PEDAL_2) / (END_ACCELERATOR_PEDAL_2 - START_ACCELERATOR_PEDAL_2)) > 0.1)
    {
        Debug_println("T.4.2.4");
        Debug_printf("pedal 1 - %f\n", (ht_data->mcu_pedal_readings.get_accelerator_pedal_1() - START_ACCELERATOR_PEDAL_1) / (END_ACCELERATOR_PEDAL_1 - START_ACCELERATOR_PEDAL_1));
        Debug_printf("pedal 2 - %f\n", (ht_data->mcu_pedal_readings.get_accelerator_pedal_2() - START_ACCELERATOR_PEDAL_2) / (END_ACCELERATOR_PEDAL_2 - START_ACCELERATOR_PEDAL_2));

        ht_data->mcu_status.set_no_accel_implausability(false);
    }
    else
    {
        ht_data->mcu_status.set_no_accel_implausability(true);
    }
}
void Status_Manager::calculate_bse_implausabilities()
{
    // BSE check
    // EV.5.6
    // FSAE T.4.3.4
    if (ht_data->mcu_pedal_readings.get_brake_pedal_1() < MIN_BRAKE_PEDAL_1 || ht_data->mcu_pedal_readings.get_brake_pedal_1() > MAX_BRAKE_PEDAL_1)
    {
        ht_data->mcu_status.set_no_brake_implausability(false);
    }
    else if (ht_data->mcu_pedal_readings.get_brake_pedal_2() > MIN_BRAKE_PEDAL_2 || ht_data->mcu_pedal_readings.get_brake_pedal_2() < MAX_BRAKE_PEDAL_2)
    { // negative slope for brake 2
        ht_data->mcu_status.set_no_brake_implausability(false);
    }
    else if (fabs((ht_data->mcu_pedal_readings.get_brake_pedal_1() - START_BRAKE_PEDAL_1) / (END_BRAKE_PEDAL_1 - START_BRAKE_PEDAL_1) -
                  (START_BRAKE_PEDAL_2 - ht_data->mcu_pedal_readings.get_brake_pedal_2()) / (START_BRAKE_PEDAL_2 - END_BRAKE_PEDAL_2)) > 0.25)
    {
        ht_data->mcu_status.set_no_brake_implausability(false);
    }
    else
    {
        ht_data->mcu_status.set_no_brake_implausability(true);
    }
}
void Status_Manager::calculate_apps_brake_implausabilities()
{
    // FSAE EV.5.7
    // APPS/Brake Pedal Plausability Check

    if (((ht_data->mcu_pedal_readings.get_accelerator_pedal_1() > ((END_ACCELERATOR_PEDAL_1 - START_ACCELERATOR_PEDAL_1) / 4 + START_ACCELERATOR_PEDAL_1)) ||
         (ht_data->mcu_pedal_readings.get_accelerator_pedal_2() > ((END_ACCELERATOR_PEDAL_2 - START_ACCELERATOR_PEDAL_2) / 4 + START_ACCELERATOR_PEDAL_2))) &&
          ht_data->mcu_status.get_mech_brake_active())
    {
        ht_data->mcu_status.set_no_accel_brake_implausability(false);
    }
    else if (
        (ht_data->mcu_pedal_readings.get_accelerator_pedal_1() < ((END_ACCELERATOR_PEDAL_1 - START_ACCELERATOR_PEDAL_1) / 20 + START_ACCELERATOR_PEDAL_1)) &&
        (ht_data->mcu_pedal_readings.get_accelerator_pedal_2() < ((END_ACCELERATOR_PEDAL_2 - START_ACCELERATOR_PEDAL_2) / 20 + START_ACCELERATOR_PEDAL_2)))
    {
        ht_data->mcu_status.set_no_accel_brake_implausability(true);
    }
    if (ht_data->mcu_status.get_no_accel_implausability() && ht_data->mcu_status.get_no_brake_implausability() && ht_data->mcu_status.get_no_accel_brake_implausability())
    {
        pedal_implausability_duration = 0;
    }
}

void Status_Manager::debugPOTS() {
    Debug_println("SUS POTS");
    Debug_println(ht_data->mcu_potentiometers.get_pot1());
    Debug_println(ht_data->mcu_potentiometers.get_pot2());
    Debug_println(ht_data->mcu_potentiometers.get_pot3());
    Debug_println(ht_data->mcu_potentiometers.get_pot4());
    Debug_println();
}
void Status_Manager::debugDash() {
    Debug_println("DIAL");
    Debug_println(ht_data->dashboard_status.get_dial_state());
    Debug_println();
}

