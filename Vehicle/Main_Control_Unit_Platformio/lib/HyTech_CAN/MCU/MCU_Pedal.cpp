#include "MCU_Pedal.h"
#include "CAN_ID.h"
#include <Task_Container.h>

MCU_Pedal::MCU_Pedal(Task_Container &t, const std::function<void(uint32_t , uint8_t[8])> &write_fn) : write_fn(
        write_fn) {
    t.emplace(1, [this]() { read(); }, 10);
}

void MCU_Pedal::read() {

}


