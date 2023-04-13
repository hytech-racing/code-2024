#include "CAN_Decl.h"

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> front_inv_can{};
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> rear_inv_can{};
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> telem_can{};

void __attribute__((constructor(150))) CAN_setup() {
    front_inv_can.begin();
    front_inv_can.setBaudRate(1000000);
    rear_inv_can.begin();
    rear_inv_can.setBaudRate(1000000);
    telem_can.begin();
    telem_can.setBaudRate(1000000);
    front_inv_can.enableMBInterrupts();
    rear_inv_can.enableMBInterrupts();
    telem_can.enableMBInterrupts();
}