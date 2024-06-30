#include "VN_300.h"

/**
 * Public functions
 */

// Data request functions
/// @brief set serial baudrate
/// @param baudrate serial speed in bps
/**
 * Checks feedback w/ delay
 */
void VN_300::setSerialBaudrate(uint32_t baudrate)
{
    char toSend[DEFAULT_WRITE_BUFFER_MIDIUM];

    size_t length = sprintf(toSend, "$VNWRG,05,%u", baudrate);

    length += sprintf(toSend + length, "*XX\r\n");

    serial_->print(toSend);
    serial_->flush();

#if CHECK_ASCII_REPONSE
    delay(10);
    
    int index = 0;
    char receiveBuffer[DEFAULT_READ_BUFFER_SIZE] = {'\0'};
    while (serial_->available() > 0 && index < DEFAULT_READ_BUFFER_SIZE - 1) {
        receiveBuffer[index++] = serial_->read();
    }
    Serial.print("Set serial baudrate: ");
    Serial.println(receiveBuffer);

    Serial.println();
#endif
}

/// @brief check the current serial baudrate being used
/**
 * Checks feedback w/ delay
 */
void VN_300::checkSerialBaudrate()
{
    char toSend[DEFAULT_WRITE_BUFFER_SIZE];

    size_t length = sprintf(toSend, "$VNRRG,05");
    length += sprintf(toSend + length, "*XX\r\n");

    serial_->print(toSend);
    serial_->flush();

#if CHECK_ASCII_RESPONSE
    delay(10);
    
    int index = 0;
    char receiveBuffer[DEFAULT_READ_BUFFER_SIZE] = {'\0'};
    while (serial_->available() > 0 && index < DEFAULT_READ_BUFFER_SIZE - 1) {
        receiveBuffer[index++] = serial_->read();
    }
    Serial.print("Read serial baudrate: ");
    Serial.println(receiveBuffer);

    Serial.println();
#endif
}

/// @brief set initial heading for GNSS
/// @param initHeading orientation relative to TRUE NORTH, TRUE NORTH, PULL OUT YOUR PHONE, TRUE NORTH
/**
 * Checks feedback with delay
 */
void VN_300::setInitialHeading(uint32_t initHeading)
{
    char toSend[DEFAULT_WRITE_BUFFER_SIZE];

    size_t length = sprintf(toSend, "$VNSIH,+%03u", initHeading);
    length += sprintf(toSend + length, "*XX\r\n");

    serial_->print(toSend);
    serial_->flush();

#if CHECK_ASCII_RESPONSE
    delay(10);

    int index = 0;
    char receiveBuffer[DEFAULT_READ_BUFFER_SIZE] = {'\0'};
    while (serial_->available() > 0 && index < DEFAULT_READ_BUFFER_SIZE - 1) {
        receiveBuffer[index++] = serial_->read();
    }
    Serial.print("Set initial heading: ");
    Serial.println(receiveBuffer);

    Serial.println();
#endif
}

/// @brief turn off asynchronous ASCII output from VN
/**
 * Does not check feedback
 */
void VN_300::turnOffAsciiOutput()
{
    char toSend[DEFAULT_WRITE_BUFFER_MIDIUM];

    size_t length = sprintf(toSend, "$VNWRG,06,%u", vn::protocol::uart::VNOFF);

    length += sprintf(toSend + length, "*XX\r\n");

    serial_->print(toSend);
    serial_->flush();

#if CHECK_ASCII_RESPONSE
    delay(10);

    int index = 0;
    char receiveBuffer[DEFAULT_READ_BUFFER_SIZE] = {'\0'};
    while (serial_->available() > 0 && index < DEFAULT_READ_BUFFER_SIZE - 1) {
        receiveBuffer[index++] = serial_->read();
    }
    Serial.print("Turn off ASCII output: ");
    Serial.println(receiveBuffer);

    Serial.println();
#endif
}

/// @brief configure asynchrinous ASCII outptu type
/// @param asciiReg value to specify which register to output
/**
 * Only certain registers can be set to asynchronously output
 * The GNSS health status one unfortunately cannot
 * Therefore when we opt for faster async data retrievement
 * We cannot be requesting GNSS signal strength at the same time
 */
void VN_300::configAsciiAsyncOutputType(uint32_t asciiReg)
{
    char toSend[DEFAULT_WRITE_BUFFER_MIDIUM];

    size_t length = sprintf(toSend, "$VNWRG,06,%u", asciiReg);

    length += sprintf(toSend + length, "*XX\r\n");

    serial_->print(toSend);
    serial_->flush();

#if CHECK_ASCII_RESPONSE
    delay(10);

    int index = 0;
    char receiveBuffer[DEFAULT_READ_BUFFER_SIZE] = {'\0'};
    while (serial_->available() > 0 && index < DEFAULT_READ_BUFFER_SIZE - 1) {
        receiveBuffer[index++] = serial_->read();
    }
    Serial.print("Configure ASCII output type: ");
    Serial.println(receiveBuffer);

    Serial.println();
#endif
}

