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

    // Getters
    inline uint16_t get_shunt_current()         const {return shunt_current;}
    inline uint16_t get_pack_filtered()         const {return pack_filtered;}
    inline uint16_t get_ts_out_filtered()         const {return ts_out_filtered;}

    // Setters
    inline void set_shunt_current(uint16_t shunt_current) {this->shunt_current = (uint16_t) (shunt_current);}
    inline void set_pack_filtered(uint16_t pack_filtered) {this->pack_filtered = (uint16_t) (pack_filtered);}
    inline void set_ts_out_filtered(uint16_t ts_out_filtered) {this->ts_out_filtered = (uint16_t) (ts_out_filtered);}

    // Member variables
    private:
        uint16_t shunt_current;
        uint16_t pack_filtered;
        uint16_t ts_out_filtered;
};

#pragma pack(pop)
