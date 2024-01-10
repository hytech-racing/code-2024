#include <ArduinoModbus.h>
#include <ArduinoRS485.h>

/* Constant defines */
#define DEFAULT_SLAVE_ADDR  ((uint16_t)0x0C)
#define PUBLIC_ADDR         ((uint16_t)0x00)

#define TEMP_CPT_SEL_BIT    ((uint16_t)0x01)
#define TEMP_CPT_ENABLE_BIT ((uint16_t)0x01 << 1)
#define MEASURE_MODE_BIT    ((uint16_t)0x01 << 2)
#define MEASURE_TRIG_BIT    ((uint16_t)0x01 << 3)

/* Sensor register index */
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

/* Global variables */
volatile uint16_t cr = 0;             // control register command message
float dist;                           // distance variable
volatile float temp;                  // temperature variable
volatile uint16_t newAddr, res;       // address variables
volatile uint16_t baudrateIndex, res; // baudrate variables

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

/*
 * @brief read data from holding register of client
 * 
 * @param addr: address of client
 * param reg: reg index
 * return data if execute successfully, false 0xffff
 */
 uint16_t readData(uint16_t addr, eRegIndex_t reg)
 {
    uint16_t
 }