/// @brief configure the frequency at whcih asynchronous ASCII output is sent by VN
/// @param asciiFreq output frequency to be set
void VN_300::configAsciiAsyncOutputFreq(uint32_t asciiFreq)
{
    char toSend[DEFAULT_WRITE_BUFFER_MIDIUM];

    size_t length = sprintf(toSend, "$VNWRG,07,%u", asciiFreq);

    length += sprintf(toSend + length, "*XX\r\n");

    serial_->print(toSend);
    serial_->flush();

#if CHECK_ASCII_RESPONSE
    delay(10);

    int index = 0;
    char receiveBuffer[DEFAULT_READ_BUFFER_SIZE] = {'\0'};
    while (serial_->available() > 0 && index < DEFAULT_READ_BUFFER_SIZE - 1) {
        receiveBuffer[index++] = serial_->read();
    }
    Serial.print("Configure ASCII output frequency: ");
    Serial.println(receiveBuffer);

    Serial.println();
#endif
}

/// @brief configure user defined binary packets
/// @param binaryOutputNumber binary register number (1-3) minus 1
/// @param fields requested fields
/// @param rateDivisor denominator from IMU rate (400Hz)
void VN_300::configBinaryOutput(uint8_t binaryOutputNumber, 
                                            uint8_t fields, 
                                            uint16_t rateDivisor)
{
    char toSend[DEFAULT_WRITE_BUFFER_LONG];

    bool commonField = fields & 0x01;   // Serial.printf("common: %d\n", commonField);
    bool timeField = fields & 0x02;     // Serial.printf("time: %d\n", timeField);
    bool imuField = fields & 0x04;      // Serial.printf("imu: %d\n", imuField);
    bool gpsField = fields & 0x08;      // Serial.printf("gps: %d\n", gpsField);
    bool attitudeField = fields & 0x10; // Serial.printf("attitude: %d\n", attitudeField);
    bool insField = fields & 0x20;      // Serial.printf("ins: %d\n", insField);
    bool gps2Field = fields & 0x40;     // Serial.printf("gps2: %d\n", gps2Field);

    // First determine which groups are present.
    uint16_t groups = 0;
    if (commonField)
        groups |= 0x0001;         // 0000 0000 0000 0001. common group selected
    if (timeField)
        groups |= 0x0002;
    if (imuField)
        groups |= 0x0004;         // 0000 0000 0000 0100. IMU group selected
    if (gpsField)
        groups |= 0x0008;         // 0000 0000 0000 1000. GNSS1 group selected
    if (attitudeField)
        groups |= 0x0010;
    if (insField)
        groups |= 0x0020;         // 0000 0000 0010 0000. INS group selected
    if (gps2Field)
        groups |= 0x0040;

    // groups = 0010 1101 = 2D

    int length = sprintf(toSend, "$VNWRG,%u,%u,%u,%X", 74 + binaryOutputNumber, vn::protocol::uart::ASYNCMODE_PORT1, rateDivisor, groups); // serial1, 800/16=50Hz, 

    if (commonField)
    {
        if (useAsync_)
        {
            length += sprintf(toSend + length, ",%X", vn::protocol::uart::COMMONGROUP_TIMEGPS |             // 0001 0001 0110 1010 = 11 6A
                                                      vn::protocol::uart::COMMONGROUP_ANGULARRATE |
                                                      vn::protocol::uart::COMMONGROUP_POSITION |
                                                      vn::protocol::uart::COMMONGROUP_YAWPITCHROLL |
                                                      vn::protocol::uart::COMMONGROUP_ACCEL |
                                                      vn::protocol::uart::COMMONGROUP_INSSTATUS);
        }
        else
        {        
            if (binaryOutputNumber == 1)
            {
                length += sprintf(toSend + length, ",%X", vn::protocol::uart::COMMONGROUP_TIMEGPS |         // 0000 0000 0110 0010 = 00 62
                                                          vn::protocol::uart::COMMONGROUP_ANGULARRATE | 
                                                          vn::protocol::uart::COMMONGROUP_POSITION);
            }
            else if (binaryOutputNumber == 2)
            {
                length += sprintf(toSend + length, ",%X", vn::protocol::uart::COMMONGROUP_YAWPITCHROLL |    // 0001 0001 0000 1000 = 11 08
                                                          vn::protocol::uart::COMMONGROUP_ACCEL |       
                                                          vn::protocol::uart::COMMONGROUP_INSSTATUS);
            }
        }
    }   
    if (timeField)
    {
        // Not requesting atm
        // Pending future use
    }
    if (imuField)
    {
        length += sprintf(toSend + length, ",%X", vn::protocol::uart::IMUGROUP_UNCOMPACCEL |    // 0000 0000 1000 0100 = 00 84
                                                  vn::protocol::uart::IMUGROUP_DELTAVEL);
    }
    if (gpsField)
    {
        length += sprintf(toSend + length, ",%X", vn::protocol::uart::GPSGROUP_POSECEF);        // 0000 0000 0100 0000 = 00 40
    }
    if (attitudeField)
    {
        // Not requesting atm
        // Pending future use
    }
    if (insField)
    {
        length += sprintf(toSend + length, ",%X", vn::protocol::uart::INSGROUP_VELBODY |
                                                  vn::protocol::uart::INSGROUP_VELU);        // 0000 0100 0000 1000 = 00 08
    }
    if(gps2Field)
    {
        // Not requesting atm
        // Pending future use
    }

    length += sprintf(toSend + length, "*XX\r\n");

    serial_->print(toSend);
    serial_->flush();
}

