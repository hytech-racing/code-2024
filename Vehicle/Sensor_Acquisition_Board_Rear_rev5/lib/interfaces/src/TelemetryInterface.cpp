#include "TelemetryInterface.h"

/* Update CAN messages */
// Invertor cooling loop thermistors
void TelemetryInterface::update_thermistors_CAN_msg(const AnalogConversion_s &therm3,
                                                    const AnalogConversion_s &therm4,
                                                    const AnalogConversion_s &therm5,
                                                    const AnalogConversion_s &therm6,
                                                    const AnalogConversion_s &therm7,
                                                    const AnalogConversion_s &therm8,
                                                    const AnalogConversion_s &therm9)
{
    sab_thermistors_1_.set_thermistor3(therm3.raw);
    sab_thermistors_1_.set_thermistor4(therm4.raw);
    sab_thermistors_1_.set_thermistor5(therm5.raw);
    sab_thermistors_1_.set_thermistor6(therm6.raw);
    enqueue_CAN_message<SAB_thermistors_1>(sab_thermistors_1_, ID_SAB_THERMISTORS_1);

    sab_thermistors_2_.set_thermistor7(therm7.raw);
    sab_thermistors_2_.set_thermistor8(therm8.raw);
    sab_thermistors_2_.set_thermistors(therm9.raw);
    enqueue_CAN_message<SAB_thermistors_2>(sab_thermistors_2_, ID_SAB_THERMISTORS_2);
}

// Rear loadcells and potentiometers from corner boards
void TelemetryInterface::update_cornerboard_CAN_msg(const AnalogConversion_s &lc_rl,
                                                    const AnalogConversion_s &lc_rr,
                                                    const AnalogConversion_s &pots_rl,
                                                    const AnalogConversion_s &pots_rr)
{
    sab_cb_.set_pot3(pots_rl.raw);
    sab_cb_.set_pot4(pots_rr.raw);
    sab_cb_.set_RL_loadcell(lc_rl.raw);
    sab_cb_.set_RR_loadcell(lc_rr.raw);
    enqueue_CAN_message<SAB_CB>(sab_cb_, ID_SAB_CB);
}

// TCU Pi shutdown signal from side panel
void TelemetryInterface::update_tcu_status_CAN_msg(const bool shutdown_status)
{
    tcu_status_.set_shutdown_status(static_cast<uint16_t>shutdown_status);
    enqueue_CAN_message<TCU_status>(tcu_status_, ID_TCU_STATUS);
}

/* Enqueue CAN message */
template<typename T>
void TelemetryInterface::enqueue_CAN_msg(T can_msg, uint32_t id)
{
    CAN_message_t msg;
    can_msg.write(msg.buf);
    msg.id = id;
    msg.len = sizeof(can_msg);
    uint8_t buf[sizeof(CAN_message_t)];
    memmove(buf, &msg, sizeof(CAN_message_t));

    msg_queue_->push_back(buf, sizeof(CAN_message_t));
}

/* Tick SysClock */
void TelemetryInterface::tick(const AnalogConversionPacket_s<4> &adc1,
                              const AnalogConversionPacket_s<4> &adc2,
                              const AnalogConversionPacket_s<8> &adc3,
                              const bool tcu_shutdown_status)
{
    // 10Hz
    update_thermistors_CAN_msg(adc3.conversions[channels_.therm3_channel],
                               adc3.conversions[channels_.therm4_channel],
                               adc3.conversions[channels_.therm5_channel],
                               adc3.conversions[channles_.therm6_channel],
                               adc3.conversions[channels_.therm7_channel],
                               adc3.conversions[channels_.therm8_channel],
                               adc3.conversions[channels_.therm9_channel]);
    // 20Hz
    update_cornerboard_CAN_msg(adc1.conversions[channels_.loadcell_rl_channel],
                               adc2.conversions[channels_.loadcell_rr_channel],
                               adc1_conversions[channels_.pots_rl_channel],
                               adc2.conversions[channels_.pots_rr_channel]);
    // 50Hz
    update_tcu_status_CAN_msg(tcu_shutdown_status);

    // IMU TBD  // 50Hz
}