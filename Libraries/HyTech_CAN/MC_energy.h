#pragma once
#include <string.h>
#include <stdint.h>
#ifdef HT_DEBUG_EN
    #include "Arduino.h"
#endif

#pragma pack(push,1)

class MC_energy{
public:
  MC_energy() = default;
  MC_energy(uint8_t buf[]) { load(buf); }

  inline void load(uint8_t buf[])         { memcpy(this, buf, sizeof(*this)); }
  inline void write(uint8_t buf[])  const { memcpy(buf, this, sizeof(*this)); }

  inline uint16_t get_dc_bus_voltage()      const { return dc_bus_voltage; }
  inline uint16_t get_torque_current()      const { return torque_current; }
  inline uint16_t get_magnetizing_current()     const { return magnetizing_current; }

private:
  uint16_t dc_bus_voltage; //dc bus voltage in 1V
  int16_t torque_current; //see datasheet p81
  int16_t magnetizing_current;
}

#pragma pack(pop)
