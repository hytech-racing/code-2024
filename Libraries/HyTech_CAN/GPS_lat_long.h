#pragma once
#include <stdint.h>

#pragma pack(push,1)

// @Parseclass
class GPS_lat_long {
public:
    GPS_lat_long() = default;

    GPS_lat_long(const uint8_t buf[8]) { load(buf); }

    inline void load(const uint8_t buf[8]) { memcpy(this, buf, sizeof(*this)); }
    inline void write(uint8_t buf[8]) const { memcpy(buf, this, sizeof(*this)); }

    inline int32_t get_gps_latitude() const {return latitude;}
    inline int32_t get_gps_longitude() const {return longitude;}

    inline void set_gps_latitude(int32_t reading) { latitude = reading;}
    inline void set_gps_longitude(int32_t reading) { longitude = reading;}

private:
    int32_t latitude;
    int32_t longitude;
};

#pragma pack(pop)