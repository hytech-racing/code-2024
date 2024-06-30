#ifndef __VECTORNAV_INTERFACE__
#define __VECTORNAV_INTERFACE__

#include <stdint.h>

#include "MessageQueueDefine.h"
#include "hytech.h"
#include "SysClock.h"
#include "VN_300.h"

const int VN_READ_INTERVAL = 10;        // milliseconds
const int VN_READ_ASCII_INTERVAL = 8;
const int VN_READ_BINARY_INTERVAL = 8;

class VectorNavInterface
{
private:
// Data
    // Logistic variables
    uint8_t requestCounter_;
    uint8_t binaryOutputNumber_;    
    // Async read variables
    bool startBinaryAsyncReceive_;
    bool startAsciiAsychReceive_;
    // Logistic variables    
    bool binaryReadingStart_;
    bool asciiReadingStart_;
    // Timer variables
    unsigned long lastVNRequestTime_;
    unsigned long lastVNReadAsciiTime_;
    unsigned long lastVNReadBinaryTime_;
// VN sensor
    VN_300 vn_;
// CAN buffer
    CANBufferType *msgQueue_;

// Private utility functions
    /// @brief template function for enqueuing CAN message
    template <typename U>
    void enqueue_new_CAN_msg(U *structure, uint32_t (*pack_function)(U *, uint8_t *, uint8_t *, uint8_t *));

public:
// Constructors
    VectorNavInterface(CANBufferType *circBuff, const vnParams_s &params):
        msgQueue_(circBuff),
        vn_(params) {};

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
    
    /// @brief configure asynchrinous ASCII outptu type
    /// @param asciiReg value to specify which register to output
    void configAsciiAsyncOutputType(uint32_t asciiReg);

    /// @brief configure the frequency at whcih asynchronous ASCII output is sent by VN
    /// @param asciiFreq output frequency to be set
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
    void processGNSSSignalStrength(unsigned long currMillis);

    /// @brief receive binary
    void processPollingBinaryOutput(unsigned long currMillis);

    /// @brief receive asynchronous binary and ASCII output
    void readAsyncOutputs();

// Data forward functions
    /// @brief update and enqueue VN GPS time
    void update_CAN_vn_gps_time(const VNSensorDataReport_s &data);

    /// @brief update and enqueue VN longitude and latitude
    void update_CAN_vn_position(const VNSensorDataReport_s &data);

    /// @brief update and enqueue VN acceleration
    void update_CAN_vn_accel(const VNSensorDataReport_s &data);

    /// @brief update and enqueue VN INS status
    void update_CAN_vn_ins_status(const VNSensorDataReport_s &data);

    /// @brief update and enqueue VN uncompensated acceleration
    void update_CAN_vn_uncomp_accel(const VNSensorDataReport_s &data);

    /// @brief update and enqueue VN body velocity
    void update_CAN_vn_vel_body(const VNSensorDataReport_s &data);

    /// @brief update and enqueue VN angular rates
    void update_CAN_vn_angular_rate(const VNSensorDataReport_s &data);

    /// @brief update and enqueue VN yaw pitch roll
    void update_CAN_vn_yaw_pitch_roll(const VNSensorDataReport_s &data);

    /// @brief update and enqueue VN Ecef x and y coordinates
    void update_CAN_vn_ecef_pos_xy(const VNSensorDataReport_s &data);

    /// @brief update and enqueue VN Ecef z coordinate
    void update_CAN_vn_ecef_pos_z(const VNSensorDataReport_s &data);
    
    /// @brief update and enqueue VN GNSS signal health status
    void update_CAN_vn_gnss_comp_sig_health(const VNSensorDataReport_s &data);

    /// @brief update and enqueue VN CAN messages from binary group 1
    /// @param data VN data sampled by sensor
    void update_binary_group_1_CAN(const VNSensorDataReport_s &data);

    /// @brief update and enqueue VN CAN messages from binary group 2
    /// @param data VN data sampled by sensor
    void update_binary_group_2_CAN(const VNSensorDataReport_s &data);

    /// @brief update and enqueue VN CAN messages from binary group 3
    /// @param data VN data sampled by sensor
    void update_binary_group_3_CAN(const VNSensorDataReport_s &data);

    /// @brief update and enqueue VN CAN messages from async binary group 1
    /// @param data VN data sampled by sensor
    void update_binary_group_CAN(const VNSensorDataReport_s &data);

// Initialization
    void init(SysTick_s currTick);

// Tick
    void tick(SysTick_s currTick);

// Getter
    const VNSensorDataReport_s& get()
    {
        return vn_.get();
    }

// Print utilities for debug
    /// @brief print receive buffer for binary packets
    void printBinaryReceiveBuffer();

    /// @brief print receive buffer for ASCII packets
    void printAsciiReceiveBuffer();

};

#endif  /* __VECTORNAV_INTERFACE__ */