#pragma once
#include <string.h>
#include <stdint.h>
#ifdef HT_DEBUG_EN
    #include "Arduino.h"
#endif

#pragma pack(push,1)

class Tire_sensor_info{
public:
  Tire_sensor_info() = default;
  Tire_sensor_info(uint8_t buf[]) { load(buf); }

  inline void load(uint8_t buf[])         { memcpy(this, buf, sizeof(*this)); }
  
  inline uint16_t get_transmission_count()   const { return transmission_count; }
  inline int16_t get_rssi()      const { return rssi; }
  inline uint16_t  get_sensor_temp()      const { return sensor_temp; }
  inline uint16_t get_sensor_id()     const { return sensor_id; }
private: 
    uint16_t transmission_count; // 1 integer / bit
    int16_t rssi; // 1 dBm / bit
    uint16_t sensor_temp; //temps in 0.1C with -100C offset
    uint16_t sensor_id;
    
};

#pragma pack(pop)