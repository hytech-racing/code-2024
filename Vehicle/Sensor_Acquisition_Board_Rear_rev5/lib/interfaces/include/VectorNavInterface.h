#ifndef __VECTORNAV_INERFACE__
#define __VECTORNAV_INERFACE__

#include <Arduino.h>

#include <stdint.h>

#include "types.h"
#include "VNConsts.h"
#include "MessageQueueDefine.h"

class VectorNavInterface
{
private:
// Data
    // Serial
    HardwareSerial *serial_;
    int serialSpeed_;
    uint8_t receiveBuffer[DEFAULT_SERIAL_BUFFER_SIZE];
    char receiveBufferAscii[DEFAULT_SERIAL_BUFFER_SIZE];
    // Logistic variables
    uint8_t requestCounter;
    uint8_t binaryOutputNumber;
    bool binaryReadingStart;
    bool asciiReadingStart;
// CAN buffer
    CANBufferType msgQueue_;

// Private utility functions
    /// @brief template function for enqueuing CAN message
    template <typename U>
    void enqueue_new_CAN_msg(U *structure, uint32_t (*pack_function)(U *, uint8_t *, uint8_t *, uint8_t *));

public:
// Constructors
    VectorNavInterface(HardwareSerial *serial, int serialSpeed):
        serial_(serial),
        serialSpeed_(serialSpeed) {};
    VectorNavInterface(HardwareSerial *serial):
        VectorNavInterface(serial, VN_SERIAL_BAUDRATE5) {};

// Data request functions
    /// @brief set serial baudrate
    /// @param baudrate serial speed in bps
    void setSerialBaudrate(uint32_t baudrate); 

    /// @brief set initial heading for GNSS
    /// @param initHeading orientation relative to TRUE NORTH, TRUE NORTH, PULL OUT YOUR PHONE, TRUE NORTH
    void setInitialHeading(uint32_t initHeading);

    /// @brief turn off asynchronous ASCII output from VN
    void turnOffAsciiOutput();

    /// @brief configure user defined binary packets
    /// @param binaryOutputNumber binary register number (1-3) minus 1
    /// @param fields requested fields
    /// @param rateDivisor denominator from IMU rate (400Hz)
    void configBinaryOutput(uint8_t binaryOutputNumber, 
                            uint8_t fields, 
                            uint16_t rateDivisor);

    /// @brief request GNSS signal strength
    void requestGNSSSignalStrength();

    /// @brief poll user configured binary packets
    /// @param binaryOutputNumber binary register number (1-3) minus 1
    void pollUserConfiguredBinaryOutput(uint8_t *binaryOutputNumber);

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

};

#endif  /* __VECTORNAV_INERFACE__ */