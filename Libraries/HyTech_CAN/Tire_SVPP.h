#pragma once
#include <string.h>
#include <stdint.h>
#ifdef HT_DEBUG_EN
    #include "Arduino.h"
#endif

#pragma pack(push,1)

class Tire_SVPP{
public:
  Tire_SVPP() = default;
  Tire_SVPP(uint8_t buf[]) { load(buf); }

  inline void load(uint8_t buf[])         { memcpy(this, buf, sizeof(*this)); }
  
  inline uint16_t get_serial_number()   const { return serial_number; }
  inline uint16_t get_battery_voltage()      const { return battery_voltage; }
  inline int16_t  get_hs_pressure()      const { return hs_pressure; }
  inline uint16_t get_gauge_pressure()     const { return gauge_pressure; }
private: 
    uint16_t serial_number; //1 integer / bit
    uint16_t battery_voltage; //1 mV / bit 
    int16_t hs_pressure; // 0.1 mBar / bit + 3000 offset
    uint16_t gauge_pressure; // 1 mBar / bit
    
};

#pragma pack(pop)