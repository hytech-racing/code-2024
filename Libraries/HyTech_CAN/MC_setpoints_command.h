#pragma once
#include <string.h>
#include <stdint.h>
#ifdef HT_DEBUG_EN
    #include "Arduino.h"
#endif

#pragma pack(push,1)

class MC_setpoints_command{
public:
  MC_setpoints_command() = default;
  MC_setpoints_command(uint8_t buf[]) { load(buf); }

  inline void load(uint8_t buf[])         { memcpy(this, buf, sizeof(*this)); }
  inline void write(uint8_t buf[])  const { memcpy(buf, this, sizeof(*this)); }

  inline uint16_t get_control_word()   const { return control_word; }
  inline bool get_inverter_enable()   const{ return control_word & 0x100; }
  inline bool get_hv_enable()   const{ return control_word & 0x200; }
  inline bool get_driver_enable()   const{ return control_word & 0x400; }
  inline bool get_remove_error()   const{ return control_word & 0x800; }
  inline int16_t get_speed_setpoint()   const { return speed_setpoint; }
  inline int16_t get_pos_torque_limit()   const { return pos_torque_limit; }
  inline int16_t get_neg_torque_limit()   const { return neg_torque_limit; }

  inline void set_control_word(const uint16_t word)   { control_word = word; }
  inline void set_inverter_enable(const bool inv_en)   { control_word  = (control_word & 0xFEFF) | (inv_en << 8); }
  inline void set_hv_enable(const bool hv_en)   { control_word  = (control_word & 0xFDFF) | (hv_en << 9); }
  inline void set_driver_enable(const bool dri_en)   { control_word  = (control_word & 0xFBFF) | (dri_en << 10); }
  inline void set_remove_error(const bool rem_err)   { control_word  = (control_word & 0xF7FF) | (rem_err << 11); }
  inline void set_speed_setpoint(const int16_t speed)    { speed_setpoint = speed; }
  inline void set_pos_torque_limit(const int16_t plimit)   { pos_torque_limit = plimit; }
  inline void set_neg_torque_limit(const int16_t nlimit)   { neg_torque_limit = nlimit;}

private:
    uint16_t control_word;
    int16_t speed_setpoint; //in rpm
    int16_t pos_torque_limit; //in 0.1% Mn
    int16_t neg_torque_limit;
};

#pragma pack(pop)
