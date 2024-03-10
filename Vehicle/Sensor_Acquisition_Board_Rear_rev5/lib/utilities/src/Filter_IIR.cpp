#include "Filter_IIR.h"

void Filter_IIR::set_alpha(float alpha) {
    if (alpha > 1.0) {
        this->alpha = 1.0;
    }
    else if (alpha < 0.0) {
        this->alpha = 0.0;
    }
    else
    {
        this->alpha = alpha;
    }
}

uint16_t Filter_IIR::filtered_result(uint16_t new_val) {
    prev_reading = (1 - alpha) * new_val + alpha * prev_reading;

    return prev_reading;
}