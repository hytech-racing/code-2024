#pragma once

#include <string.h>
#include <stdint.h>

#pragma pack(push,1)

// @Parseclass
class SAB_thermistors_1 {
public:
    SAB_thermistors_1() = default;
    SAB_thermistors_1(const uint8_t buf[8]) { load(buf); }

    inline void load(const uint8_t buf[])  { memcpy(this, buf, sizeof(*this)); }
    inline void write(uint8_t buf[]) const { memcpy(buf, this, sizeof(*this)); }

    // Getters
    inline uint16_t get_thermistor3()                 const { return thermistor3; }
    inline uint16_t get_thermistor4()                 const { return thermistor4; }
    inline uint16_t get_thermistor5()                 const { return thermistor5; }
    inline uint16_t get_thermistor6()                 const { return thermistor6; }

    // Setters
    inline void set_thermistor3(const uint16_t value)                       { this->thermistor3 = value; }
    inline void set_thermistor4(const uint16_t value)                       { this->thermistor4 = value; }
    inline void set_thermistor5(const uint16_t value)                       { this->thermistor5 = value; }
    inline void set_thermistor6(const uint16_t value)                       { this->thermistor6 = value; }

private: 
    //no free bytes
    uint16_t thermistor3; //center front left thermistor
    uint16_t thermistor4; //center front right thermistor
    uint16_t thermistor5; //center rear left thermistor
    uint16_t thermistor6; //center rear right thermistor
};

#pragma pack(pop)