/// @brief request GNSS signal strength
void VN_300::requestGNSSSignalStrength()
{
    char toSend[DEFAULT_WRITE_BUFFER_SIZE];

    size_t length = sprintf(toSend, "$VNRRG,86");
    length += sprintf(toSend + length, "*XX\r\n");

    serial_->print(toSend);
    serial_->flush();
}

/// @brief poll user configured binary packets
/// @param binaryOutputNumber binary register number (1-3) minus 1
void VN_300::pollUserConfiguredBinaryOutput(uint8_t binaryOutputNumber)
{
    char toSend[DEFAULT_WRITE_BUFFER_SIZE];

    size_t length = sprintf(toSend, "$VNBOM,%u*", binaryOutputNumber + 1);
    length += sprintf(toSend + length, "XX\r\n");

    serial_->print(toSend);
    serial_->flush();
}

// Data reveive functions
/// @brief receive ASCII
void VN_300::readGNSSSignalStrength()
{
    int index = 0;
    char receiveBufferAscii[DEFAULT_READ_BUFFER_SIZE] = {'\0'};

    while (serial_->available())
    {
        receiveBufferAscii[index++] = serial_->read();
    }

#if DEBUG_GNSS_HEALTH
    Serial.print("Read GNSS compass health: ");
    Serial.println(receiveBufferAscii);

    Serial.println();
#endif

    strcpy(receiveBufferAscii_, receiveBufferAscii);
    
    currentAsciiLength_ = index;

    parseGNSSSignalStrength(receiveBufferAscii, 
                            &data_.gnssHealth.numSatsPVT_1,
                            &data_.gnssHealth.numSatsRTK_1,
                            &data_.gnssHealth.highestCN0_1,
                            &data_.gnssHealth.numSatsPVT_2,
                            &data_.gnssHealth.numSatsRTK_2,
                            &data_.gnssHealth.highestCN0_2,
                            &data_.gnssHealth.numComSatsPVT,
                            &data_.gnssHealth.numComSatsRTK);

#if DEBUG_GNSS_HEALTH
    Serial.printf("numSatsPVT_1 = %f, numSatsRTK_1 = %f, highestCN0_1 = %f\n", data_.gnssHealth.numSatsPVT_1, data_.gnssHealth.numSatsRTK_1, data_.gnssHealth.highestCN0_1);
    Serial.printf("numSatsPVT_2 = %f, numSatsRTK_2 = %f, highestCN0_2 = %f\n", data_.gnssHealth.numSatsPVT_2, data_.gnssHealth.numSatsRTK_2, data_.gnssHealth.highestCN0_2);
    Serial.printf("numComSatsPVT = %f, numComSatsRTK = %f\n\n", data_.gnssHealth.numComSatsPVT, data_.gnssHealth.numComSatsRTK);
#endif
}

/// @brief receive binary
/// @return binary group received (1-3)
uint8_t VN_300::readPollingBinaryOutput()
{
    int index = 0;
    uint8_t receiveBuffer[DEFAULT_SERIAL_BUFFER_SIZE] = {0};

    while (serial_->available())
    {
        receiveBuffer[index++] = serial_->read();
    }
    
    currentPacketLength_ = index;

    copyForExternalDispay<uint8_t>(receiveBuffer, currentPacketLength_);

    uint8_t binaryGroupReceived = 0;

    if (receiveBuffer[0] == 0xFA)
    {
        switch (receiveBuffer[1])
        {
            case 0x01:
            parseBinaryOutput_1(receiveBuffer, currentPacketLength_);
            binaryGroupReceived = 1;
            break;

            case 0x05:
            parseBinaryOutput_2(receiveBuffer, currentPacketLength_);
            binaryGroupReceived = 2;
            break;

            case 0x28:
            parseBinaryOutput_3(receiveBuffer, currentPacketLength_);
            binaryGroupReceived = 3;
            break;
            
            default:
            break;
        }
    }

    return binaryGroupReceived;
}

/// @brief receive asynchronous binary and ASCII output
/// @return true if complete async binary packet has been received
/**
 * Called at loop rate
 */
