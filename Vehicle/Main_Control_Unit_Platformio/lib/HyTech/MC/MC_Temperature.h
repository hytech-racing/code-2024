#pragma once

#include <cstdint>
#include "MC_Container.h"

class MC_Temperature {
private:
    void sendMsg();

public:
    struct __attribute__((packed)) data_s {
        int16_t motor_temp;
        int16_t inverter_temp; //cold plate temp
        int16_t igbt_temp;
        int16_t diagnostic_number;
    };

    MC_Container<data_s> data{};

    MC_Temperature();

    void load(const uint8_t buf[]);
};