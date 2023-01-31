#include "MC_Setpoint.h"
#include <Task_Container.h>
#include "CAN_ID.h"
#include "CAN_Decl.h"

MC_Setpoint::MC_Setpoint() {
    taskContainer.emplace("MC Setpoint CAN", INSTANCE_FUNC(sendMsg), 5);
}

void MC_Setpoint::sendMsg() {
    uint32_t id = ID_MC1_SETPOINTS_COMMAND;
    for (auto &i: data.iter()) {
        CAN_message_t msg;
        msg.id = id;
        msg.len = sizeof(i.get());
        memcpy(msg.buf, &(i.get()), msg.len);
        if (id < ID_MC3_TEMPS) {
            front_inv_can.write(msg);
        } else {
            rear_inv_can.write(msg);
        }
        ++id;
    }
}

std::vector<uint16_t> MC_Setpoint::get_control_word() {
    std::vector<uint16_t> vec;
    for (auto const &i : data.iter()) {
        vec.push_back(i.get().control_word);
    }
    return vec;
}

std::vector<bool> MC_Setpoint::get_inverter_enable() {
    std::vector<bool> vec;
    for (auto const &i : data.iter()) {
        vec.push_back(i.get().control_word & 0x100);
    }
    return vec;
}

std::vector<bool> MC_Setpoint::get_hv_enable() {
    std::vector<bool> vec;
    for (auto const &i : data.iter()) {
        vec.push_back(i.get().control_word & 0x200);
    }
    return vec;
}

std::vector<bool> MC_Setpoint::get_driver_enable() {
    std::vector<bool> vec;
    for (auto const &i : data.iter()) {
        vec.push_back(i.get().control_word & 0x400);
    }
    return vec;
}

std::vector<bool> MC_Setpoint::get_remove_error() {
    std::vector<bool> vec;
    for (auto const &i : data.iter()) {
        vec.push_back(i.get().control_word & 0x800);
    }
    return vec;
}

std::vector<uint16_t> MC_Setpoint::get_speed_setpoint() {
    std::vector<uint16_t> vec;
    for (auto const &i : data.iter()) {
        vec.push_back(i.get().speed_setpoint);
    }
    return vec;
}

std::vector<uint16_t> MC_Setpoint::get_pos_torque_limit() {
    std::vector<uint16_t> vec;
    for (auto const &i : data.iter()) {
        vec.push_back(i.get().pos_torque_limit);
    }
    return vec;
}

std::vector<uint16_t> MC_Setpoint::get_neg_torque_limit() {
    std::vector<uint16_t> vec;
    for (auto const &i : data.iter()) {
        vec.push_back(i.get().neg_torque_limit);
    }
    return vec;
}

void MC_Setpoint::set_control_word(const uint16_t word) {
    for (auto const &i : data.iter()) {
        i.get().control_word = word;
    }
}

void MC_Setpoint::set_inverter_enable(const bool inv_en) {
    for (auto const &i : data.iter()) {
        i.get().control_word = (i.get().control_word & 0xFEFF) | (inv_en << 8);
    }
}

void MC_Setpoint::set_hv_enable(const bool hv_en) {
    for (auto const &i : data.iter()) {
        i.get().control_word = (i.get().control_word & 0xFDFF) | (hv_en << 9);
    }
}

void MC_Setpoint::set_driver_enable(const bool dri_en) {
    for (auto const &i : data.iter()) {
        i.get().control_word = (i.get().control_word & 0xFBFF) | (dri_en << 10);
    }
}

void MC_Setpoint::set_remove_error(const bool rem_err) {
    for (auto const &i : data.iter()) {
        i.get().control_word = (i.get().control_word & 0xF7FF) | (rem_err << 11);
    }
}

void MC_Setpoint::set_speed_setpoint(const int16_t speed) {
    for (auto const &i : data.iter()) {
        i.get().speed_setpoint = speed;
    }
}

void MC_Setpoint::set_pos_torque_limit(const int16_t plimit) {
    for (auto const &i : data.iter()) {
        i.get().pos_torque_limit = plimit;
    }
}

void MC_Setpoint::set_neg_torque_limit(const int16_t nlimit) {
    for (auto const &i : data.iter()) {
        i.get().neg_torque_limit = nlimit;
    }
}
