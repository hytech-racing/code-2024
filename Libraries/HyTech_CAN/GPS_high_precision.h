#pragma once
#include <stdint.h>

#pragma pack(push,1)

// @Parseclass
class GPS_high_precision {
public:
    GPS_high_precision() = default;

    GPS_high_precision(const uint8_t buf[8]) { load(buf); }

    inline void load(const uint8_t buf[8]) { memcpy(this, buf, sizeof(*this)); }
    inline void write(uint8_t buf[8]) const { memcpy(buf, this, sizeof(*this)); }

    inline int8_t get_gps_latitudeHp() const {return latitudeHp;}
    inline int8_t get_gps_longitudeHp() const {return longitudeHp;}
    inline int8_t get_gps_ellipsoidHp() const {return ellipsoidHp;}
    inline int8_t get_gps_mslHp() const {return mslHp;}
    inline int32_t get_gps_ellipsoid() const {return ellipsoid;}

    inline void set_gps_latitudeHp(int8_t reading) { latitudeHp = reading;}
    inline void set_gps_longitudeHp(int8_t reading) { longitudeHp = reading;}
    inline void set_gps_ellipsoidHp(int8_t reading) { ellipsoidHp = reading;}
    inline void set_gps_mslHp(int8_t reading) { mslHp = reading;}
    inline void set_gps_ellipsoid(int32_t reading) { ellipsoid = reading;}

private:
    int8_t latitudeHp;
    int8_t longitudeHp;
    int8_t ellipsoidHp;
    int8_t mslHp;
    int32_t ellipsoid;
};

#pragma pack(pop)