bool VN_300::readAsyncOutputs()
{
    bool dataProcessed = false;

    while (serial_->available())
    {
        char data = serial_->read();

        // Received potential keyword
        // Binary sync byte
        if (static_cast<uint8_t>(data) == 0xFA)
        {
            if (startAsciiAsychReceive_)    // currently we do not request ASCII async output, so this would always be fasle. if we ever do want to it should be a evaluation similar to the one for binary below
            {
                // process received ASCII packet
                startAsciiAsychReceive_ = false;
            }

            // Check if binary packet ready to parse
            if (asyncBinaryReadyToProcess())
            {
                currentPacketLength_ = indexBinary_;

                copyForExternalDispay<uint8_t>(receiveBufferBinaryAsync_, currentPacketLength_);

                parseBinaryOutput(receiveBufferBinaryAsync_, currentPacketLength_);

                dataProcessed = true;

#if DEBUG_ASYNC
                printBinaryReceiveBuffer();
#endif
                // Set to false once one packet finished parsing
                startBinaryAsyncReceive_ = false;
            }            
            
            if (!startBinaryAsyncReceive_)
            {
                startBinaryAsyncReceive_ = true;
                // If a packet finished parsing, start recording of a new packet
                indexBinary_ = 0;
            }
            
            // Append received byte
            receiveBufferBinaryAsync_[indexBinary_++] = static_cast<uint8_t>(data);
        }
        // Received byte is not keyword and binary packet actively recording
        else if (startBinaryAsyncReceive_)
        {
            // Append received byte
            receiveBufferBinaryAsync_[indexBinary_++] = static_cast<uint8_t>(data);
        }
        else if (startAsciiAsychReceive_)
        {
            receiveBufferAscii_[indexAscii_++] = data;
        }        
        else
        {
            continue;
        }
    }

    return dataProcessed;

}

/// @brief parse user define binary packet 1 when we request everything in group 1
/**
 * Used when asynchronously retrieving data
 */
void VN_300::parseBinaryOutput(uint8_t receiveBuffer[], int receivedPacketLength)
{
    if (receivedPacketLength != binaryPacketLength_)
    {
        return;
    }

    uint8_t syncByte = receiveBuffer[0];

    uint8_t groupByte = receiveBuffer[1];

    uint16_t groupField1 = parseUint16(receiveBuffer, 2);
    uint16_t groupField2 = parseUint16(receiveBuffer, 4);
    uint16_t groupField3 = parseUint16(receiveBuffer, 6);
    uint16_t groupField4 = parseUint16(receiveBuffer, 8);

    data_.timeGPS = parseUint64(receiveBuffer, 0 + OFFSET_PADDING);

    data_.yaw   = parseFloat(receiveBuffer, 8 + OFFSET_PADDING);
    data_.pitch = parseFloat(receiveBuffer, 12 + OFFSET_PADDING);
    data_.roll  = parseFloat(receiveBuffer, 16 + OFFSET_PADDING);

    data_.angularRateBodyX = parseFloat(receiveBuffer, 20 + OFFSET_PADDING);
    data_.angularRateBodyY = parseFloat(receiveBuffer, 24 + OFFSET_PADDING);
    data_.angularRateBodyZ = parseFloat(receiveBuffer, 28 + OFFSET_PADDING);

    data_.latitude = parseDouble(receiveBuffer, 32 + OFFSET_PADDING);
    data_.longitude = parseDouble(receiveBuffer, 40 + OFFSET_PADDING);
    data_.altitude = parseDouble(receiveBuffer, 48 + OFFSET_PADDING);    

    data_.accelBodyX = parseFloat(receiveBuffer, 56 + OFFSET_PADDING);
    data_.accelBodyY = parseFloat(receiveBuffer, 60 + OFFSET_PADDING);
    data_.accelBodyZ = parseFloat(receiveBuffer, 64 + OFFSET_PADDING);

    data_.InsStatus = parseUint16(receiveBuffer, 68 + OFFSET_PADDING);
    // (Refer to VN_300 manual p164)
    // 00 not tracking
    // 01 aligning
    // 10 tracking
    // 11 loss of GNSS for >45 seconds
    data_.insMode = data_.InsStatus & 0x0003; // Only take the last two bits

    data_.uncompAccelBodyX = parseFloat(receiveBuffer, 70 + OFFSET_PADDING);
    data_.uncompAccelBodyY = parseFloat(receiveBuffer, 74 + OFFSET_PADDING);
    data_.uncompAccelBodyZ = parseFloat(receiveBuffer, 78 + OFFSET_PADDING);

    data_.deltaVelX = parseFloat(receiveBuffer, 82 + OFFSET_PADDING);
    data_.deltaVelY = parseFloat(receiveBuffer, 86 + OFFSET_PADDING);
    data_.deltaVelZ = parseFloat(receiveBuffer, 90 + OFFSET_PADDING);

    data_.posEcefX = parseDouble(receiveBuffer, 94 + OFFSET_PADDING);
    data_.posEcefY = parseDouble(receiveBuffer, 102 + OFFSET_PADDING);
    data_.posEcefZ = parseDouble(receiveBuffer, 110 + OFFSET_PADDING);
    
    data_.velBodyX = parseFloat(receiveBuffer, 118 + OFFSET_PADDING);
    data_.velBodyY = parseFloat(receiveBuffer, 122 + OFFSET_PADDING);
    data_.velBodyZ = parseFloat(receiveBuffer, 126 + OFFSET_PADDING);
    data_.velU = parseFloat(receiveBuffer, 130 + OFFSET_PADDING_3);

    uint16_t crc = parseUint16(receiveBuffer, 134);
}

