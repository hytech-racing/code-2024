#ifndef __TELEMTETRYINTERFACE_H__
#define __TELEMTETRYINTERFACE_H__

#include <stdint.h>
#include "FlexCAN_T4.h"
#include "HyTech_CAN.h"
#include "MessageQueueDefine.h"
#include "AnalogSensorsInterface.h"
#include "ThermistorInerface.h"

/**
 * ADC channels
 */
struct TelemetryInterfaceReadChannels
{
    int therm3_channel;
    int therm4_channel;
    int therm5_channel;
    int therm6_channel;
    int therm7_channel;
    int therm8_channel;
    int therm9_channel;
    int pots_rl_channel;
    int pots_rr_channel;
    int loadcell_rl_channel;
    int loadcell_rr_channel;
};

class TelemetryInterface
{
public:
    /* Constructors */
    TelemetryInterface(CANBufferType *msg_output_queue, const TelemetryInterfaceReadChannels &channels):
        msg_queue_(msg_output_queue),
        channels_(channels) {};

    /* Update CAN messages */
    void update_thermistors_CAN_msg(
        const TemperatureReport_s &thermTemp
    );
    void update_cornerboard_CAN_msg(
        const AnalogConversion_s &lc_rl,
        const AnalogConversion_s &lc_rr,
        const AnalogConversion_s &pots_rl,
        const AnalogConversion_s &pots_rr
    );
    void update_tcu_status_CAN_msg(
        const bool shutdown_status
    );

    template<typename T>
    void enqueue_CAN_msg(T can_msg, uint32_t id);

    template <typename U>
    void enqueue_new_CAN_msg(U *structure, uint32_t (*pack_function)(U *, uint8_t *, uint8_t *, uint8_t *));

    void tick(
        const AnalogConversionPacket_s<4> &adc1,
        const AnalogConversionPacket_s<4> &adc2,
        const AnalogConversionPacket_s<8> &adc3,
        const bool tcu_shutdown_status,
        const TemperatureReport_s &thermTemp
    );

private:
    /* CAN buffer */
    CANBufferType *msg_queue_;

    /* ADC read channels */
    TelemetryInterfaceReadChannels channels_;

    /* Outbound CAN message */
    SAB_thermistors_1 sab_thermistors_1_;
    SAB_thermistors_2 sab_thermistors_2_;
};




#endif /* __TELEMTETRYINTERFACE_H__ */