#ifndef DASHBOARDCAN_H
#define DASHBOARDCAN_H

#include <Metro.h>
#include <STM32_CAN.h>
// #include "HyTech_CAN.h"
#include "hytech_dashboard.h"
#include "hytech.h"

#define MCU_HEARTBEAT_TIMEOUT 1000

#define GLV_THRESHOLD 3104
#define PACK_THRESHOLD 33000
#define OUTSIDE_BRIGHTNESS 255
#define LOW_BRIGHTNESS 16

/*
    The DashboardCAN class handles all interaction with the CAN interface through the use
    of a pointer to the STM32_CAN object corresponding to the main CAN interface. All reading/writing
    to the CAN bus is handled in this class utilizing ht_can to pack, unpack, and store the data in
    message-specific structs.
*/
class DashboardCAN {
    private:
        // STM32_CAN member variable
        //member vars designated by _ prefix
        STM32_CAN* _CAN;
        // Creates metro timer with 0 interval and autoreset
        // This metro timer acts as a latch to prevent the dashboard from sending/flooding messages
        // onto the CAN line if it has not received messages in 1 second
        // A metro of interval 0 will always be true, triggering
        Metro heartbeat_timer = Metro(2000, 1);
        Metro send_timer = Metro(100);
        Metro mcu_state_timer = Metro(1000);
        uint8_t imd_ams_flags = 0;
        CAN_message_t _msg;

        uint64_t prev_dash_mcu_state = 0;
        DASHBOARD_STATE_t prev_dash_state;
    public:
        // CAN structs for different messages

        /* SEND */
        DASHBOARD_STATE_t dash_state;

        /* RECEIVE */
        MCU_STATUS_t mcu_status;
        MCU_ANALOG_READINGS_t mcu_analog_readings;
        BMS_VOLTAGES_t bms_voltages;
        MCU_PEDAL_READINGS_t mcu_pedal_readings;
        MCU_SUSPENSION_t mcu_suspension;
        SAB_SUSPENSION_t sab_suspension;
        DRIVETRAIN_RPMS_TELEM_t drivetrain_rpms;
        MC1_STATUS_t mc1_status;
        EM_MEASUREMENT_t em_measurement;
        MCU_PEDAL_RAW_t pedal_raw;
        VN_STATUS_t vn_status;
        CONTROLLER_BOOLEAN_t controller_boolean;
        CONTROLLER_POWER_LIM_t controller_power_lim;
        
        TCU_LAP_TIMES_t lap_times;
        TCU_DRIVER_MSG_t driver_msg;
        DASHBOARD_MCU_STATE_t dash_mcu_state;

        bool mcu_state_update = true;
        bool send_now = false;

        /* SHUTDOWN VARS */
        bool inertia_read;
        bool brb_read;
        

        // constructor takes pointer to STM32_CAN object
        DashboardCAN(STM32_CAN* CAN);
        void read_CAN();
        void send_status();
        inline void mcu_status_received();
        inline void mcu_analog_readings_received();
        inline void bms_voltages_received();
        inline uint32_t color_wheel_glv(uint16_t voltage);
        inline uint32_t color_wheel_bms_glv(bool isBms);
};  

#endif