// Data parsers
/// @brief parse user defined binary packet 1
/// @param receiveBuffer receive buffer
/// @param receivedPacketLength length of current received packet
void VN_300::parseBinaryOutput_1(uint8_t receiveBuffer[], int receivedPacketLength)
{
    if (receivedPacketLength != binaryPacketLength_1)
    {
        return;
    }  

    uint8_t syncByte = receiveBuffer[0];
#if SANITY_CHECK
    if (syncByte != 0xFA)
        return;
#endif

    uint8_t groupsByte = receiveBuffer[1];
#if SANITY_CHECK
    if (groupsByte != 0x01)   // 0000 0001
        return;
#endif

    uint16_t groupField1 = ((uint16_t)receiveBuffer[3] << 8) | receiveBuffer[2];
#if SANITY_CHECK
    if (groupField1 != 0x0062)  // 0000 0000 0110 0010
        return;
#endif

#if DEBUG_VN
    Serial.println("Group 1 output:");
#endif

    data_.timeGPS = parseUint64(receiveBuffer, 0 + OFFSET_PADDING_1);
#if DEBUG_VN
    Serial.printf("GPS time: %X\n", data_.timeGPS);
#endif

    data_.angularRateBodyX = parseFloat(receiveBuffer, 8 + OFFSET_PADDING_1);
    data_.angularRateBodyY = parseFloat(receiveBuffer, 12 + OFFSET_PADDING_1);
    data_.angularRateBodyZ = parseFloat(receiveBuffer, 16 + OFFSET_PADDING_1);
#if DEBUG_VN
    Serial.printf("Angular rate X: %f  ", data_.angularRateBodyX);
    Serial.printf("Angular rate Y: %f  ", data_.angularRateBodyY);
    Serial.printf("Angular Rate Z raw: %f  ", data_.angularRateBodyZ);
#endif

    data_.latitude = parseDouble(receiveBuffer, 20 + OFFSET_PADDING_1);
    data_.longitude = parseDouble(receiveBuffer, 28 + OFFSET_PADDING_1);
    data_.altitude = parseDouble(receiveBuffer, 36 + OFFSET_PADDING_1);
#if DEBUG_VN
    Serial.printf("Raw Latitude: %f  ", data_.latitude);
    Serial.printf("Raw Longitude: %f  ", data_.longitude);
#endif

    uint16_t crc = (receiveBuffer[45 + OFFSET_PADDING_1] << 8) | receiveBuffer[44 + OFFSET_PADDING_1];
}

/// @brief parse user defined binary packet 2     
void VN_300::parseBinaryOutput_2(uint8_t receiveBuffer[], int receivedPacketLength)
{
    if (receivedPacketLength != binaryPacketLength_2)
    {
        return;
    }

    uint8_t syncByte = receiveBuffer[0];
#if SANITY_CHECK
    if (syncByte != 0xFA)
        return;
#endif

    uint8_t groupsByte = receiveBuffer[1];
#if SANITY_CHECK
    if (groupsByte != 0x05)   // 0000 0101
        return;
#endif

    uint16_t groupField1 = ((uint16_t)receiveBuffer[3] << 8) | receiveBuffer[2];
#if SANITY_CHECK
    if (groupField1 != 0x1100)    // 0001 0001 0000 0000
        return;
#endif

    uint16_t groupField2 = (receiveBuffer[5] << 8) | receiveBuffer[4];
#if SANITY_CHECK
    if (groupField2 != 0x0084)    // 0000 0000 1000 0100
        return;
#endif

#if DEBUG_VN
    Serial.println("Group 2 output:");
#endif

    data_.yaw   = parseFloat(receiveBuffer, OFFSET_PADDING_2_ORGINAL);
    data_.pitch = parseFloat(receiveBuffer, 4 + OFFSET_PADDING_2_ORGINAL);
    data_.roll  = parseFloat(receiveBuffer, 8 + OFFSET_PADDING_2_ORGINAL);   
#if DEBUG_VN
    Serial.printf("Yaw: %f  ", data_.yaw);                                 
    Serial.printf("Pitch: %f  ", data_.pitch);
    Serial.printf("Roll: %f\n", data_.roll);        
#endif          

    data_.accelBodyX = parseFloat(receiveBuffer, OFFSET_PADDING_2);
    data_.accelBodyY = parseFloat(receiveBuffer, 4 + OFFSET_PADDING_2);
    data_.accelBodyZ = parseFloat(receiveBuffer, 8 + OFFSET_PADDING_2);
#if DEBUG_VN
    Serial.printf("Accel body X: %f  ", data_.accelBodyX);
    Serial.printf("Accel body Y: %f  ", data_.accelBodyY);
    Serial.printf("Accel body Z: %f\n", data_.accelBodyZ);
#endif

    data_.InsStatus = parseUint16(receiveBuffer, 12 + OFFSET_PADDING_2);
    // (Refer to VN_300 manual p164)
    // 00 not tracking
    // 01 aligning
    // 10 tracking
    // 11 loss of GNSS for >45 seconds
    data_.insMode = data_.InsStatus & 0x0003; // Only take the last two bits  
#if DEBUG_VN
    Serial.printf("Ins status: %X\n", data_.InsStatus);
    Serial.printf("Ins status: %X\n", data_.insMode);
#endif

    data_.uncompAccelBodyX = parseFloat(receiveBuffer, 14 + OFFSET_PADDING_2);
    data_.uncompAccelBodyY = parseFloat(receiveBuffer, 18 + OFFSET_PADDING_2);
    data_.uncompAccelBodyZ = parseFloat(receiveBuffer, 22 + OFFSET_PADDING_2);
#if DEBUG_VN
    Serial.printf("UncompAccelBodyZ: %f  ", data_.uncompAccelBodyX);
    Serial.printf("UncompAccelBodyY: %f  ", data_.uncompAccelBodyY);
    Serial.printf("UncompAccelBodyZ: %f\n", data_.uncompAccelBodyZ);
#endif

    data_.deltaVelX = parseFloat(receiveBuffer, 26 + OFFSET_PADDING_2);
    data_.deltaVelY = parseFloat(receiveBuffer, 30 + OFFSET_PADDING_2);
    data_.deltaVelZ = parseFloat(receiveBuffer, 34 + OFFSET_PADDING_2);

    uint16_t crc = parseUint16(receiveBuffer, 38 + OFFSET_PADDING_2);
}

