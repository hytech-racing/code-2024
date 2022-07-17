#pragma once
#include <string.h>
#include <stdint.h>
#ifdef HT_DEBUG_EN
    #include "Arduino.h"
#endif

#pragma pack(push,1)

class MC_torque_command{
public:
  MC_torque_command() = default;
  MC_torque_command(uint8_t buf[]) { load(buf); }

  inline void load(uint8_t buf[])         { memcpy(this, buf, sizeof(*this)); }
  inline void write(uint8_t buf[])  const { memcpy(buf, this, sizeof(*this)); }

  inline int16_t get_torque_setpoint()   const { return torque_setpoint; }

  inline void set_torque_setpoint(const int16_t torque) {torque_setpoint = torque;}

private:
    int16_t torque_setpoint; //in 0.1% Mn

}

#pragma pack(pop)
