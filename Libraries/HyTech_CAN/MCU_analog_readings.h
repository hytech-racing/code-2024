#pragma once

#include <string.h>
#include <stdint.h>

#pragma pack(push,1)

// @Parseclass
class MCU_analog_readings {
public:
    MCU_analog_readings() = default;
    MCU_analog_readings(const uint8_t buf[8]) { load(buf); }

    inline void load(const uint8_t buf[])  { memcpy(this, buf, sizeof(*this)); }
    inline void write(uint8_t buf[]) const { memcpy(buf, this, sizeof(*this)); }

    // Getters
    inline uint16_t get_steering_1()         const { return steering_1; }
    inline uint16_t get_steering_2()     const { return steering_2; }
    inline int16_t  get_temperature()         const { return temperature; }
    inline uint16_t get_glv_battery_voltage() const { return glv_battery_voltage; }

    // Setters
    inline void set_steering_1(const int16_t steering_1)                { this->steering_1 = steering_1; }
    inline void set_steering_2(const uint16_t steering_2)        { this->steering_2 = steering_2; }
    inline void set_temperature(const int16_t temperature)                  { this->temperature = temperature; }
    inline void set_glv_battery_voltage(const uint16_t glv_battery_voltage) { this->glv_battery_voltage = glv_battery_voltage; }

private:

    // @Parse @Unit(A) @Scale(5000)
    int16_t steering_1; // signed int, >0 is turning right, <0 is turning left, (2^14)/2 is +180 degrees out of digital sensor
	// @Parse @Unit(A) @Scale(5000)
    uint16_t steering_2; // unsigned int, 0-5V out of analog steering wheel sensor
    // @Parse @Unit(C) @Scale(100)
    int16_t temperature;
    // @Parse @Unit(V) @Scale(2500)
    uint16_t glv_battery_voltage;
};

#pragma pack(pop)