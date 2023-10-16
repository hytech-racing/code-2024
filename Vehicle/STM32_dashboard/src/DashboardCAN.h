#ifndef DASHBOARDCAN_H
#define DASHBOARDCAN_H

#include <Metro.h>
#include <STM32_CAN.h>
#include <HyTech_CAN.h>

class DashboardCAN {
    private:
        // STM32_CAN member variable
        //member vars designated by _ prefix
        STM32_CAN _CAN;

        Dashboard_status dashboard_status{};
        MCU_status mcu_status{};
        MCU_analog_readings mcu_analog_readings;
        BMS_voltages bms_voltages{};
    public:
        // constructor
        DashboardCAN(STM32_CAN* CAN);
        // read function
        void read_CAN();
};  

#endif