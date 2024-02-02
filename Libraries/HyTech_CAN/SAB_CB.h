#pragma once

#include <string.h>
#include <stdint.h>

#pragma pack(push,1)

// @Parseclass
class SAB_CB {
public:
    SAB_CB() = default;
    SAB_CB(const uint8_t buf[8]) { load(buf); }

    inline void load(const uint8_t buf[])  { memcpy(this, buf, sizeof(*this)); }
    inline void write(uint8_t buf[]) const { memcpy(buf, this, sizeof(*this)); }

    // Getters
    inline uint16_t get_RL_load_cell()         const { return RL_load_cell; }
    inline uint16_t get_RR_load_cell()         const { return RR_load_cell; }
    inline uint16_t get_pot3()                 const { return pot3; } //rl
    inline uint16_t get_pot4()                 const { return pot4; } //rr


    // Setters
    inline void set_pot3(const uint16_t value)                       { this->pot3 = value; }
    inline void set_pot4(const uint16_t value)                       { this->pot4 = value; }
    inline void set_RL_load_cell(const uint16_t load)                { this->RL_load_cell = load; }
    inline void set_RR_load_cell(const uint16_t load)                { this->RR_load_cell = load; }


private:
    uint16_t pot3;
    uint16_t pot4;
    
    uint16_t RL_load_cell; //in lbs
    uint16_t RR_load_cell; //in lbs
};

#pragma pack(pop)
