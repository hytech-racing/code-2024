#pragma once

#include <cstdint>
#include "MC_Container.h"

class MC_Status {
private:
    void sendMsg();

public:
    struct __attribute__((packed)) data_s {
        uint16_t status_word;
        int16_t speed; //speed in rpm
        int16_t torque; //torque in 0.1% mn
        int16_t torque_current; //torque current in 0.1A
    };

    MC_Container<data_s> data{};

    MC_Status();

    void load(const uint8_t buf[]);

    std::vector<uint16_t> get_status_word();
    std::vector<uint16_t> get_speed();
    std::vector<uint16_t> get_torque();
    std::vector<uint16_t> get_torque_current();

    std::vector<bool> get_system_ready();
    std::vector<bool> get_error();
    std::vector<bool> get_warning();
    std::vector<bool> get_quit_dc_on();
    std::vector<bool> get_dc_on();
    std::vector<bool> get_quit_inverter_on();
    std::vector<bool> get_inverter_on();
    std::vector<bool> get_derating_on();
};