/// @brief parse user defined binary packet 3
void VN_300::parseBinaryOutput_3(uint8_t receiveBuffer[], int receivedPacketLength)
{
    if (receivedPacketLength != binaryPacketLength_3)
    {
        return;
    }

    uint8_t syncByte = receiveBuffer[0];
#if SANITY_CHECK
    if (syncByte != 0xFA)
        return;
#endif

    uint8_t groupsByte = receiveBuffer[1];
#if SANITY_CHECK
    if (groupsByte != 0x28)   // 0010 1000
        return;
#endif

    uint16_t groupField1 = ((uint16_t)receiveBuffer[3] << 8) | receiveBuffer[2];
#if SANITY_CHECK
    if (groupField1 != 0x0040)
        return;
#endif

    uint16_t groupField2 = (receiveBuffer[5] << 8) | receiveBuffer[4];
#if SANITY_CHECK
    if (groupField2 != 0x0008)
        return;
#endif

#if DEBUG_VN
    Serial.println("Group 3 output:");
#endif

    data_.posEcefX = parseDouble(receiveBuffer, OFFSET_PADDING_3);
    data_.posEcefY = parseDouble(receiveBuffer, 8 + OFFSET_PADDING_3);
    data_.posEcefZ = parseDouble(receiveBuffer, 16 + OFFSET_PADDING_3);
#if DEBUG_VN
    Serial.printf("PosEcf0 raw: %f  ", static_cast<float>(data_.posEcefX));
    Serial.printf("PosEcf1 raw: %f  ", static_cast<float>(data_.posEcefY));
    Serial.printf("PosEcf2 raw: %f  \n", static_cast<float>(data_.posEcefZ));
#endif
    
    data_.velBodyX = parseFloat(receiveBuffer, 24 + OFFSET_PADDING_3);
    data_.velBodyY = parseFloat(receiveBuffer, 28 + OFFSET_PADDING_3);
    data_.velBodyZ = parseFloat(receiveBuffer, 32 + OFFSET_PADDING_3);
    data_.velU = parseFloat(receiveBuffer, 36 + OFFSET_PADDING_3);
#if DEBUG_VN
    Serial.printf("Velocity body X: %f  ", data_.velBodyX);
    Serial.printf("Velocity body Y: %f  ", data_.velBodyY);
    Serial.printf("Velocity body Z: %f\n", data_.velBodyZ);
#endif    

    uint16_t crc = (receiveBuffer[41 + OFFSET_PADDING_3] << 8) | receiveBuffer[40 + OFFSET_PADDING_3];
}

