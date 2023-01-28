#pragma once

#include "MC_Energy.h"
#include "MC_Temperature.h"

struct MC {
    MC_Energy energy{};
    MC_Temperature temps{};
};

extern MC mc;
