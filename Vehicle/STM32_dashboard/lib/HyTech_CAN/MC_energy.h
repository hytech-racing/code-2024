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
  inline int32_t get_actual_power()      const { return actual_power; }
  inline int16_t get_feedback_torque()      const { return feedback_torque; }

private:
  uint16_t dc_bus_voltage; //dc bus voltage in 1V
  uint32_t actual_power;//power calculated from motor torque and speed, in 1W
  int16_t feedback_torque;// torque feedback value in units 0.1% nominal torque (9.8Nm)
};

#pragma pack(pop)
