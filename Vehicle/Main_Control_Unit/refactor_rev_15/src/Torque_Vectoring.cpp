#include "Torque_Vectoring.h"
Torque_Vectoring::Torque_Vectoring()
{
    ht_data = HT_Data::getInstance();
    ht_data->mcu_status.set_max_torque(TORQUE_3);
    ht_data->mcu_status.set_torque_mode(3);
    inverter_control = Inverter_Control::getInstance();

    rear_power_balance = 0.66;
    front_power_balance = 1.0 - rear_power_balance;
    rear_brake_balance = 0.1;
    front_brake_balance = 1.0 - rear_brake_balance;

    total_torque = 0;
    total_load_cells = 0;
    load_cell_alpha = 0.95;
    // attesa_def_split = 0.85;
    // attesa_alt_split = 0.5;

    // fr_slip_factor = 2.5; // Factor of 5 causes 50/50 split at 20% r/f slip. Lower values allow more slip
    // // Fraction of rear axle torque going to rear right wheel
    // lsd_slip_factor = 0.5;

    avg_speed = 0.0;
    start_derating_rpm = 2000;
    end_derating_rpm = 20000;

    // hairpin_rpm_factor = 0.0;

    steering_calibration_slope = -0.111;
    steering_calibration_offset = 260.0;
    //positive steering angle is to the left
    // hairpin_reallocation = 0.0;
    // hairpin_steering_factor = 0.0;
    max_front_power = 0.0;
    max_rear_power = 0.0;
    launch_state = NOT_READY;
    launch_speed_target = 0;
    launch_rate_target = 0.0;
}

