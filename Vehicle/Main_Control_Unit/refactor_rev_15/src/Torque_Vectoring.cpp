#include "Torque_Vectoring.h"
Torque_Vectoring::Torque_Vectoring()
{
    ht_data = HT_Data::getInstance();
    inverter_control = Inverter_Control::getInstance();

    rear_power_balance = 0.66;
    front_power_balance = 1.0 - rear_power_balance;
    rear_brake_balance = 0.1;
    front_brake_balance = 1.0 - rear_brake_balance;

    total_torque = 0;
    total_load_cells = 0;

    attesa_def_split = 0.85;
    attesa_alt_split = 0.5;
    fr_slip_clamped;
    fr_slip_factor = 2.5; // Factor of 5 causes 50/50 split at 20% r/f slip. Lower values allow more slip
    f_torque;
    r_torque;
    rear_lr_slip_clamped;
    lsd_right_split; // Fraction of rear axle torque going to rear right wheel
    lsd_slip_factor = 0.5;

    avg_speed = 0.0;
    start_derating_rpm = 2000;
    end_derating_rpm = 20000;

    hairpin_rpm_factor = 0.0;

    steering_calibration_slope = -0.111;
    steering_calibration_offset = 260.0;
    // positive steering angle is to the left
    hairpin_reallocation = 0.0;
    hairpin_steering_factor = 0.0;
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
    for (int i = 0; i < NUM_APPS;i++) {
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
    float steering_angle = ht_data->mcu_analog_readings.get_steering_2() * steering_calibration_slope + steering_calibration_offset;
    

    switch(state) {
        case 0: {

            break;
        }
        case 1: {
            
            break;
        }
        case 2: {
            
            break;
        }
        case 3: {
            
            break;
        }
        case 4: {
            
            break;
        }
        case 5: {
            
            break;
        }
        default:{ break};
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