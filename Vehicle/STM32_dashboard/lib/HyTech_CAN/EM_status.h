#pragma once
#include <string.h>
#include <stdint.h>
#ifdef HT_DEBUG_EN
#include "Arduino.h"
#endif

#pragma pack(push,1)

// @Parseclass
class EM_status {
public:
    EM_status() = default;
    EM_status(uint8_t buf[]) { load(buf); }

    inline void load(uint8_t buf[])           { memcpy(this, buf, sizeof(*this)); }
    inline void write(uint8_t buf[])    const { memcpy(buf, this, sizeof(*this)); }

    inline uint8_t get_voltage_gain()   const { return gain & 0xF;  }
    inline uint8_t get_current_gain()   const { return gain >> 4;   }
    inline bool get_overvoltage()       const { return flags & 0x1; }
    inline bool get_overpower()         const { return flags & 0x2; }
    inline bool get_logging()           const { return flags & 0x4; }

#ifdef HT_DEBUG_EN
    void print() {
        SerialUSB.println("\n\nEM STATUS");
		SerialUSB.println(     "---------");
		SerialUSB.print("VOLTAGE GAIN: "); SerialUSB.println(get_voltage_gain());
		SerialUSB.print("CURRENT GAIN: "); SerialUSB.println(get_current_gain());
		SerialUSB.print("OVERVOLTAGE:  "); SerialUSB.println(get_overvoltage());
		SerialUSB.print("OVERPOWER:    "); SerialUSB.println(get_overpower());
		SerialUSB.print("LOGGING:      "); SerialUSB.println(get_logging());
	}
#endif

private:
    uint8_t gain;
    uint8_t flags; //@Parse @Flaglist(overvoltage, overpower, logging)
};

#pragma pack(pop) 