#include "Filter_IIR.h"

Filter_IIR::Filter_IIR(float alpha, uint16_t init_val) {
    set_alpha(alpha);
    this->prev_reading = init_val;
}

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
    
}