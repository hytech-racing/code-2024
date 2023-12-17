#pragma once

#include <string.h>
#include <stdint.h>

#pragma pack(push,1)

// @Parseclass
class SAB_rear_potentiometers {
public:
    SAB_rear_potentiometers() = default;
    SAB_rear_potentiometers(const uint8_t buf[8]) { load(buf); }

    inline void load(const uint8_t buf[])  { memcpy(this, buf, sizeof(*this)); }
    inline void write(uint8_t buf[]) const { memcpy(buf, this, sizeof(*this)); }

    // Getters
    inline uint16_t get_pot3()                 const { return pot3; } //rl
    inline uint16_t get_pot4()                 const { return pot4; } //rr
    // inline uint16_t get_pot5()                 const { return pot5; }
    // inline uint16_t get_pot6()                 const { return pot6; }

    // Setters
    inline void set_pot3(const uint16_t value)                       { this->pot3 = value; }
    inline void set_pot4(const uint16_t value)                       { this->pot4 = value; }
    // inline void set_pot5(const uint16_t value)                       { this->pot5 = value; }
    // inline void set_pot6(const uint16_t value)                       { this->pot6 = value; }

private:
    uint16_t pot3;
    uint16_t pot4;
    // uint16_t pot6;
};

#pragma pack(pop)
