/**************************************************************************************************************
     This code tests the range finder function of the URM14 ultrasonic sensor
     @ author : roker.wang@dfrobot.com
     @ data   : 11.08.2020
     @ version: 1.0
**************************************************************************************************************/
#include <ArduinoModbus.h>
#include <ArduinoRS485.h>

#define   SLAVE_ADDR                ((uint16_t)0x0C)

#define   TEMP_CPT_SEL_BIT          ((uint16_t)0x01)
#define   TEMP_CPT_ENABLE_BIT       ((uint16_t)0x01 << 1)
#define   MEASURE_MODE_BIT          ((uint16_t)0x01 << 2)
#define   MEASURE_TRIG_BIT          ((uint16_t)0x01 << 3)


typedef enum{ 
  ePid,
  eVid,
  eAddr,
  eComBaudrate,
  eComParityStop,
  eDistance,
  eInternalTempreture,
  eExternTempreture,
  eControl,
  eNoise
}eRegIndex_t;//Sensor register index

/*
 *@brief Read data from holding register of client
 *
 *@param addr ： Address of Client
 *@param reg: Reg index
 *@return data if execute successfully, false oxffff.
 */
uint16_t readData(uint16_t addr, eRegIndex_t reg)
{
  uint16_t data;
  if (!ModbusRTUClient.requestFrom(addr, HOLDING_REGISTERS, reg, 1)){
    Serial.print("failed to read registers! ");
    Serial.println(ModbusRTUClient.lastError());
    data = 0xffff;
  }else{
    data =  ModbusRTUClient.read();
  }
  return data;
}

/*
 *@brief write data to holding register of client 
 *
 *@param addr ： Address of Client
 *@param reg: Reg index
 *@param data: The data to be written
 *@return 1 if execute successfully, false 0.
 */
uint16_t writeData(uint16_t addr, eRegIndex_t reg, uint16_t data)
{
  if (!ModbusRTUClient.holdingRegisterWrite(addr, reg, data)){
    Serial.print("Failed to write coil! ");
    Serial.println(ModbusRTUClient.lastError());
    return 0;
  }else
    return 1;
}

float  dist;
volatile uint16_t cr = 0;
void setup() {
  ModbusRTUClient.begin(115200);
  Serial.begin(9600);
  cr |= MEASURE_MODE_BIT;//Set bit2 , Set to trigger mode
  cr &= ~(uint16_t)TEMP_CPT_SEL_BIT;//Select internal temperature compensation
  cr &= ~(uint16_t)TEMP_CPT_ENABLE_BIT;//enable temperature compensation
  writeData(SLAVE_ADDR, eControl, cr); //Writes the setting value to the control register
  delay(100);
}

void loop() {
  cr |= MEASURE_TRIG_BIT;//Set trig bit
  writeData(SLAVE_ADDR, eControl, cr); //Write the value to the control register and trigger a ranging
  delay(50);//Delay of 300ms(minimum delay should be greater than 30ms) is to wait for the completion of ranging
  dist = (float)readData(SLAVE_ADDR, eDistance) / 10;//Read distance register, one LSB is 0.1mm

  Serial.print("distance = ");
  Serial.print(dist, 1);
  Serial.println("mm");
}
