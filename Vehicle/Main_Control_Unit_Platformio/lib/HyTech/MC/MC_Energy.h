#pragma once

#include <cstdint>
#include "MC_Container.h"

class MC_Energy {
private:
    void sendMsg();

public:
    struct __attribute__((packed)) data_s {
        uint16_t dc_bus_voltage; //dc bus voltage in 1V
        int16_t torque_current; //see datasheet p81
        int16_t magnetizing_current;
    };

    MC_Container<data_s> data{};

    MC_Energy();

    void load(const uint8_t buf[]);
};
