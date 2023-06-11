#pragma once

#include <string.h>
#include <stdint.h>

#pragma pack(push,1)

// @Parseclass
class MCU_front_potentiometers {
public:
    MCU_front_potentiometers() = default;
    MCU_front_potentiometers(const uint8_t buf[8]) { load(buf); }

    inline void load(const uint8_t buf[])  { memcpy(this, buf, sizeof(*this)); }
    inline void write(uint8_t buf[]) const { memcpy(buf, this, sizeof(*this)); }

    // Getters
    inline uint16_t get_pot1()                 const { return pot1; }
    inline uint16_t get_pot2()                 const { return pot2; }
    inline uint16_t get_pot3()                 const { return pot3; }

    // Setters
    inline void set_pot1(const uint16_t value)                       { this->pot1 = value; }
    inline void set_pot2(const uint16_t value)                       { this->pot2 = value; }
    inline void set_pot3(const uint16_t value)                       { this->pot3 = value; }

private:
    uint16_t pot1;
    uint16_t pot2;
    uint16_t pot3;
};

#pragma pack(pop)
