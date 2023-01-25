#include "MCU.h"

MCU::MCU(Task_Container &t, const std::function<void(uint32_t, uint8_t[8])> &write_fn,
         const std::function<void(uint32_t, std::function<void(uint8_t[8])>)> &register_load_fn) : pedal(t, write_fn) {}