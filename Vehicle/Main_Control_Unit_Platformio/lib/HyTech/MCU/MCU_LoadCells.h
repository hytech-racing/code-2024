#pragma once

#include <cstdint>

class MCU_LoadCells {
private:
    void read();

    void sendMsg();

public:
    struct __attribute__((packed)) data_s {
        uint16_t FL_load_cell;
        uint16_t FR_load_cell;
        uint16_t RL_load_cell;
        uint16_t RR_load_cell;
    };

    data_s data{};

    MCU_LoadCells();

};
