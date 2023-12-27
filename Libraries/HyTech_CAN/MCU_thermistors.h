#pragma once

#include <string.h>
#include <stdint.h>

#pragma pack(push,1)

// @Parseclass
class MCU_thermistors {
public:
    MCU_thermistors() = default;
    MCU_thermistors(const uint8_t buf[8]) { load(buf); }

    inline void load(const uint8_t buf[])  { memcpy(this, buf, sizeof(*this)); }
    inline void write(uint8_t buf[]) const { memcpy(buf, this, sizeof(*this)); }

    // Getters
    inline uint16_t get_thermistor1()                 const { return thermistor1; }
    inline uint16_t get_thermistor2()                 const { return thermistor2; }

    // Setters
    inline void set_thermistor1(const uint16_t value)                       { this->thermistor1 = value; }
    inline void set_thermistor2(const uint16_t value)                       { this->thermistor2 = value; }

private:
    uint16_t thermistor1; //front left thermistor
    uint16_t thermistor2; //front right thermistor
};

#pragma pack(pop)
