#include "RideHeightRS485.h"

// static baudrate array
uint32_t RideHeightRS485::baudrates[8] = {2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200};

RideHeightRS485::RideHeightRS485(uint32_t baudrate)
{
    init(baudrate);
}

void RideHeightRS485::init(uint32_t baudrate)
{
    this->baudrate = baudrate;
}

void RideHeightRS485::begin()
{
    ModbusRTUClient.begin(baudrate);

    cr |= MEASURE_MODE_BIT;                 //Set bit 2 , Set to trigger mode
    cr &= ~(uint16_t)TEMP_CPT_SEL_BIT;      //Select internal temperature compensation
    cr &= ~(uint16_t)TEMP_CPT_ENABLE_BIT;   //enable temperature compensation

    writeData(PUBLIC_ADDR, eControl, cr);    //Writes the setting value to the control register for all sensors

    delay(100);
}

/*
 *@brief Read data from holding register of client
 *
 *@param addr ： Address of Client
 *@param reg: Reg index
 *@return data if execute successfully, false oxffff.
 */
uint16_t RideHeightRS485::readData(uint16_t addr, eRegIndex_t reg)
{
    uint16_t data;
    if (!ModbusRTUClient.requestFrom(addr, HOLDING_REGISTERS, reg, 1))
    {
        Serial.print("failed to read registers! ");
        Serial.println(ModbusRTUClient.lastError());
        data = 0xffff;
    }
    else
    {
        data =  ModbusRTUClient.read();
    }

    return data;
}

/*
 *@brief write data to holding register of client 
 *
 *@param addr： Address of Client
 *@param reg: Reg index
 *@param data: The data to be written
 *@return 1 if execute successfully, false 0.
 */
uint16_t RideHeightRS485::writeData(uint16_t addr, eRegIndex_t reg, uint16_t data)
{
    if (!ModbusRTUClient.holdingRegisterWrite(addr, reg, data))
    {
        Serial.print("Failed to write coil! ");
        Serial.println(ModbusRTUClient.lastError());

        return 0;
    }
    else
    {
        return 1;
    }        
}

/*
*@brief Read the distance measurement readings from target sensor
*
*@param addr: address of the sensor to be read from
*@return distance read
*/
float RideHeightRS485::readDistance(uint16_t addr)
{
    cr |= MEASURE_TRIG_BIT;           //Set trig bit

    writeData(addr, eControl, cr);    //Write the value to the control register and trigger a ranging
    delay(60);                        //Delay of 300ms(minimum delay should be greater than 30ms) is to wait for the completion of ranging

    dist = (float)readData(addr, eDistance) / 10.0;    //Read distance register, one LSB is 0.1mm

    return dist;
}

/*
*@brief Read the temperature readings from target sensor
*
*@param addr: address of the sensor to be read from
*@return temperature read
*/
float RideHeightRS485::readTemp(uint16_t addr)
{
    temp =  (float)readData(addr, eInternalTemperature) / 10.0;    //Read the temperature register, one LSB is 0.1℃

    return temp;
}

/*
*@brief Revise the address of individual sensor
*
*@param currAddr: sensor address prior to revision
*@param newAddr: new address to be set
*/
void RideHeightRS485::setAddress(uint16_t currAddr, uint16_t newAddr)
{
    writeData(currAddr, eAddr, newAddr);    //Writes the new address value to the register
}

/*
*@brief Set the baudrate of the RS485 bus
*
*@param addr: address of the target sensor
*@param baudrateIndex: values map to different baudrates per sensor register specification
*/
void RideHeightRS485::setBaudrate(uint16_t addr, uint16_t baudrateIndex)
{
    //Writes the new baud rate value to the corresponding register
    if (writeData(addr, eComBaudrate, baudrateIndex))
    {
        if (baudrateIndex <= 8)
        {
            baudrate = baudrates[baudrateIndex-1];
        }
        else
        {
            baudrate = baudrates[7];
        }        
    }
}

#ifdef RS485_SERIAL_PORT
RS485Class RS485(RS485_SERIAL_PORT, RS485_TX_PIN, RS485_DE_PIN, RS485_RE_PIN);
#else
RS485Class RS485(SERIAL_PORT_HARDWARE, 1, RS485_DEFAULT_DE_PIN, RS485_DEFAULT_RE_PIN);
#endif

