#include "VectorNavInterface.h"

/**
 * Public functions
 */

// Data request functions
/// @brief set serial baudrate
/// @param baudrate serial speed in bps
/**
 * Checks feedback w/ delay
 */
void VectorNavInterface::setSerialBaudrate(uint32_t baudrate)
{
    vn_.setSerialBaudrate(baudrate);
}

/// @brief check the current serial baudrate being used
/**
 * Checks feedback w/ delay
 */
void VectorNavInterface::checkSerialBaudrate()
{
    vn_.checkSerialBaudrate();
}

/// @brief set initial heading for GNSS
/// @param initHeading orientation relative to TRUE NORTH, TRUE NORTH, PULL OUT YOUR PHONE, TRUE NORTH
/**
 * Checks feedback with delay
 */
void VectorNavInterface::setInitialHeading(uint32_t initHeading)
{
    vn_.setInitialHeading(initHeading);
}

/// @brief turn off asynchronous ASCII output from VN
/**
 * Does not check feedback
 */
void VectorNavInterface::turnOffAsciiOutput()
{
    vn_.turnOffAsciiOutput();
}

/// @brief configure asynchrinous ASCII outptu type
/// @param asciiReg value to specify which register to output
/**
 * Only certain registers can be set to asynchronously output
 * The GNSS health status one unfortunately cannot
 * Therefore when we opt for faster async data retrievement
 * We cannot be requesting GNSS signal strength at the same time
 */
void VectorNavInterface::configAsciiAsyncOutputType(uint32_t asciiReg)
{
    vn_.configAsciiAsyncOutputType(asciiReg);
}

/// @brief configure the frequency at whcih asynchronous ASCII output is sent by VN
/// @param asciiFreq output frequency to be set
void VectorNavInterface::configAsciiAsyncOutputFreq(uint32_t asciiFreq)
{
    vn_.configAsciiAsyncOutputFreq(asciiFreq);
}

/// @brief configure user defined binary packets
/// @param binaryOutputNumber binary register number (1-3) minus 1
/// @param fields requested fields
/// @param rateDivisor denominator from IMU rate (400Hz)
void VectorNavInterface::configBinaryOutput(uint8_t binaryOutputNumber, 
                                            uint8_t fields, 
                                            uint16_t rateDivisor)
{
    vn_.configBinaryOutput(binaryOutputNumber, fields, rateDivisor);
}

/// @brief request GNSS signal strength
void VectorNavInterface::requestGNSSSignalStrength(unsigned long currMillis)
{
    if (currMillis - lastVNRequestTime_ > VN_READ_INTERVAL)
    {
        vn_.requestGNSSSignalStrength();

        lastVNReadAsciiTime_ = currMillis;

        if (!asciiReadingStart_)
            asciiReadingStart_ = true;
 
        requestCounter_ = (requestCounter_ + 1) % 4;
        lastVNRequestTime_ = currMillis;
    }
}

/// @brief poll user configured binary packets
/// @param binaryOutputNumber binary register number (1-3) minus 1
void VectorNavInterface::pollUserConfiguredBinaryOutput(uint8_t *binaryOutputNumber, unsigned long currMillis)
{
    if (currMillis - lastVNRequestTime_ > VN_READ_INTERVAL)
    {
        vn_.pollUserConfiguredBinaryOutput(*binaryOutputNumber);

        lastVNReadBinaryTime_ = currMillis;

        if (!binaryReadingStart_)
            binaryReadingStart_ = true;

        *binaryOutputNumber = (*binaryOutputNumber + 1) % 3;  // skill issue fixed by Andy

        requestCounter_ = (requestCounter_ + 1) % 4;
        lastVNRequestTime_ = currMillis;
    }
}

// Data reveive functions
/// @brief receive ASCII
void VectorNavInterface::processGNSSSignalStrength(unsigned long currMillis)
{
    if ((currMillis - lastVNReadAsciiTime_ > VN_READ_ASCII_INTERVAL) && asciiReadingStart_)
    {
        vn_.readGNSSSignalStrength();

        update_CAN_vn_gnss_comp_sig_health(vn_.get());

        // Reset ascii reading start flag
        asciiReadingStart_ = false;
        // Reset VN read time
        lastVNReadAsciiTime_ = currMillis;
    }
}

