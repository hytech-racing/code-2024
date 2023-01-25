#pragma once

#include <functional>
#include "Task_Container.h"

class MCU_Pedal {
private:
    std::function<void(uint32_t, uint8_t[8])> write_fn;

    void read();

public:
    struct data_s {
        uint16_t accelerator_pedal_1;
        uint16_t accelerator_pedal_2;
        uint16_t brake_transducer_1;
        uint16_t brake_transducer_2;
    };

    data_s data{};

    MCU_Pedal() = delete;

    MCU_Pedal(Task_Container &t, const std::function<void(uint32_t, uint8_t[8])> &write_fn);
};
