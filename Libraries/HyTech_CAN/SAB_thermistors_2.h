#pragma once

#include <string.h>
#include <stdint.h>

#pragma pack(push,1)

// @Parseclass
class SAB_thermistors_2 {
public:
    SAB_thermistors_2() = default;
    SAB_thermistors_2(const uint8_t buf[8]) { load(buf); }

    inline void load(const uint8_t buf[])  { memcpy(this, buf, sizeof(*this)); }
    inline void write(uint8_t buf[]) const { memcpy(buf, this, sizeof(*this)); }

    // Getters
    inline uint16_t get_thermistor7()                 const { return thermistor7; }
    inline uint16_t get_thermistor8()                 const { return thermistor8; }
    inline uint16_t get_thermistor9()                 const { return thermistor9; }

    // Setters
    inline void set_thermistor7(const uint16_t value)                       { this->thermistor7 = value; }
    inline void set_thermistor8(const uint16_t value)                       { this->thermistor8 = value; }
    inline void set_thermistor9(const uint16_t value)                       { this->thermistor9 = value; }

private: 
    //no free bytes
    uint16_t thermistor7; //rear forward thermistor (on left side)
    uint16_t thermistor8; //rear left thermistor
    uint16_t thermistor9; //rear right thermistor
};

#pragma pack(pop)