/// @brief receive binary
void VectorNavInterface::processPollingBinaryOutput(unsigned long currMillis)
{
    if ((currMillis - lastVNReadBinaryTime_ > VN_READ_BINARY_INTERVAL) && binaryReadingStart_)
    {
        uint8_t binaryGroupRead =  vn_.readPollingBinaryOutput();

        switch (binaryGroupRead)
        {
            case 1:
                update_binary_group_1_CAN(vn_.get());
                break;

            case 2:
                update_binary_group_2_CAN(vn_.get());
                break;

            case 3:
                update_binary_group_3_CAN(vn_.get());
                break;
            
            default:
                break;
        }

        // Reset binary reading start flag
        binaryReadingStart_ = false;
        // Reset VN read time
        lastVNReadBinaryTime_ = currMillis;
    }
}

/// @brief receive asynchronous binary and ASCII output
/**
 * Called at loop rate
 */
void VectorNavInterface::readAsyncOutputs()
{
    if (vn_.readAsyncOutputs())
        update_binary_group_CAN(vn_.get());

}

// Data forward functions
/// @brief update and enqueue VN GPS time
void VectorNavInterface::update_CAN_vn_gps_time(const VNSensorDataReport_s &data)
{
    VN_GPS_TIME_t vn_time_gps;
    vn_time_gps.vn_gps_time = data.timeGPS;
    enqueue_new_CAN_msg<VN_GPS_TIME_t>(&vn_time_gps, &Pack_VN_GPS_TIME_hytech);
}

/// @brief update and enqueue VN longitude and latitude
void VectorNavInterface::update_CAN_vn_position(const VNSensorDataReport_s &data)
{
    VN_LAT_LON_t vn_position;
    vn_position.vn_gps_lat_ro = HYTECH_vn_gps_lat_ro_toS(static_cast<float>(data.latitude));
    vn_position.vn_gps_lon_ro = HYTECH_vn_gps_lon_ro_toS(static_cast<float>(data.longitude));
    enqueue_new_CAN_msg<VN_LAT_LON_t>(&vn_position, &Pack_VN_LAT_LON_hytech);
}

/// @brief update and enqueue VN acceleration
void VectorNavInterface::update_CAN_vn_accel(const VNSensorDataReport_s &data)
{
    VN_LINEAR_ACCEL_t vn_accel;
    vn_accel.vn_lin_ins_accel_x_ro = HYTECH_vn_lin_ins_accel_x_ro_toS(data.accelBodyX);
    vn_accel.vn_lin_ins_accel_y_ro = HYTECH_vn_lin_ins_accel_y_ro_toS(data.accelBodyY);
    vn_accel.vn_lin_ins_accel_z_ro = HYTECH_vn_lin_ins_accel_z_ro_toS(data.accelBodyZ);
    enqueue_new_CAN_msg<VN_LINEAR_ACCEL_t>(&vn_accel, &Pack_VN_LINEAR_ACCEL_hytech);
}

/// @brief update and enqueue VN INS status
void VectorNavInterface::update_CAN_vn_ins_status(const VNSensorDataReport_s &data)
{
    VN_STATUS_t vn_ins_status;
    vn_ins_status.vn_gps_status = data.insMode;
    enqueue_new_CAN_msg<VN_STATUS_t>(&vn_ins_status, &Pack_VN_STATUS_hytech);
}

/// @brief update and enqueue VN uncompensated acceleration
void VectorNavInterface::update_CAN_vn_uncomp_accel(const VNSensorDataReport_s &data)
{
    VN_LINEAR_ACCEL_UNCOMP_t vn_uncomp_accel;
    vn_uncomp_accel.vn_lin_uncomp_accel_x_ro = HYTECH_vn_lin_uncomp_accel_x_ro_toS(data.uncompAccelBodyX);
    vn_uncomp_accel.vn_lin_uncomp_accel_y_ro = HYTECH_vn_lin_uncomp_accel_y_ro_toS(data.uncompAccelBodyY);
    vn_uncomp_accel.vn_lin_uncomp_accel_z_ro = HYTECH_vn_lin_uncomp_accel_z_ro_toS(data.uncompAccelBodyZ);
    enqueue_new_CAN_msg<VN_LINEAR_ACCEL_UNCOMP_t>(&vn_uncomp_accel, &Pack_VN_LINEAR_ACCEL_UNCOMP_hytech);
}

