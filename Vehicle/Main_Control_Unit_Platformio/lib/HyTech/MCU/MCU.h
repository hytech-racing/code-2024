#pragma once

#include "MCU_Pedal.h"
#include "MCU_LoadCells.h"
#include <Task_Container.h>

struct MCU {
    MCU_Pedal pedal{};
    MCU_LoadCells loadCells{};
};

extern MCU mcu;