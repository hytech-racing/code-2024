/* IIR digital low pass filter */
#ifndef __FILTER_IIR__
#define __FILTER_IIR__

#include <stdint.h>

class Filter_IIR
{

public:
    Filter_IIR(float alpha, uint16_t init_val=0);
    void set_alpha(float alpha);
    uint16_t filtered_result(uint16_t new_val);
    
private:    
    float alpha;
    uint16_t prev_reading;
};

#endif
#pragma once