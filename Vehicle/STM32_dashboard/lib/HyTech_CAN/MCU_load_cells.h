#pragma once

#include <string.h>
#include <stdint.h>

#pragma pack(push,1)

// @Parseclass
class MCU_load_cells {
public:
    MCU_load_cells() = default;
    MCU_load_cells(const uint8_t buf[8]) { load(buf); }

    inline void load(const uint8_t buf[])  { memcpy(this, buf, sizeof(*this)); }
    inline void write(uint8_t buf[]) const { memcpy(buf, this, sizeof(*this)); }

    // Getters
    inline uint16_t get_FL_load_cell()         const { return FL_load_cell; }
    inline uint16_t get_FR_load_cell()         const { return FR_load_cell; }
    inline uint16_t get_RL_load_cell()         const { return RL_load_cell; }
    inline uint16_t get_RR_load_cell()         const { return RR_load_cell; }

    // Setters
    inline void set_FL_load_cell(const uint16_t load)                { this->FL_load_cell = load; }
    inline void set_FR_load_cell(const uint16_t load)                { this->FR_load_cell = load; }
    inline void set_RL_load_cell(const uint16_t load)                { this->RL_load_cell = load; }
    inline void set_RR_load_cell(const uint16_t load)                { this->RR_load_cell = load; }


private:
    uint16_t FL_load_cell; //in lbs
    uint16_t FR_load_cell; //in lbs
    uint16_t RL_load_cell; //in lbs
    uint16_t RR_load_cell; //in lbs
};

#pragma pack(pop)
