#include "TelemetryInterface.h"

/* Update CAN messages */
// Invertor cooling loop thermistors
void TelemetryInterface::update_thermistors_CAN_msg(const TemperatureReport_s<7> &thermTemp)
{
    SAB_THERMISTORS_1_t sab_thermistors_1;
    sab_thermistors_1.thermistor_inv1_ro = HYTECH_thermistor_inv1_ro_toS(thermTemp.temperatures[channels_.therm_channel_1]);
    sab_thermistors_1.thermistor_inv2_ro = HYTECH_thermistor_inv2_ro_toS(thermTemp.temperatures[channels_.therm_channel_2]);
    sab_thermistors_1.thermistor_acc1_ro = HYTECH_thermistor_acc1_ro_toS(thermTemp.temperatures[channels_.therm_channel_3]);
    sab_thermistors_1.thermistor_acc2_ro = HYTECH_thermistor_acc2_ro_toS(thermTemp.temperatures[channels_.therm_channel_4]);
    enqueue_new_CAN_msg<SAB_THERMISTORS_1_t>(&sab_thermistors_1, &Pack_SAB_THERMISTORS_1_hytech);

    SAB_THERMISTORS_2_t sab_thermistors_2;
    sab_thermistors_2.thermistor_motor_rl_ro = HYTECH_thermistor_motor_rl_ro_toS(thermTemp.temperatures[channels_.therm_channel_5]);
    sab_thermistors_2.thermistor_motor_rr_ro = HYTECH_thermistor_motor_rr_ro_toS(thermTemp.temperatures[channels_.therm_channel_6]);
    sab_thermistors_2.thermistor_pump_ro = HYTECH_thermistor_pump_ro_toS(thermTemp.temperatures[channels_.therm_channel_7]);
    enqueue_new_CAN_msg<SAB_THERMISTORS_2_t>(&sab_thermistors_2, &Pack_SAB_THERMISTORS_2_hytech);
}

// Rear loadcells and potentiometers from corner boards
void TelemetryInterface::update_cornerboard_CAN_msg(const AnalogConversion_s &lc_rl,
                                                    const AnalogConversion_s &lc_rr,
                                                    const AnalogConversion_s &pots_rl,
                                                    const AnalogConversion_s &pots_rr)
{
    SAB_CB sab_cb;
    sab_cb.set_pot3(pots_rl.raw);
    sab_cb.set_pot4(pots_rr.raw);
    sab_cb.set_RL_load_cell(lc_rl.raw);
    sab_cb.set_RR_load_cell(lc_rr.raw);
    enqueue_CAN_msg<SAB_CB>(sab_cb, ID_SAB_CB);
}

// TCU Pi shutdown signal from side panel
void TelemetryInterface::update_tcu_status_CAN_msg(const bool shutdown_status)
{
    TCU_status tcu_status;
    tcu_status.set_shutdown_status(static_cast<uint16_t>(shutdown_status));
    enqueue_CAN_msg<TCU_status>(tcu_status, ID_TCU_STATUS);
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

template<typename U>
void TelemetryInterface::enqueue_new_CAN_msg(U *structure, uint32_t (*pack_function)(U *, uint8_t *, uint8_t *, uint8_t *))
{
    CAN_message_t can_msg;
    can_msg.id = pack_function(structure, can_msg.buf, &can_msg.len, (uint8_t*) &can_msg.flags.extended);
    uint8_t buf[sizeof(CAN_message_t)] = {};
    memmove(buf, &can_msg, sizeof(CAN_message_t));
    
    msg_queue_->push_back(buf, sizeof(CAN_message_t));
}

/* Tick SysClock */
void TelemetryInterface::tick(const AnalogConversionPacket_s<4> &adc1,
                              const AnalogConversionPacket_s<4> &adc2,
                              const AnalogConversionPacket_s<8> &adc3,
                              const bool tcu_shutdown_status,
                              const TemperatureReport_s<7> &thermTemp)
{
    // 10Hz
    update_thermistors_CAN_msg(thermTemp);
    // 20Hz
    update_cornerboard_CAN_msg(adc1.conversions[channels_.loadcell_rl_channel],
                               adc2.conversions[channels_.loadcell_rr_channel],
                               adc1.conversions[channels_.pots_rl_channel],
                               adc2.conversions[channels_.pots_rr_channel]);
    // 50Hz
    update_tcu_status_CAN_msg(tcu_shutdown_status);

    // IMU TBD  // 50Hz
}