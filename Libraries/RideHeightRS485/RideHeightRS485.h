#ifndef _RIDE_HEIGHT_RS485_H_
#define _RIDE_HEIGHT_RS485_H_

#include <stdint.h>

/* SAB pinout */
// #define RS485_SERIAL_PORT Serial4
// #define RS485_TX_PIN 17
// #define RS485_DE_PIN 18
/* test board pinout */
#define RS485_SERIAL_PORT Serial1
#define RS485_TX_PIN 1
#define RS485_DE_PIN 2
#define RS485_RE_PIN -1    // set as -1 to never bother with it, otherwise set as corresponding pin in use

#include <ArduinoModbus.h>
#include <ArduinoRS485.h>

#define   DEFAULT_SLAVE_ADDR        ((uint16_t)0x0C)
#define   PUBLIC_ADDR               ((uint16_t)0x00)

#define   TEMP_CPT_SEL_BIT          ((uint16_t)0x01)
#define   TEMP_CPT_ENABLE_BIT       ((uint16_t)0x01 << 1)
#define   MEASURE_MODE_BIT          ((uint16_t)0x01 << 2)
#define   MEASURE_TRIG_BIT          ((uint16_t)0x01 << 3)

//Sensor register index
typedef enum { 
  ePid,
  eVid,
  eAddr,
  eComBaudrate,
  eComParityStop,
  eDistance,
  eInternalTemperature,
  eExternTemperature,
  eControl,
  eNoise
} eRegIndex_t;

class RideHeightRS485
{
public:
    RideHeightRS485(uint32_t baudrate);
    void init(uint32_t baudrate);
    void begin();
    uint16_t readData(uint16_t addr, eRegIndex_t reg);
    uint16_t writeData(uint16_t addr, eRegIndex_t reg, uint16_t data);
    float readDistance(uint16_t addr=DEFAULT_SLAVE_ADDR);
    float readTemp(uint16_t addr=DEFAULT_SLAVE_ADDR);
    void setAddress(uint16_t currAddr=PUBLIC_ADDR, uint16_t newAddr=DEFAULT_SLAVE_ADDR);
    void setBaudrate(uint16_t addr=PUBLIC_ADDR, uint16_t baudrateIndex=8);

private:
    static uint32_t baudrates[8];

    uint16_t cr = 0;       // command word

    float dist;            // distance reading
    float temp;            // temperature reading
    uint32_t baudrate;     //0x0001---2400   0x0002---4800  0x0003---9600   0x0004---14400
                           //0x0005---19200  0x0006---38400 0x0007---57600  0x0008---115200  Other----115200

};

#endif
#pragma once