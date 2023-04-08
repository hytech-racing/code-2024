#pragma once
#include <stdint.h>

#pragma pack(push,1)

// @Parseclass
class GPS_other {
public:
    GPS_other() = default;

    GPS_other(const uint8_t buf[8]) { load(buf); }

    inline void load(const uint8_t buf[8]) { memcpy(this, buf, sizeof(*this)); }
    inline void write(uint8_t buf[8]) const { memcpy(buf, this, sizeof(*this)); }

    inline int32_t get_gps_msl() const {return msl;}
    inline uint32_t get_gps_accuracy() const {return accuracy;}

    inline void set_gps_msl(int32_t reading) { msl = reading;}
    inline void set_gps_accuracy(uint32_t reading) { accuracy = reading;}

private:
    int32_t msl;
    uint32_t accuracy;
};

#pragma pack(pop)