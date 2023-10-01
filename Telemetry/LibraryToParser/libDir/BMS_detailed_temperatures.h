#pragma once
#include <string.h>
#include <stdint.h>
#ifdef HT_DEBUG_EN
    #include "Arduino.h"
#endif

#pragma pack(push,1)

/**
 * @Parse BMS detailed temperatures
 * @Id 0xDA
 * @Length 64
 */
class BMS_detailed_temperatures {
public:
    BMS_detailed_temperatures() = default;
    BMS_detailed_temperatures(uint8_t buf[]) { load(buf); }
    BMS_detailed_temperatures(uint8_t ic_id, uint8_t group_id, int16_t temperature_0, int16_t temperature_1, int16_t temperature_2) {
        set_group_id(group_id);
        set_ic_id(ic_id);
        set_temperature_0(temperature_0);
        set_temperature_1(temperature_1);
        set_temperature_2(temperature_2);
    }

    inline void load(uint8_t buf[])         { memcpy(this, buf, sizeof(*this)); }
    inline void write(uint8_t buf[])  const { memcpy(buf, this, sizeof(*this)); }

    inline uint8_t get_ic_id()         const  { return group_id_ic_id & 0xF; }
    inline uint8_t get_group_id()      const  { return group_id_ic_id >> 4;}
    inline int16_t get_temperature_0() const  { return temperature_0; }
    inline int16_t get_temperature_1() const  { return temperature_1; }
    inline int16_t get_temperature_2() const  { return temperature_2; }
    int16_t get_temperature(uint8_t temperature_id) {
        switch (temperature_id) {
            case 0: return temperature_0;
            case 1: return temperature_1;
            case 2: return temperature_2;
        }
        return 0;
    }

    inline void set_ic_id(uint8_t ic_id)                 { this->group_id_ic_id = (group_id_ic_id & 0xF0) | ic_id; }
    inline void set_group_id(uint8_t group_id)           { this->group_id_ic_id = (group_id << 4) | (group_id_ic_id & 0xF); }
    inline void set_temperature_0(uint16_t temperature_0) { this->temperature_0 = temperature_0; }
    inline void set_temperature_1(uint16_t temperature_1) { this->temperature_1 = temperature_1; }
    inline void set_temperature_2(uint16_t temperature_2) { this->temperature_2 = temperature_2; }
    void set_temperature(uint8_t temperature_id, int16_t temperature) {
        switch (temperature_id) {
            case 0: this->temperature_0 = temperature; return;
            case 1: this->temperature_1 = temperature; return;
            case 2: this->temperature_2 = temperature; return;
        }
    }

#ifdef HT_DEBUG_EN
    void print() {
        Serial.println("\n\nBMS DETAILED TEMPERATURES");
        Serial.println    ("-------------------------");
        Serial.print("IC:            ");    Serial.println((uint32_t) ic_id);
        Serial.print("TEMPERATURE 0: ");    Serial.println(temperature_0 / 100., 2);
        Serial.print("TEMPERATURE 1: ");    Serial.println(temperature_1 / 100., 2);
        Serial.print("TEMPERATURE 2: ");    Serial.println(temperature_2 / 100., 2);
    }
#endif

private:
    /**
     * @Signal @Name(ic) @Path(BMS.detailed_temps.ic) @Bits(0,3) @Unsigned
     * @Signal @Name(group) @Path(BMS.detailed_temps.group) @Bits(4,7) @Unsigned
     */
    uint8_t group_id_ic_id;
    /**
     * @Signal @Name(temps[0]) @Path(BMS.detailed_temps.temps[0]) @Bits(8,23) @Signed @Transformation(x/100) @Units(C)
     */
    int16_t temperature_0;
    /**
     * @Signal @Name(temps[1]) @Path(BMS.detailed_temps.temps[1]) @Bits(24,39) @Signed @Transformation(x/100) @Units(C)
     */
    int16_t temperature_1;
    /**
     * @Signal @Name(temps[2]) @Path(BMS.detailed_temps.temps[2]) @Bits(40,55) @Signed @Transformation(x/100) @Units(C)
     */
    int16_t temperature_2;
};

#pragma pack(pop)
