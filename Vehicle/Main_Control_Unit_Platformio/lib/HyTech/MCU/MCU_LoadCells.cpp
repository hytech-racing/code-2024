#include "MCU_LoadCells.h"
#include "MCU_rev12_dfs.h"
#include <ADC_SPI.h>
#include <Task_Container.h>
#include "CAN_Decl.h"
#include "CAN_ID.h"

MCU_LoadCells::MCU_LoadCells() {
    taskContainer.emplace("MCU Load Cells Read", INSTANCE_FUNC(read), 5);
    taskContainer.emplace("MCU Load Cells CAN FWD", INSTANCE_FUNC(sendMsg), 5);
}

void MCU_LoadCells::read() {
    static ADC_SPI adc2(ADC2_CS, ADC_SPI_SPEED);

    //load cell is 2mV/V, 10V excitation, 1000lb max
    //goes through 37.5x gain of INA823, 21x gain of OPA991, +0.314V offset, 0.1912x reduction on ECU and MAX7400 before reaching ADC
    data.FL_load_cell = (uint16_t) (((adc2.read_adc(ADC_FL_LOAD_CELL_CHANNEL) / 0.1912) - 0.314) / 787.5 * 50);
    data.FR_load_cell = (uint16_t) (((adc2.read_adc(ADC_FR_LOAD_CELL_CHANNEL) / 0.1912) - 0.314) / 787.5 * 50);
    data.RL_load_cell = (uint16_t) (((adc2.read_adc(ADC_RL_LOAD_CELL_CHANNEL) / 0.1912) - 0.314) / 787.5 * 50);
    data.RR_load_cell = (uint16_t) (((adc2.read_adc(ADC_RR_LOAD_CELL_CHANNEL) / 0.1912) - 0.314) / 787.5 * 50);
}

void MCU_LoadCells::sendMsg() {
    CAN_message_t msg;
    msg.id = ID_MCU_LOAD_CELLS;
    msg.len = sizeof(data);
    memcpy(msg.buf, &data, msg.len);
    telem_can.write(msg);
}
