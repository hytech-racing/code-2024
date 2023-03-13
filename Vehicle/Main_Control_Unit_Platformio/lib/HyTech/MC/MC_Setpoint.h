#pragma once

#include <cstdint>
#include "MC_Container.h"

class MC_Setpoint {
private:
    void sendMsg();

public:
    struct __attribute__((packed)) data_s {
        uint16_t control_word{0};
        int16_t speed_setpoint{0}; //in rpm
        int16_t pos_torque_limit{0}; //in 0.1% Mn
        int16_t neg_torque_limit{0};
    };

    MC_Container<data_s> data{};

    MC_Setpoint();

    inline std::vector<uint16_t> get_control_word();

    inline std::vector<bool> get_inverter_enable();

    inline std::vector<bool> get_hv_enable();

    inline std::vector<bool> get_driver_enable();

    inline std::vector<bool> get_remove_error();

    inline std::vector<uint16_t> get_speed_setpoint();

    inline std::vector<uint16_t> get_pos_torque_limit();

    inline std::vector<uint16_t> get_neg_torque_limit();

    void set_control_word(uint16_t word);

    void set_inverter_enable(bool inv_en);

    void set_hv_enable(bool hv_en);

    void set_driver_enable(bool dri_en);

    void set_remove_error(bool rem_err);

    void set_speed_setpoint(int16_t speed);

    void set_pos_torque_limit(int16_t plimit);

    void set_neg_torque_limit(int16_t nlimit);
};