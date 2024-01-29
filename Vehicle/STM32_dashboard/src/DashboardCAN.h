#ifndef DASHBOARDCAN_H
#define DASHBOARDCAN_H

#include <Metro.h>
#include <STM32_CAN.h>
// #include "HyTech_CAN.h"
#include "hytech_dashboard.h"
#include "ht_can.h"

#define MCU_HEARTBEAT_TIMEOUT 1000

#define GLV_THRESHOLD 3104
#define PACK_THRESHOLD 33000
#define OUTSIDE_BRIGHTNESS 255
#define LOW_BRIGHTNESS 16

#define LED_MIN_FAULT 23

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
        Metro send_timer = Metro(1000);
        uint8_t imd_ams_flags = 0;
        CAN_message_t _msg;
    public:
        // CAN structs for different messages

        /* SEND */
        DASHBOARD_STATE_t dash_state;

        /* RECEIVE */
        MCU_STATUS_t mcu_status;
        MCU_ANALOG_READINGS_t mcu_analog_readings;
        BMS_VOLTAGES_t bms_voltages;
        MCU_PEDAL_READINGS_t mcu_pedal_readings;
        MCU_LOAD_CELLS_t mcu_load_cells;
        
        TCU_LAP_TIMES_t lap_times;
        TCU_DRIVER_MSG_t driver_msg;
        DASHBOARD_MCU_STATE_t dash_mcu_state;

        

        // constructor takes pointer to STM32_CAN object
        // can dereference with &
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