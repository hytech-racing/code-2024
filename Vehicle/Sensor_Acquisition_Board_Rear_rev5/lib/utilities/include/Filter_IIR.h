/* IIR digital low pass filter */
#ifndef __FILTER_IIR__
#define __FILTER_IIR__

#include <stdint.h>

#define DEFAULT_ALPHA 0.0

class Filter_IIR
{

public:
    /**
     * Constructors
     */
    Filter_IIR(float alpha, uint16_t init_val=0) {
        set_alpha(alpha);
        prev_reading = init_val;
    }
    Filter_IIR() {
        Filter_IIR(DEFAULT_ALPHA);
    }

    void set_alpha(float alpha);
    uint16_t get_prev_reading() const {return prev_reading;}

    uint16_t filtered_result(uint16_t new_val);
    
private:    
    float alpha;
    uint16_t prev_reading;
};

#endif
#pragma once