/// @brief update and enqueue VN body velocity
void VectorNavInterface::update_CAN_vn_vel_body(const VNSensorDataReport_s &data)
{
    VN_VEL_t vn_vel_body;
    vn_vel_body.vn_body_vel_x_ro = HYTECH_vn_body_vel_x_ro_toS(data.velBodyX);
    vn_vel_body.vn_body_vel_y_ro = HYTECH_vn_body_vel_y_ro_toS(data.velBodyY);
    vn_vel_body.vn_body_vel_z_ro = HYTECH_vn_body_vel_z_ro_toS(data.velBodyZ);
    vn_vel_body.vn_vel_uncertainty_ro = HYTECH_vn_vel_uncertainty_ro_toS(data.velU);
    enqueue_new_CAN_msg<VN_VEL_t>(&vn_vel_body, &Pack_VN_VEL_hytech);
}

/// @brief update and enqueue VN angular rates
void VectorNavInterface::update_CAN_vn_angular_rate(const VNSensorDataReport_s &data)
{
    VN_ANGULAR_RATE_t vn_angular_rate;
    vn_angular_rate.angular_rate_x_ro = HYTECH_angular_rate_x_ro_toS(data.angularRateBodyX);
    vn_angular_rate.angular_rate_y_ro = HYTECH_angular_rate_y_ro_toS(data.angularRateBodyY);
    vn_angular_rate.angular_rate_z_ro = HYTECH_angular_rate_z_ro_toS(data.angularRateBodyZ);
    enqueue_new_CAN_msg<VN_ANGULAR_RATE_t>(&vn_angular_rate, &Pack_VN_ANGULAR_RATE_hytech);
}

/// @brief update and enqueue VN yaw pitch roll
void VectorNavInterface::update_CAN_vn_yaw_pitch_roll(const VNSensorDataReport_s &data)
{
    VN_YPR_t vn_YPR;
    vn_YPR.vn_yaw_ro = HYTECH_vn_yaw_ro_toS(data.yaw);
    vn_YPR.vn_pitch_ro = HYTECH_vn_pitch_ro_toS(data.pitch);
    vn_YPR.vn_roll_ro = HYTECH_vn_roll_ro_toS(data.roll);
    enqueue_new_CAN_msg<VN_YPR_t>(&vn_YPR, &Pack_VN_YPR_hytech);
}

/// @brief update and enqueue VN Ecef x and y coordinates
void VectorNavInterface::update_CAN_vn_ecef_pos_xy(const VNSensorDataReport_s &data)
{
    VN_ECEF_POS_XY_t vn_ecef_pos_xy;
    vn_ecef_pos_xy.vn_ecef_pos_x_ro = HYTECH_vn_ecef_pos_x_ro_toS(data.posEcefX);
    vn_ecef_pos_xy.vn_ecef_pos_y_ro = HYTECH_vn_ecef_pos_y_ro_toS(data.posEcefY);
    enqueue_new_CAN_msg<VN_ECEF_POS_XY_t>(&vn_ecef_pos_xy, &Pack_VN_ECEF_POS_XY_hytech);
}

/// @brief update and enqueue VN Ecef z coordinate
void VectorNavInterface::update_CAN_vn_ecef_pos_z(const VNSensorDataReport_s &data)
{
    VN_ECEF_POS_Z_t vn_ecef_pos_z;
    vn_ecef_pos_z.vn_ecef_pos_z_ro = HYTECH_vn_ecef_pos_z_ro_toS(data.posEcefZ);
    enqueue_new_CAN_msg<VN_ECEF_POS_Z_t>(&vn_ecef_pos_z, &Pack_VN_ECEF_POS_Z_hytech);
}

/// @brief update and enqueue VN GNSS signal health status
void VectorNavInterface::update_CAN_vn_gnss_comp_sig_health(const VNSensorDataReport_s &data)
{
    VN_GNSS_COMP_SIG_HEALTH_t vn_gnss_comp_health;
    vn_gnss_comp_health.num_sats_pvt_1 = data.gnssHealth.numSatsPVT_1;
    vn_gnss_comp_health.num_sats_rtk_1 = data.gnssHealth.numSatsRTK_1;
    vn_gnss_comp_health.highest_cn0_1_ro = HYTECH_highest_cn0_1_ro_toS(data.gnssHealth.highestCN0_1);
    vn_gnss_comp_health.num_sats_pvt_2 = data.gnssHealth.numSatsPVT_2;
    vn_gnss_comp_health.num_sats_rtk_2 = data.gnssHealth.numSatsRTK_2;
    vn_gnss_comp_health.highest_cn0_2_ro = HYTECH_highest_cn0_2_ro_toS(data.gnssHealth.highestCN0_2);
    vn_gnss_comp_health.num_com_sats_pvt = data.gnssHealth.numComSatsPVT;
    vn_gnss_comp_health.num_com_sats_rtk = data.gnssHealth.numComSatsRTK;
    enqueue_new_CAN_msg<VN_GNSS_COMP_SIG_HEALTH_t>(&vn_gnss_comp_health, &Pack_VN_GNSS_COMP_SIG_HEALTH_hytech);
}

