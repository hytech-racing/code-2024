#include "MC_Status.h"
#include "CAN_ID.h"
#include "CAN_Decl.h"

MC_Status::MC_Status() {

}

void MC_Status::load(const uint8_t *buf) {
    memcpy(&data, buf, sizeof(*this));
}

std::vector<uint16_t> MC_Status::get_status_word() {
    std::vector<uint16_t> vec;
    for (auto const &i : data.iter()) {
        vec.push_back(i.get().status_word);
    }
    return vec;
}

std::vector<uint16_t> MC_Status::get_speed() {
    std::vector<uint16_t> vec;
    for (auto const &i : data.iter()) {
        vec.push_back(i.get().speed);
    }
    return vec;
}

std::vector<uint16_t> MC_Status::get_torque() {
    std::vector<uint16_t> vec;
    for (auto const &i : data.iter()) {
        vec.push_back(i.get().torque);
    }
    return vec;
}

std::vector<uint16_t> MC_Status::get_torque_current() {
    std::vector<uint16_t> vec;
    for (auto const &i : data.iter()) {
        vec.push_back(i.get().torque_current);
    }
    return vec;
}

std::vector<bool> MC_Status::get_system_ready() {
    std::vector<bool> vec;
    for (auto const &i : data.iter()) {
        vec.push_back(i.get().status_word & 0x100);
    }
    return vec;
}

std::vector<bool> MC_Status::get_error() {
    std::vector<bool> vec;
    for (auto const &i : data.iter()) {
        vec.push_back(i.get().status_word & 0x200);
    }
    return vec;
}

std::vector<bool> MC_Status::get_warning() {
    std::vector<bool> vec;
    for (auto const &i : data.iter()) {
        vec.push_back(i.get().status_word & 0x400);
    }
    return vec;
}

std::vector<bool> MC_Status::get_quit_dc_on() {
    std::vector<bool> vec;
    for (auto const &i : data.iter()) {
        vec.push_back(i.get().status_word & 0x800);
    }
    return vec;
}

std::vector<bool> MC_Status::get_dc_on() {
    std::vector<bool> vec;
    for (auto const &i : data.iter()) {
        vec.push_back(i.get().status_word & 0x1000);
    }
    return vec;
}

std::vector<bool> MC_Status::get_quit_inverter_on() {
    std::vector<bool> vec;
    for (auto const &i : data.iter()) {
        vec.push_back(i.get().status_word & 0x2000);
    }
    return vec;
}

std::vector<bool> MC_Status::get_derating_on() {
    std::vector<bool> vec;
    for (auto const &i : data.iter()) {
        vec.push_back(i.get().status_word & 0x4000);
    }
    return vec;
}

std::vector<bool> MC_Status::get_inverter_on() {
    std::vector<bool> vec;
    for (auto const &i : data.iter()) {
        vec.push_back(i.get().status_word & 0x8000);
    }
    return vec;
}
