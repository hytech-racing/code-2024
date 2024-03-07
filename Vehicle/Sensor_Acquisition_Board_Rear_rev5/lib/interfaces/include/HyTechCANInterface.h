#ifndef __HYTECHCAN_INTERFACE__
#define __HYTECHCAN_INTERFACE__

#include "FlexCAN_T4.h"
#include "HyTech_CAN.h"

#include "MessageQueueDefine.h"

/* CAN2 Rx buffer */
extern CANBufferType CAN2_rxBuffer;
/* CAN2 Tx buffer */
extern CANBufferType CAN2_txBuffer;

/** 
 * Receive callback function
 * Push received CAN messages to Rx buffer
 */
void on_can3_receive(const CAN_message_t &msg);





#endif  /* __HYTECHCAN_INTERFACE__ */