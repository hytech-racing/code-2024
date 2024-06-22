#include "VectorNavInterface.h"


// Data request functions
/// @brief set serial baudrate
/// @param baudrate serial speed in bps
void VectorNavInterface::setSerialBaudrate(uint32_t baudrate)
{
    char toSend[DEFAULT_WRITE_BUFFER_MIDIUM];

    size_t length = sprintf(toSend, "$VNWRG,05,%u", baudrate);

    length += sprintf(toSend + length, "*XX\r\n");

    Serial2.print(toSend);
    Serial2.flush();

    delay(10);
    
    int index = 0;
    char receiveBuffer[DEFAULT_READ_BUFFER_SIZE] = {'\0'};
    while (Serial2.available() > 0 && index < DEFAULT_READ_BUFFER_SIZE - 1) {
        receiveBuffer[index++] = Serial2.read();
    }
    Serial.print("Set serial baudrate: ");
    Serial.println(receiveBuffer);

    Serial.println();
}

/// @brief set initial heading for GNSS
/// @param initHeading orientation relative to TRUE NORTH, TRUE NORTH, PULL OUT YOUR PHONE, TRUE NORTH
void setInitialHeading(uint32_t initHeading)
{
    char toSend[DEFAULT_WRITE_BUFFER_SIZE];

    size_t length = sprintf(toSend, "$VNSIH,+%03u", initHeading);
    length += sprintf(toSend + length, "*XX\r\n");

    Serial2.print(toSend);
    Serial2.flush();

    delay(10);

    int index = 0;
    char receiveBuffer[DEFAULT_READ_BUFFER_SIZE] = {'\0'};
    while (Serial2.available() > 0 && index < DEFAULT_READ_BUFFER_SIZE - 1) {
        receiveBuffer[index++] = Serial2.read();
    }
    Serial.print("Set initial heading: ");
    Serial.println(receiveBuffer);

    Serial.println();
}

/// @brief turn off asynchronous ASCII output from VN
void turnOffAsciiOutput()
{
    char toSend[DEFAULT_WRITE_BUFFER_MIDIUM];

    size_t length = sprintf(toSend, "$VNWRG,06,%u", vn::protocol::uart::VNOFF);

    length += sprintf(toSend + length, "*XX\r\n");

    Serial2.print(toSend);
    Serial2.flush();
}

/// @brief configure user defined binary packets
/// @param binaryOutputNumber binary register number (1-3) minus 1
/// @param fields requested fields
/// @param rateDivisor denominator from IMU rate (400Hz)
void configBinaryOutput(uint8_t binaryOutputNumber, 
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

    #if VN_HAVE_SECURE_CRT
    int length = sprintf_s(toSend, sizeof(toSend), "$VNWRG,%u,%u,%u,%X", 74 + binaryOutputNumber, fields.asyncMode, fields.rateDivisor, groups);
    #else
    int length = sprintf(toSend, "$VNWRG,%u,%u,%u,%X", 74 + binaryOutputNumber, vn::protocol::uart::ASYNCMODE_PORT1, rateDivisor, groups); // serial1, 800/16=50Hz, 
    #endif

    if (commonField)
    {
        if (binaryOutputNumber == 1)
        {
            length += sprintf(toSend + length, ",%X", vn::protocol::uart::COMMONGROUP_TIMEGPS |     // 0000 0000 0110 0010 = 00 62
                                                    vn::protocol::uart::COMMONGROUP_ANGULARRATE | 
                                                    vn::protocol::uart::COMMONGROUP_POSITION);
        }
        else if (binaryOutputNumber == 2)
        {
            length += sprintf(toSend + length, ",%X", vn::protocol::uart::COMMONGROUP_YAWPITCHROLL |
                                                    vn::protocol::uart::COMMONGROUP_ACCEL |       // 0001 0001 0000 1000 = 11 08
                                                    vn::protocol::uart::COMMONGROUP_INSSTATUS);
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

    Serial2.print(toSend);
    Serial2.flush();
}

/// @brief request GNSS signal strength
void requestGNSSSignalStrength()
{
    if (timer_vectornav_read_binary.check())
    {
        char toSend[DEFAULT_WRITE_BUFFER_SIZE];

        size_t length = sprintf(toSend, "$VNRRG,86");
        length += sprintf(toSend + length, "*XX\r\n");

        Serial2.print(toSend);
        Serial2.flush();

        timer_read_ascii.reset();

        if (!asciiReadingStart)
            asciiReadingStart = true;
 
        requestCounter = (requestCounter + 1) % 4;
    }
}

/// @brief poll user configured binary packets
/// @param binaryOutputNumber binary register number (1-3) minus 1
void pollUserConfiguredBinaryOutput(uint8_t *binaryOutputNumber)
{
    
}

// Data reveive functions
    /// @brief receive ASCII
    void readGNSSSignalStrength();

    /// @brief receive binary
    void readPollingBinaryOutput();

// Data parsers
    /// @brief parse user defined binary packet 1
    /// @param receiveBuffer receive buffer
    /// @param receivedPacketLength length of current received packet
    void parseBinaryOutput_1(uint8_t receiveBuffer[], int receivedPacketLength);

    /// @brief parse user defined binary packet 2     
    void parseBinaryOutput_2(uint8_t receiveBuffer[], int receivedPacketLength);

    /// @brief parse user defined binary packet 3
    void parseBinaryOutput_3(uint8_t receiveBuffer[], int receivedPacketLength);

// Data forward functions
    /// @brief update and enqueue VN GPS time
    void update_CAN_vn_gps_time();

    /// @brief update and enqueue VN longitude and latitude
    void update_CAN_vn_position();

    /// @brief update and enqueue VN acceleration
    void update_CAN_vn_accel();

    /// @brief update and enqueue VN INS status
    void update_CAN_vn_ins_status();

    /// @brief update and enqueue VN uncompensated acceleration
    void update_CAN_vn_uncomp_accel();

    /// @brief update and enqueue VN body velocity
    void update_CAN_vn_vel_body();

    /// @brief update and enqueue VN angular rates
    void update_CAN_vn_angular_rate();

    /// @brief update and enqueue VN yaw pitch roll
    void update_CAN_vn_yaw_pitch_roll();

    /// @brief update and enqueue VN Ecef x and y coordinates
    void update_CAN_vn_ecef_pos_xy();

    /// @brief update and enqueue VN Ecef z coordinate
    void update_CAN_vn_ecef_pos_z();
    
    /// @brief update and enqueue VN GNSS signal health status
    void update_CAN_vn_gnss_comp_sig_health();

// Tick
    void tick();
