#pragma once
#include <string.h>
#include <stdint.h>
#ifdef HT_DEBUG_EN
    #include "Arduino.h"
#endif

#pragma pack(push,1)


class Tire_infared_temps{
public:
  Tire_infared_temps() = default;
  Tire_infared_temps(uint8_t buf[]) { load(buf); }

  inline void load(uint8_t buf[]) { memcpy(this, buf, sizeof(*this)); }
  


  inline const uint16_t* get_infared_temps() const { return infared_temps; }
  inline float get_infared_temp_1() const { return infared_temps[1] * .1 - 100; }
  inline float get_infared_temp_2() const { return infared_temps[0] * .1 - 100; }
  inline float get_infared_temp_3() const { return infared_temps[2] * .1 - 100; }
  inline float get_infared_temp_4() const { return infared_temps[3] * .1 - 100; }

private: // all temps in 0.1C with -100C offset / bit
    uint16_t infared_temps[4];
    
};

class Tire_temps {
  public:
  Tire_temps() = default;

  inline void set_tire_infared_temps_group_1(uint8_t buf[]) { this->tire_infared_temps_group_1.load(buf); }
  inline void set_tire_infared_temps_group_2(uint8_t buf[]) { this->tire_infared_temps_group_2.load(buf); }
  inline void set_tire_infared_temps_group_3(uint8_t buf[]) { this->tire_infared_temps_group_3.load(buf); }
  inline void set_tire_infared_temps_group_4(uint8_t buf[]) { this->tire_infared_temps_group_4.load(buf); }
  inline Tire_infared_temps get_tire_infared_temps_group_1() const { return tire_infared_temps_group_1; }
  inline Tire_infared_temps get_tire_infared_temps_group_2() const { return tire_infared_temps_group_2; }
  inline Tire_infared_temps get_tire_infared_temps_group_3() const { return tire_infared_temps_group_3; }
  inline Tire_infared_temps get_tire_infared_temps_group_4() const { return tire_infared_temps_group_4; }

  inline Tire_infared_temps get_tire_infared_temps(uint8_t group) const {
    if (group == 1) {
      return tire_infared_temps_group_1;
    } else if (group == 2) {
      return tire_infared_temps_group_2;
    } else if (group == 3) {
      return tire_infared_temps_group_3;
    } else if (group == 4) {
      return tire_infared_temps_group_4;
    } else {
      return nullptr;
    }
  }


private:
    Tire_infared_temps tire_infared_temps_group_1; //1 - 4
    Tire_infared_temps tire_infared_temps_group_2; //5 - 8
    Tire_infared_temps tire_infared_temps_group_3; //9 - 12
    Tire_infared_temps tire_infared_temps_group_4; //13 - 16
    };


#pragma pack(pop)

