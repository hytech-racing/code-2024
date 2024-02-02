#pragma once

#include <string.h>
#include <stdint.h>

#pragma pack(push,1)

class SAB_ride_heights {
public:
    SAB_ride_heights() = default;
    SAB_ride_heights(const uint8_t buf[8]) { load(buf); }

    inline void load(const uint8_t buf[])  { memcpy(this, buf, sizeof(*this)); }
    inline void write(uint8_t buf[]) const { memcpy(buf, this, sizeof(*this)); }

    // Getters
    inline uint16_t get_FL_ride_height() const { return FL_ride_height; }
    inline uint16_t get_FR_ride_height() const { return FR_ride_height; }
    inline uint16_t get_RL_ride_height() const { return RL_ride_height; }
    inline uint16_t get_RR_ride_height() const { return RR_ride_height; }

    // Setters
    inline void set_FL_ride_height(const uint16_t distance)  { this->FL_ride_height = distance; }
    inline void set_FR_ride_height(const uint16_t distance)  { this->FR_ride_height = distance; }
    inline void set_RL_ride_height(const uint16_t distance)  { this->RL_ride_height = distance; }
    inline void set_RR_ride_height(const uint16_t distance)  { this->RR_ride_height = distance; }


private:
    uint16_t FL_ride_height;    // @Parse @Unit(mm) @Scale(10) @Name(Front left ride height)
    uint16_t FR_ride_height;    // @Parse @Unit(mm) @Scale(10) @Name(Front right ride height)
    uint16_t RL_ride_height;    // @Parse @Unit(mm) @Scale(10) @Name(Rear left ride height)
    uint16_t RR_ride_height;    // @Parse @Unit(mm) @Scale(10) @Name(Rear right ride height)
};

#pragma pack(pop)