void Torque_Vectoring::set_inverter_torques(int state)
{
    max_torque = ht_data->mcu_status.get_max_torque() / 0.0098; // max possible value for torque multiplier, unit in 0.1% nominal torque
    int avg_accel = 0;
    int avg_brake = 0;
    for (int i = 0; i < NUM_APPS; i++)
    {
        accel[i].val = min(max_torque, map(round((ht_data->mcu_pedal_readings.*accel[i].get_accel)()), accel[i].start, accel[i].end, 0, 2140));
        avg_accel += accel[i].val;
    }

    for (int i = 0; i < NUM_BRAKES; i++)
    {
        brake[i].val = map(round((ht_data->mcu_pedal_readings.*brake[i].get_brake)()), brake[i].start, brake[i].end, 0, 2140);
        avg_brake += brake[i].val;
    }
    avg_accel /= NUM_APPS;
    avg_brake /= NUM_BRAKES;
    avg_accel = min(max_torque, max(avg_accel, 0));
    avg_brake = min(1500, max(avg_brake, 0));
    int avg_speed = 0.0;
    for (int i = 0; i < NUM_INVERTERS; i++)
        avg_speed += ((float)inverter_control->get_inverter(i)->mc_status.get_speed()) / 4.0;

    steering_angle = ht_data->mcu_analog_readings.get_steering_2() * steering_calibration_slope + steering_calibration_offset;

    switch (state)
    {
    case 0:
    {
        launch_state = NOT_READY;
        safe_mode(avg_accel, avg_brake, 19000.0, 36000.0);

        break;
    }
    case 1:
    {
        launch_state = NOT_READY;
        load_cell_mode(avg_accel, avg_brake);
        break;
    }
    case 2:
    {
        launch_control(avg_accel, avg_brake, 21760.0, 41230.0, 11.76);
        break;
    }
    case 3:
    {
        launch_control(avg_accel, avg_brake, 21760.0, 41230.0, 12.74);
        break;
    }
    case 4:
    {
        launch_state = NOT_READY;
        endurance_mode(avg_accel, avg_brake, avg_speed);
        break;
    }
    case 5:
    {
        launch_state = NOT_READY;
        safe_mode(avg_accel, avg_brake, 21760.0, 41240.0);
        break;
    }
    default:
    {
        launch_state = NOT_READY;
        break;
    }
    }

    // power limiting time
    bool over_limit = true;
    for (int i = 0; i < NUM_INVERTERS; i++)
    {
        if (inverter_control->get_inverter(i)->mc_setpoints_command.get_pos_torque_limit() <= 0)
        {
            over_limit = false;
            break;
        }
    }
    if (over_limit)
    {
        float mech_power = 0;
        float voltage_lim_factor = 1.0;
        float temp_lim_factor = 1.0;
        float accu_lim_factor = 1.0;

        for (int i = 0; i < NUM_INVERTERS; i++)
        {
            float torque_in_nm = 9.8 * ((float)inverter_control->get_inverter(i)->mc_setpoints_command.get_pos_torque_limit()) / 1000.0;
            float speed_in_rpm = (float)inverter_control->get_inverter(i)->mc_status.get_speed();
            mech_power += 2 * 3.1415 * torque_in_nm * speed_in_rpm / 60.0;
        }
        voltage_lim_factor = float_map(ht_data->filtered_min_cell_voltage, 3.5, 3.2, 1.0, 0.2);
        voltage_lim_factor = max(min(1.0, voltage_lim_factor), 0.2);

        temp_lim_factor = float_map(ht_data->filtered_max_cell_temp, 50.0, 58.0, 1.0, 0.2);
        temp_lim_factor = max(min(1.0, temp_lim_factor), 0.2);

        accu_lim_factor = min(temp_lim_factor, voltage_lim_factor);
        for (int i = 0; i < NUM_INVERTERS; i++)
        {
            Inverter *inv = inverter_control->get_inverter(i);
            float power = max_front_power;
            if (!inv->is_front())
            {
                power = max_rear_power;
            }

            torque_setpoint_array[i] = (uint16_t)(min((float)torque_setpoint_array[i], max_allowed_torque(power / 2.0, (float)inv->mc_status.get_speed())) * accu_lim_factor);
        }

        int16_t max_speed_regen = 0;
        for (int i = 0; i < NUM_INVERTERS; i++)
        {
            Inverter *inv = inverter_control->get_inverter(i);
            max_speed_regen = (max_speed_regen < inv->mc_status.get_speed()) ? inv->mc_status.get_speed() : max_speed_regen;
        }
        for (int i = 0; i < NUM_INVERTERS; i++)
        {
            Inverter *inv = inverter_control->get_inverter(i);

            torque_setpoint_array[i] = max(-2140, min(2140, torque_setpoint_array[i]));

            if (torque_setpoint_array[i] >= 0)
            {
                inv->mc_setpoints_command.set_speed_setpoint(speed_setpoint_array[i]);
                inv->mc_setpoints_command.set_pos_torque_limit(min(torque_setpoint_array[i], 2140));
                inv->mc_setpoints_command.set_neg_torque_limit(0);
            }
            else
            {
                float scale_down = max(REGEN_OFF_START_THRESHOLD, min(max_speed_regen, 770));
                scale_down = map(max_speed_regen, 770, REGEN_OFF_START_THRESHOLD, 0, 1);

                inv->mc_setpoints_command.set_speed_setpoint(0);
                inv->mc_setpoints_command.set_pos_torque_limit(0);
                inv->mc_setpoints_command.set_neg_torque_limit(max(((int16_t)(torque_setpoint_array[i]) * scale_down), -2140));
            }
        }
    }
}

    void Torque_Vectoring::safe_mode(int accel, int brake, float max_front_power, float max_rear_power)
    {
        this->max_front_power = max_front_power;
        this->max_rear_power = max_rear_power;
        float balance = 0;
        for (int i = 0; i < NUM_INVERTERS; i++)
        {
            torque_setpoint_array[i] = accel - brake;

            if (torque_setpoint_array[i] >= 0)
            {
                balance = front_power_balance;
                if (!inverter_control->get_inverter(i)->is_front())
                {
                    balance = rear_power_balance;
                }
            }
            else
            {
                balance = front_brake_balance;
                if (!inverter_control->get_inverter(i)->is_front())
                {
                    balance = rear_brake_balance;
                }
            }
            torque_setpoint_array[i] = (int16_t)(torque_setpoint_array[i] * balance);
        }
    }

    // void Torque_Vectoring::nissan_mode(int accel, int brake)
    // {
    // }
    void Torque_Vectoring::endurance_mode(int accel, int brake, int avg_speed)
    {
        this->max_front_power = 19000.0;
        this->max_rear_power = 36000.0;
        int32_t total_torque = NUM_LOAD_CELLS * (accel - brake);

        int32_t total_load_cells = 0;
        // derating
        int16_t start_derating_rpm = 2000;
        int16_t end_derating_rpm = 20000;
        float derating_factor = float_map(avg_speed, start_derating_rpm, end_derating_rpm, 1.0, 0.0);
        derating_factor = min(1.0, max(0.0, derating_factor));
        for (int i = 0; i < NUM_LOAD_CELLS; i++)
        {
            total_load_cells += (ht_data->mcu_load_cells.*load_cell[i])();
        }
        for (int i = 0; i < NUM_INVERTERS; i++)
        {
            speed_setpoint_array[i] = MAX_ALLOWED_SPEED;
            torque_setpoint_array[i] = (int16_t)((float)(ht_data->mcu_load_cells.*load_cell[i])() / (float)total_load_cells * (float)total_torque * derating_factor);
            if (accel < brake && !inverter_control->get_inverter(i)->is_front())
            {
                torque_setpoint_array[i] = (int16_t)((float)(ht_data->mcu_load_cells.*load_cell[i])() / (float)total_load_cells * (float)total_torque / 2.0);
            }
        }
    }
    void Torque_Vectoring::launch_control(int accel, int brake, float max_front_power, float max_rear_power, float launch_rate_target)
    {
        this->max_front_power = max_front_power;
        this->max_rear_power = max_rear_power;
        uint16_t max_speed = 0;
        float launch_speed_target = 0;
        for (int i = 0; i < NUM_INVERTERS; i++)
        {
            max_speed = max(max_speed, inverter_control->get_inverter(i)->mc_status.get_speed());
        }

        switch (launch_state)
        {
        case NOT_READY:
        {
            for (int i = 0; i < NUM_INVERTERS; i++)
            {
                torque_setpoint_array[i] = (int16_t)(-1 * brake);
                speed_setpoint_array[i] = 0;
            }
            time_since_launch = 0;
            // To enter launch_ready, the following conditions must be true:
            // 1. Pedals are not pressed
            // 2. Speed is zero
            if (accel < LAUNCH_READY_ACCEL_THRESHOLD && brake < LAUNCH_READY_BRAKE_THRESHOLD && max_speed < LAUNCH_READY_SPEED_THRESHOLD)
            {
                launch_state = READY;
            }
            break;
        }

        case READY:
        {
            for (int i = 0; i < NUM_INVERTERS; i++)
            {
                torque_setpoint_array[i] = 0;
                speed_setpoint_array[i] = 0;
            }
            time_since_launch = 0;

            // Revert to launch_not_ready if brake is pressed or speed is too high
            if (brake >= LAUNCH_READY_BRAKE_THRESHOLD || max_speed >= LAUNCH_READY_SPEED_THRESHOLD)
            {
                launch_state = NOT_READY;
            }
            else if (accel >= LAUNCH_GO_THRESHOLD)
            {
                launch_state = LAUNCH;
            }
            break;
        }
        case LAUNCH:
        {
            if (accel <= LAUNCH_STOP_THRESHOLD || brake >= LAUNCH_READY_BRAKE_THRESHOLD)
            {
                launch_state = NOT_READY;
                break;
            }

            launch_speed_target = (int16_t)((float)time_since_launch / 1000.0 * launch_rate_target * 60.0 / 1.2767432544 * 11.86);
            launch_speed_target += 1500; // wth is this?
            launch_speed_target = min(20000, max(0, launch_speed_target));

            for (int i = 0; i < NUM_INVERTERS; i++)
            {
                torque_setpoint_array[i] = 2142;
                speed_setpoint_array[i] = launch_speed_target;
            }
            break;
        }
        }
    }
    void Torque_Vectoring::load_cell_mode(int accel, int brake)
    {
        this->max_front_power = 19000.0;
        this->max_rear_power = 36000.0;
        int32_t total_torque = NUM_LOAD_CELLS * (accel - brake);

        int32_t total_load_cells = 0;
        for (int i = 0; i < NUM_LOAD_CELLS; i++)
        {
            total_load_cells += (ht_data->mcu_load_cells.*load_cell[i])();
        }
        for (int i = 0; i < NUM_INVERTERS; i++)
        {
            speed_setpoint_array[i] = MAX_ALLOWED_SPEED;
            torque_setpoint_array[i] = (int16_t)((float)(ht_data->mcu_load_cells.*load_cell[i])() / (float)total_load_cells * (float)total_torque);
            if (accel < brake && !inverter_control->get_inverter(i)->is_front())
            {
                torque_setpoint_array[i] = (int16_t)((float)(ht_data->mcu_load_cells.*load_cell[i])() / (float)total_load_cells * (float)total_torque / 2.0);
            }
        }
    }

    float Torque_Vectoring::max_allowed_torque(float maxwatts, float rpm)
    {
        float angularspeed = (abs(rpm) + 1) / 60 * 2 * 3.1415;
        float maxnm = min(maxwatts / angularspeed, 20);
        return maxnm / 9.8 * 1000;
    }

    float Torque_Vectoring::float_map(float x, float in_min, float in_max, float out_min, float out_max)
    {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
    void Torque_Vectoring::debugPedals() {
        Debug_println("ACCEL");
        for (int i = 0; i < NUM_APPS; i++) {
            Debug_println((ht_data->mcu_pedal_readings.*accel[i].get_accel)());
        }
        Debug_println("BRAKES");
        for (int i = 0; i < NUM_BRAKES; i++) {
            Debug_println((ht_data->mcu_pedal_readings.*brake[i].get_brake)());
        }
        // calculate_pedal_implausibilities();
    //    Debug_println(mcu_status.get_no_accel_implausability());
    //    Debug_println(mcu_status.get_no_brake_implausability());
    //    Debug_println(mcu_status.get_no_accel_brake_implausability());
    //    int brake1 = map(round(mcu_pedal_readings.get_brake_pedal_1()), START_BRAKE_PEDAL_1, END_BRAKE_PEDAL_1, 0, 2140);
    //    int brake2 = map(round(mcu_pedal_readings.get_brake_pedal_2()), START_BRAKE_PEDAL_2, END_BRAKE_PEDAL_2, 0, 2140);
    //    Debug_println(brake1);
    //    Debug_println(brake2);
    }
    void Torque_Vectoring::debugLoadCells() {
         Debug_println();
         Debug_println("LOAD CELLS");
         for (int i = 0; i < NUM_LOAD_CELLS; i++) {
            Debug_println((ht_data->mcu_load_cells.*load_cell[i])());
         }
    }
    void Torque_Vectoring::debugTorque() {
         Debug_println();
         Debug_println("TORQUE SETPOINTS");
        for (int i = 0; i < NUM_LOAD_CELLS; i++) {
            Debug_println(torque_setpoint_array[i]);
         }
    }