/// @brief update and enqueue VN CAN messages from binary group 1
/// @param data VN data sampled by sensor
void VectorNavInterface::update_binary_group_1_CAN(const VNSensorDataReport_s &data)
{
    update_CAN_vn_gps_time(data);
    update_CAN_vn_position(data);
    update_CAN_vn_angular_rate(data);
}

/// @brief update and enqueue VN CAN messages from binary group 2
/// @param data VN data sampled by sensor
void VectorNavInterface::update_binary_group_2_CAN(const VNSensorDataReport_s &data)
{
    update_CAN_vn_accel(data);
    update_CAN_vn_ins_status(data);
    update_CAN_vn_uncomp_accel(data);
    update_CAN_vn_yaw_pitch_roll(data);

    // Missing CAN message for deltaVel right now
}

/// @brief update and enqueue VN CAN messages from binary group 3
/// @param data VN data sampled by sensor
void VectorNavInterface::update_binary_group_3_CAN(const VNSensorDataReport_s &data)
{
    update_CAN_vn_ecef_pos_xy(data);
    update_CAN_vn_ecef_pos_z(data);
    update_CAN_vn_vel_body(data);

    // Missing CAN message for PosEcef right now
}

/// @brief update and enqueue VN CAN messages from async binary group 1
/// @param data VN data sampled by sensor
/**
 * Used when asynchronously retrieving data
 */
void VectorNavInterface::update_binary_group_CAN(const VNSensorDataReport_s &data)
{
    update_binary_group_1_CAN(data);
    update_binary_group_2_CAN(data);
    update_binary_group_3_CAN(data);
}

// Iniialization
void VectorNavInterface::init(SysTick_s currTick)
{
    // Initialize sensor
    vn_.init();

    // Initialize binary output reg. number
    binaryOutputNumber_ = 0;
    // Initialize data request counter
    requestCounter_ = 0;    
    
    // Initialize timestamps
    lastVNRequestTime_ = currTick.millis;
    lastVNReadAsciiTime_ = currTick.millis;
    lastVNReadBinaryTime_ = currTick.millis;

    // Initialize boolean vars.
    binaryReadingStart_ = false;
    asciiReadingStart_ = false;
}

// Tick
void VectorNavInterface::tick(SysTick_s currTick)
{
    if (vn_.usePolling())
    {
        // Request data  
        if (requestCounter_ == 3)
            requestGNSSSignalStrength(currTick.millis);
        else
            pollUserConfiguredBinaryOutput(&binaryOutputNumber_, currTick.millis);

        // Read data
        processGNSSSignalStrength(currTick.millis);
        processPollingBinaryOutput(currTick.millis);
    }
    else if (vn_.useAsync())
    {
        if (currTick.triggers.trigger1000)  // should be at least 3x the rate at which async data is sent
        {
            readAsyncOutputs();
        }        
    }
    else
    {
        return;
    }   
}

// Print utilities for debug
/// @brief print receive buffer for binary packets
void VectorNavInterface::printBinaryReceiveBuffer()
{
    vn_.printBinaryReceiveBuffer();
}

/// @brief print receive buffer for ASCII packets
void VectorNavInterface::printAsciiReceiveBuffer()
{
    vn_.printAsciiReceiveBuffer();
}


/**
 * Private functions
 */

/// @brief template function for enqueuing CAN message
template <typename U>
void VectorNavInterface::enqueue_new_CAN_msg(U *structure, uint32_t (*pack_function)(U *, uint8_t *, uint8_t *, uint8_t *))
{
    CAN_message_t can_msg;

    can_msg.id = pack_function(structure, can_msg.buf, &can_msg.len, (uint8_t*) &can_msg.flags.extended);
    uint8_t buf[sizeof(CAN_message_t)] = {};
    memmove(buf, &can_msg, sizeof(CAN_message_t));

    msgQueue_->push_back(buf, sizeof(CAN_message_t));
}






