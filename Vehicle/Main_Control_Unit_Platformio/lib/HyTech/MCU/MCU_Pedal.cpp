#include "MCU_Pedal.h"
#include "MCU_rev12_dfs.h"
#include <ADC_SPI.h>
#include <Task_Container.h>
#include "CAN_ID.h"
#include "CAN_Decl.h"

MCU_Pedal::MCU_Pedal() {
    taskContainer.emplace("MCU Pedal Read", INSTANCE_FUNC(read), 5);
    taskContainer.emplace("MCU Pedal CAN FWD", INSTANCE_FUNC(sendMsg), 5);
}

void MCU_Pedal::read() {
    static ADC_SPI adc2(ADC2_CS, ADC_SPI_SPEED);

    /* Filter ADC readings */
    data.accelerator_pedal_1 = ALPHA * data.accelerator_pedal_1 + (1 - ALPHA) * adc2.read_adc(ADC_ACCEL_1_CHANNEL);
    data.accelerator_pedal_2 = ALPHA * data.accelerator_pedal_2 + (1 - ALPHA) * adc2.read_adc(ADC_ACCEL_2_CHANNEL);
    data.brake_transducer_1 = ALPHA * data.brake_transducer_1 + (1 - ALPHA) * adc2.read_adc(ADC_BRAKE_1_CHANNEL);
    data.brake_transducer_2 = ALPHA * data.brake_transducer_2 + (1 - ALPHA) * adc2.read_adc(ADC_BRAKE_2_CHANNEL);
}

void MCU_Pedal::sendMsg() {
    CAN_message_t msg;
    msg.id = ID_MCU_PEDAL_READINGS;
    msg.len = sizeof(data);
    memcpy(msg.buf, &data, msg.len);
    telem_can.write(msg);
}