/// @brief parse GNSS signal strength ASCII packet
/// @param receiveBufferAscii buffer array holding recieved ASCII output from VN
/// @param numSatsPVT_1 number of satellites PVT for GNSS 1
/// @param numSatsRTK_1 number of satellites RTK for GNSS 1
/// @param highestCN0_1 highest signal to noise ratio for GNSS 1
/// @param numSatsPVT_2 number of satellites PVT for GNSS 2
/// @param numSatsRTK_2 number of satellites RTK for GNSS 2
/// @param highestCN0_2 highest signal to noise ratio for GNSS 2
/// @param numComSatsPVT number of common satellites PVT for both GNSS's
/// @param numComSatsRTK number of common satellites RTK for both GNSS's
void VN_300::parseGNSSSignalStrength(char *receiveBufferAscii,
                                                 float *numSatsPVT_1,
                                                 float *numSatsRTK_1,
                                                 float *highestCN0_1,
                                                 float *numSatsPVT_2,
                                                 float *numSatsRTK_2,
                                                 float *highestCN0_2,
                                                 float *numComSatsPVT,
                                                 float *numComSatsRTK)
{
    char *vnAsciiHeader = "$VNRRG,86";

    // Sanity check receive string
    if (strncmp(receiveBufferAscii, vnAsciiHeader, 9))
    {
        return;
    }

    size_t parseIndex;

	char* result = startAsciiPacketParse(receiveBufferAscii, parseIndex); NEXT

    *numSatsPVT_1 = ATOFF; NEXT
    *numSatsRTK_1 = ATOFF; NEXT
    *highestCN0_1 = ATOFF; NEXT
    *numSatsPVT_2 = ATOFF; NEXT
    *numSatsRTK_2 = ATOFF; NEXT
    *highestCN0_2 = ATOFF; NEXT
    *numComSatsPVT = ATOFF; NEXT
    *numComSatsRTK = ATOFF;
}

// Iniialization
void VN_300::init()
{
    // Begin Serial communication
    serial_->begin(DEFAULT_SERIAL_BAUDRATE);
    // Wait for IMU to wake up  - Shayan
    delay(START_UP_DELAY);
    // Jack up baudrate. 128000 is the highest we can go, limitation unknown
    setSerialBaudrate((serialSpeed_ <= VN_SERIAL_BAUDRATE6) ? serialSpeed_ : VN_SERIAL_BAUDRATE6);
    // End current serial comm.
    serial_->end();
    // Resart serial with new baudrate
    serial_->begin((serialSpeed_ <= VN_SERIAL_BAUDRATE6) ? serialSpeed_ : VN_SERIAL_BAUDRATE6);

    // Configure sensor
    if (setInitHeading_)
    {
        setInitialHeading(initHeading_);
    }
    // Turn off asynchronous ascii output
    turnOffAsciiOutput();

    if (useAsync_)
    {
        // Configure ASCII
        // configAsciiAsyncOutputType();
        // configAsciiAsyncOutputFreq();

        // Configure user defined binary groups
        configBinaryOutput(1, 0x2D, ((binaryRateDivisor_ > 8) ? binaryRateDivisor_ : 8));    // 0010 1101. we max out at 50Hz, possibly constrained by hardware
    }    
    else
    {
        // Configure user defined binary groups
        configBinaryOutput(1, 0x01, 0);                     // 0000 0001
        configBinaryOutput(2, 0x05, 0);                     // 0000 0101
        configBinaryOutput(3, 0x28, 0);                     // 0010 1000
    }
    
    // Initialize binary packet length
    currentPacketLength_ = 0;
    // Initialize ascii packet length
    currentAsciiLength_ = 0;
    // Initialize display length
    displayLength_ = 0;    
    // Initialize boolean vars.    
    startBinaryAsyncReceive_ = false;
    startAsciiAsychReceive_ = false;
    // Initialize async index's
    indexBinary_ = 0;
    indexAscii_ = 0;
    // Calculate set length's
    calculateBinaryPacketsLength();   
}

// Print utilities for debug
/// @brief print receive buffer for binary packets
void VN_300::printBinaryReceiveBuffer()
{
    Serial.print("Binary: ");
    for (int i = 0; i < displayLength_; i++)
    {
        Serial.printf("%X ", receiveBuffer_[i]);
    }
    Serial.printf("\nCurrent binary packet length: %d", displayLength_);

    Serial.println();
}

/// @brief print receive buffer for ASCII packets
void VN_300::printAsciiReceiveBuffer()
{
    // Only print when not actively receiving async
    // When polling this is always true
    if (!startAsciiAsychReceive_)   // currently always true bc not requesting ASCII async output
    {
        Serial.print("Ascii: ");
        Serial.println(receiveBufferAscii_);
    }

    Serial.println();
}


/**
 * Private functions
 */

/// @brief parse uint16_t data from VN. small endianness
/// @param buffer byte holder array for VN data
/// @param startIndex array index where the uint16_t data starts
/// @return parsed uint16_t data
uint16_t VN_300::parseUint16(uint8_t buffer[], int startIndex)
{
    uint16_t data = ((uint16_t) buffer[1 + startIndex] << (8 * 1)) | ((uint16_t) buffer[startIndex]);

    return data;
}

/// @brief parse uint32_t data from VN. small endianness
/// @return parsed uint32_t data
uint32_t VN_300::parseUint32(uint8_t buffer[], int startIndex)
{
    uint32_t data = (((uint32_t) buffer[3 + startIndex] << (8 * 3)) | ((uint32_t) buffer[2 + startIndex] << (8 * 2)) |
                    ((uint32_t) buffer[1 + startIndex] << (8 * 1)) | ((uint32_t) buffer[0 + startIndex] << (8 * 0)));
  
    return data;
}

