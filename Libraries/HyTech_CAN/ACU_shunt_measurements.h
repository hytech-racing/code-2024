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
    inline uint16_t get_shunt_voltage()         const {return shunt_voltage;}
    inline uint16_t get_shunt_current()         const {return shunt_current;}

    inline void set_shunt_voltage(uint16_t shunt_voltage) {this->shunt_voltage = shunt_voltage;}
    inline void set_shunt_current(uint16_t shunt_current) {this->shunt_current = shunt_current;}
    private:
        uint16_t shunt_voltage;
        uint16_t shunt_current;
};

#pragma pack(pop)