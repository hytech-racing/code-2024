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

    //inline uint8_t get_voltage_gain()   const { return gain & 0xF;  }
    inline uint8_t get_voltage_gain()   const { return ((gain & 0x1F) >> 1); }  // Modification per DBC file, yet to be verified
    //inline uint8_t get_current_gain()   const { return gain >> 4;   }
    inline uint8_t get_current_gain()   const { return (((gain & 0x01) << 3) | ((flags & 0xE0) >> 5)); }
    //inline bool get_overvoltage()       const { return flags & 0x1; }
    inline bool get_overvoltage()       const { return flags & 0x80; }
    //inline bool get_overpower()         const { return flags & 0x2; }
    inline bool get_overpower()         const { return flags & 0x40; }
    //inline bool get_logging()           const { return flags & 0x4; }
    inline bool get_logging()           const { return flags & 0x20; }

#ifdef HT_DEBUG_EN
    void print() {
        Serial.println("\n\nEM STATUS");
		Serial.println(     "---------");
		Serial.print("VOLTAGE GAIN: "); Serial.println(get_voltage_gain());
		Serial.print("CURRENT GAIN: "); Serial.println(get_current_gain());
		Serial.print("OVERVOLTAGE:  "); Serial.println(get_overvoltage());
		Serial.print("OVERPOWER:    "); Serial.println(get_overpower());
		Serial.print("LOGGING:      "); Serial.println(get_logging());
	}
#endif

private:
    uint8_t gain;
    uint8_t flags; //@Parse @Flaglist(overvoltage, overpower, logging)
    uint16_t dummy; // Compensate 4-byte data frame
};

#pragma pack(pop) 