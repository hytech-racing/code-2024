#ifndef __VECTORNAV_INERFACE__
#define __VECTORNAV_INERFACE__

#include <Arduino.h>

#include <stdint.h>

#include "types.h"
#include "VNConsts.h"
#include "MessageQueueDefine.h"
#include "hytech.h"
#include "SysClock.h"

extern CANTelem TELEM_CAN;

const int DEFAULT_SERIAL_BAUDRATE = 115200;
const int DEFAULT_INIT_HEADING = 0;     // Relative to TRUE NORTH TRUE NORTH TRUE NORTH

const int START_UP_DELAY = 5000;
const int VN_READ_INTERVAL = 10;        // milliseconds
const int VN_READ_ASCII_INTERVAL = 8;
const int VN_READ_BINARY_INTERVAL = 8;

// Sensor data
struct VNGNSSSigHealth_s
{
    float numSatsPVT_1;
    float numSatsRTK_1;
    float highestCN0_1;
    float numSatsPVT_2;
    float numSatsRTK_2;
    float highestCN0_2;
    float numComSatsPVT;
    float numComSatsRTK;
};

struct VNSensorDataReport_s
{
    // GPS time
    uint64_t timeGPS;
    // Angular rates
    float angularRateBodyX;
    float angularRateBodyY;
    float angularRateBodyZ;
    // GNSS LLA
    double latitude;
    double longitude;
    double altitude;
    // Attitudes
    float yaw;
    float pitch;
    float roll;
    // Acceleration
    float accelBodyX;
    float accelBodyY;
    float accelBodyZ;
    // INS status
    uint16_t InsStatus;
    uint16_t insMode;
    // Uncompensated acceleration
    float uncompAccelBodyX;
    float uncompAccelBodyY;
    float uncompAccelBodyZ;
    // Velocity delta
    float deltaVelX;
    float deltaVelY;
    float deltaVelZ;
    // GNSS Ecef
    double posEcefX;
    double posEcefY;
    double posEcefZ;
    // Body velocity
    float velBodyX;
    float velBodyY;
    float velBodyZ;
    float velU;     // uncertainty of velocity measurement. Not very indicative, VN hella confident when wrong, reporting neg vel during accel.
    // GNSS health
    VNGNSSSigHealth_s gnssHealth;
};

class VectorNavInterface
{
private:
// Data
    // Serial
    HardwareSerial *serial_;
    int serialSpeed_;
    uint8_t receiveBuffer_[DEFAULT_SERIAL_BUFFER_SIZE] = {0};
    char receiveBufferAscii_[DEFAULT_SERIAL_BUFFER_SIZE] = {'\0'};
    // Initial heading
    bool setInitHeading_;
    uint32_t initHeading_;
    // Logistic variables
    uint8_t requestCounter_;
    uint8_t binaryOutputNumber_;
    bool binaryReadingStart_;
    bool asciiReadingStart_;
    int currentPacketLength_;
    int currentAsciiLength_;
    int binaryPacketLength_1;
    int binaryPacketLength_2;
    int binaryPacketLength_3;
    // Async read variables
    uint16_t binaryRateDivisor_;
    bool startBinaryAsyncReceive_;
    bool startAsciiAsychReceive_;
    int indexBinary_;
    int indexAscii_;
    // Data requesting approach
    bool usePolling_;
    bool useAsync_;
    // Timer variables
    unsigned long lastVNRequestTime_;
    unsigned long lastVNReadAsciiTime_;
    unsigned long lastVNReadBinaryTime_;
// CAN buffer
    CANBufferType *msgQueue_;
// Sensor readings
    VNSensorDataReport_s data_;

// Private utility functions
    /// @brief template function for enqueuing CAN message
    template <typename U>
    void enqueue_new_CAN_msg(U *structure, uint32_t (*pack_function)(U *, uint8_t *, uint8_t *, uint8_t *));

    /// @brief parse uint16_t data from VN. small endianness
    /// @param buffer byte holder array for VN data
    /// @param startIndex array index where the uint16_t data starts
    /// @return parsed uint16_t data
    uint16_t parseUint16(uint8_t buffer[], int startIndex);

    /// @brief parse uint32_t data from VN. small endianness
    /// @return parsed uint32_t data
    uint32_t parseUint32(uint8_t buffer[], int startIndex);

    /// @brief parse uint32_t data from VN. small endianness
    /// @return parsed uint64_t data
    uint64_t parseUint64(uint8_t buffer[], int startIndex);

    /// @brief parse float data from VN. small endianness
    /// @return parsed float data
    float parseFloat(uint8_t buffer[], int startIndex);

    /// @brief parse double data from VN. small endianness
    /// @return parsed double data
    double parseDouble(uint8_t buffer[], int startIndex);

    /// @brief function to initiate the parsing of ASCII data packets
    /// @param packetStart char array that holds ASCII response
    /// @param index index in the array where data begins
    /// @return sth. not sure what but co-works with the rest in macros
    char* startAsciiPacketParse(char* packetStart, size_t& index);

    /// @brief function to get next data in ASCII packet response
    /// @param str char array that holds ASCII response
    /// @param startIndex index where the next data starts
    /// @return pointer to next char in array
    char* getNextData(char* str, size_t& startIndex);

