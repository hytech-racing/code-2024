#pragma once

#include "MCU_Pedal.h"
#include "MCU_LoadCells.h"
#include "MCU_Status.h"
#include "MCU_rev12_dfs.h"

struct MCU {
    MCU_Pedal pedal{};
    MCU_LoadCells loadCells{};
    MCU_Status status{};
};

extern MCU mcu;