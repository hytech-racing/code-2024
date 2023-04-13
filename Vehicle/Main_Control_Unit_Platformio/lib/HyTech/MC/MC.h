#pragma once

#include "MC_Energy.h"
#include "MC_Temperature.h"
#include "MC_Setpoint.h"
#include "MC_Status.h"

struct MC {
    MC_Energy energy{};
    MC_Temperature temps{};
    MC_Setpoint setpoint{};
    MC_Status status{};
};

extern MC mc;
