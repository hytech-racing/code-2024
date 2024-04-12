#pragma once
#include <string.h>
#include <stdint.h>
#ifdef HT_DEBUG_EN
    #include "Arduino.h"
#endif

#pragma pack(push,1)

// @Parseclass @Prefix(ACU)
class ACU_shunt_measurements {
    public:
    ACU_shunt_measurements() = default;
    ACU_shunt_measurements(uint8_t buf[]) { load(buf); }

    inline void load(uint8_t buf[])         { memcpy(this, buf, sizeof(*this)); }
    inline void write(uint8_t buf[])  const { memcpy(buf, this, sizeof(*this)); }
    inline float get_shunt_voltage()         const {return shunt_voltage;}
    inline float get_shunt_current()         const {return shunt_current;}

    inline void set_shunt_voltage(float shunt_voltage) {this->shunt_voltage = (uint32_t) ((shunt_voltage - 0.0)/(0.001));}
    inline void set_shunt_current(float shunt_current) {this->shunt_current = (uint32_t) ((shunt_current - 0.0)/(0.001));}
    private:
        uint32_t shunt_voltage;
        uint32_t shunt_current;
};

#pragma pack(pop)