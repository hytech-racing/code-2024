#pragma once

#include <cstdint>

class MCU_Pedal {
private:
    void read();

    void sendMsg();

public:
    struct __attribute__((packed)) data_s {
        uint16_t accelerator_pedal_1;
        uint16_t accelerator_pedal_2;
        uint16_t brake_transducer_1;
        uint16_t brake_transducer_2;
    };

    data_s data{};

    MCU_Pedal();

};