    /// @brief function to return pointer to next data in ASCII packet response
    /// @param str char array that holds ASCII response
    /// @param startIndex index where the next data starts
    /// @return pointer to next data char in array
    char* vnstrtok(char* str, size_t& startIndex);

    /// @brief clear binary receive buffer
    /// @param receiveBuffer the data buffer to be cleared
    void clearReceiveBuffer(uint8_t receiveBuffer[]);

    void calculateBinaryPacketsLength();

    bool asyncBinaryReadyToProcess();

public:
// Constructors
    VectorNavInterface(CANBufferType *circBuff, HardwareSerial *serial, int serialSpeed, bool setInitHeading, uint32_t initHeading, bool useAsync, uint16_t binaryRateDivisor):
        msgQueue_(circBuff),
        serial_(serial),
        serialSpeed_(serialSpeed),
        setInitHeading_(setInitHeading),
        initHeading_(initHeading),
        useAsync_(useAsync),
        usePolling_(!useAsync),
        binaryRateDivisor_(useAsync ? binaryRateDivisor : 0) {};
    VectorNavInterface(CANBufferType *circBuff, HardwareSerial *serial, int serialSpeed, bool setInitHeading, uint32_t initHeading):
        VectorNavInterface(circBuff, serial, serialSpeed, setInitHeading, initHeading, false, 0) {};
    VectorNavInterface(CANBufferType *circBuff, HardwareSerial *serial, int serialSpeed, bool setInitHeading):
        VectorNavInterface(circBuff, serial, serialSpeed, setInitHeading, DEFAULT_INIT_HEADING, false, 0) {};
    VectorNavInterface(CANBufferType *circBuff, HardwareSerial *serial, int serialSpeed):
        VectorNavInterface(circBuff, serial, serialSpeed, false, DEFAULT_INIT_HEADING, false, 0) {};
    VectorNavInterface(CANBufferType *circBuff, HardwareSerial *serial):
        VectorNavInterface(circBuff, serial, DEFAULT_SERIAL_BAUDRATE, false, DEFAULT_INIT_HEADING, false, 0) {};

// Data request functions
    /// @brief set serial baudrate
    /// @param baudrate serial speed in bps
    void setSerialBaudrate(uint32_t baudrate);

    /// @brief check the current serial baudrate being used
    void checkSerialBaudrate();

    /// @brief set initial heading for GNSS
    /// @param initHeading orientation relative to TRUE NORTH, TRUE NORTH, PULL OUT YOUR PHONE, TRUE NORTH
    void setInitialHeading(uint32_t initHeading);

    /// @brief turn off asynchronous ASCII output from VN
    void turnOffAsciiOutput();
    
    void configAsciiAsyncOutputType(uint32_t asciiReg);

    void configAsciiAsyncOutputFreq(uint32_t asciiFreq);

    /// @brief configure user defined binary packets
    /// @param binaryOutputNumber binary register number (1-3) minus 1
    /// @param fields requested fields
    /// @param rateDivisor denominator from IMU rate (400Hz)
    void configBinaryOutput(uint8_t binaryOutputNumber,
                            uint8_t fields,
                            uint16_t rateDivisor);

    /// @brief request GNSS signal strength
    void requestGNSSSignalStrength(unsigned long currMillis);

    /// @brief poll user configured binary packets
    /// @param binaryOutputNumber binary register number (1-3) minus 1
    void pollUserConfiguredBinaryOutput(uint8_t *binaryOutputNumber, unsigned long currMillis);

// Data reveive functions
    /// @brief receive ASCII
    void readGNSSSignalStrength(unsigned long currMillis);

    /// @brief receive binary
    void readPollingBinaryOutput(unsigned long currMillis);

    /// @brief receive asynchronous binary and ASCII output
    void readAsyncOutputs();

// Data process intermediate functions
    void processGNSSSignalStrength(char *receiveBufferAscii);

// Data parsers
    /// @brief parse user defined binary packet 1
    /// @param receiveBuffer receive buffer
    /// @param receivedPacketLength length of current received packet
    void parseBinaryOutput_1(uint8_t receiveBuffer[], int receivedPacketLength);

    /// @brief parse user defined binary packet 2     
    void parseBinaryOutput_2(uint8_t receiveBuffer[], int receivedPacketLength);

    /// @brief parse user defined binary packet 3
    void parseBinaryOutput_3(uint8_t receiveBuffer[], int receivedPacketLength);

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
    void parseGNSSSignalStrength(char *receiveBufferAscii,
                                 float *numSatsPVT_1,
                                 float *numSatsRTK_1,
                                 float *highestCN0_1,
                                 float *numSatsPVT_2,
                                 float *numSatsRTK_2,
                                 float *highestCN0_2,
                                 float *numComSatsPVT,
                                 float *numComSatsRTK);

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

// Initialization
    void init(SysTick_s currTick);

// Tick
    void tick(SysTick_s currTick);

// Getter
    const VNSensorDataReport_s& get()
    {
        return data_;
    }

// Print utilities for debug
    /// @brief print receive buffer for binary packets
    void printBinaryReceiveBuffer();

    /// @brief print receive buffer for ASCII packets
    void printAsciiReceiveBuffer();

};

#endif  /* __VECTORNAV_INERFACE__ */