#pragma once

#include <FlexCAN_T4.h>

extern FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> front_inv_can;
extern FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> rear_inv_can;
extern FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> telem_can;