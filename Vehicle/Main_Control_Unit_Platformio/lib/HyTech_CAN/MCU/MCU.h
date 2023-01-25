#pragma once
#include <functional>

#include "MCU_Pedal.h"
#include <Task_Container.h>

class MCU {
public:
    MCU_Pedal pedal;

    MCU() = delete;
    MCU(Task_Container&,const std::function<void(uint32_t, uint8_t[8])>&, const std::function<void(uint32_t, std::function<void(uint8_t[8])>)>&);
};