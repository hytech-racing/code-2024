#ifndef DASHBOARDCAN_H
#define DASHBOARDCAN_H

#include <Metro.h>
#include <STM32_CAN.h>
#include <HyTech_CAN.h>

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
        STM32_CAN _CAN;
        Metro heartbeat_timer = Metro(0);
        uint8_t imd_ams_flags = 0;
        CAN_message_t _msg;

        // CAN classes for different messages
        Dashboard_status dashboard_status{};
        MCU_status mcu_status{};
        MCU_analog_readings mcu_analog_readings{};
        BMS_voltages bms_voltages{};


        enum class _LED_MODES { OFF = 0, ON = 1, YELLOW = 2, RED = 3};
        enum _LED_LIST { AMS = 0, IMD = 1, MC_ERR = 2, GEN_PURP = 3, INERTIA = 4, BOTS = 5, 
                        COCKPIT_BRB = 6, CRIT_CHARGE = 7, GLV = 8, BRAKE_ENGAGE = 9, LAUNCH_CTRL = 10, 
                        TORQUE_MODE = 11, RDY_DRIVE = 12};

        #define LED_OFF 0x00
        #define LED_ON_GREEN 0xFF00
        #define LED_YELLOW 0xFFFF00
        #define LED_RED 0xFF0000
        #define LED_INIT 0xFF007F
        #define LED_BLUE 0xFF
        #define LED_WHITE 0xFFFFFFFF
    public:
        // constructor takes pointer to STM32_CAN object
        // can dereference with &
        DashboardCAN(STM32_CAN* CAN);
        void read_CAN();
        inline void mcu_status_received();
        inline void mcu_analog_readings_received();
        inline void bms_voltages_received();
        inline uint32_t color_wheel_glv(uint16_t voltage);
        inline uint32_t color_wheel_bms_glv(bool isBms);
};  

#endif