/// @brief parse uint32_t data from VN. small endianness
/// @return parsed uint64_t data
uint64_t VN_300::parseUint64(uint8_t buffer[], int startIndex)
{
    uint64_t data = (((uint64_t) buffer[7 + startIndex] << (8 * 7)) | ((uint64_t) buffer[6 + startIndex] << (8 * 6)) |
                    ((uint64_t) buffer[5 + startIndex] << (8 * 5)) | ((uint64_t) buffer[4 + startIndex] << (8 * 4)) |
                    ((uint64_t) buffer[3 + startIndex] << (8 * 3)) | ((uint64_t) buffer[2 + startIndex] << (8 * 2)) |
                    ((uint64_t) buffer[1 + startIndex] << (8 * 1)) | ((uint64_t) buffer[0 + startIndex] << (8 * 0)));
  
    return data;
}

/// @brief parse float data from VN. small endianness
/// @return parsed float data
float VN_300::parseFloat(uint8_t buffer[], int startIndex)
{
    uint32_t dataBits = parseUint32(buffer, startIndex);
  
    float data;
    memcpy(&data, &dataBits, sizeof(float));
    return data;
}

/// @brief parse double data from VN. small endianness
/// @return parsed double data
double VN_300::parseDouble(uint8_t buffer[], int startIndex)
{
    uint64_t dataBits = parseUint64(buffer, startIndex);

    double data;
    memcpy(&data, &dataBits, sizeof(double));
    return data;
}

/// @brief function to initiate the parsing of ASCII data packets
/// @param packetStart char array that holds ASCII response
/// @param index index in the array where data begins
/// @return sth. not sure what but co-works with the rest in macros
char* VN_300::startAsciiPacketParse(char* packetStart, size_t& index)
{
    index = 7;
	return vnstrtok(packetStart, index);
}

/// @brief function to get next data in ASCII packet response
/// @param str char array that holds ASCII response
/// @param startIndex index where the next data starts
/// @return pointer to next char in array
char* VN_300::getNextData(char* str, size_t& startIndex)
{
    return vnstrtok(str, startIndex);
}

/// @brief function to return pointer to next data in ASCII packet response
/// @param str char array that holds ASCII response
/// @param startIndex index where the next data starts
/// @return pointer to next data char in array
char* VN_300::vnstrtok(char* str, size_t& startIndex)
{
    size_t origIndex = startIndex;

	if (str[startIndex-1] == '*') // attempting to read too many fields
		return NULL;
		
	while (str[startIndex] != ',' && str[startIndex] != '*')
	{
		if((str[startIndex] < ' ') || (str[startIndex] > '~') || (str[startIndex] == '$')) // check for garbage characters
			return NULL;
		startIndex++;
	}
	
	str[startIndex++] = '\0';

	return str + origIndex;
}

/// @brief clear receive buffer
/// @param receiveBuffer the data buffer to be cleared
template <typename T>
void VN_300::clearReceiveBuffer(T receiveBuffer[], int length)
{
    for (int i = 0; i < length; i++)
    {
        receiveBuffer[i] = 0;
    }
}

/// @brief calculate the length of each binary packet
void VN_300::calculateBinaryPacketsLength()
{
    if (useAsync_)
    {
        binaryPacketLength_ = 1 + 1 + 2 * BINARY_OUTPUT_GROUP_COUNT_TOTAL + BINARY_OUTPUT_PAYLOAD + 2;

        binaryPacketLength_1 = 0;
        binaryPacketLength_2 = 0;
        binaryPacketLength_3 = 0;
    }
    else
    {
        binaryPacketLength_ = 0;

        binaryPacketLength_1 = 1 + 1 + 2 * BINARY_OUTPUT_GROUP_COUNT_1 + BINARY_OUTPUT_PAYLOAD_1 + 2;    
        binaryPacketLength_2 = 1 + 1 + 2 * BINARY_OUTPUT_GROUP_COUNT_2 + BINARY_OUTPUT_PAYLOAD_2 + 2;    
        binaryPacketLength_3 = 1 + 1 + 2 * BINARY_OUTPUT_GROUP_COUNT_3 + BINARY_OUTPUT_PAYLOAD_3 + 2;
    }    
}

/// @brief evaluate whether the asynchronous binary output received 
///        so far is ready to be parsed
/// @return true if data is ready
bool VN_300::asyncBinaryReadyToProcess()
{
    if (!startBinaryAsyncReceive_)
    {
        return false;
    }
    
    bool dataReady = false;
    if (receiveBufferBinaryAsync_[0] == 0xFA)
    {
        if (receiveBufferBinaryAsync_[1] == 0x2D && indexBinary_ == binaryPacketLength_)
        {
            dataReady = true;
        }
    }

    return dataReady;
}

/// @brief copy completely received data packet for external display
/// @param buffer holder array for received binary data packet
/// @param length length of the received packet
template <typename T>
void VN_300::copyForExternalDispay(T buffer[], int length)
{
    for (int i = 0; i < length; i++)
    {
        receiveBuffer_[i] = buffer[i];
    }

    displayLength_ = length;
}




