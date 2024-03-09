#include "HyTechCANInterface.h"

/* Rx buffer */
CANBufferType CAN2_rxBuffer;
/* Tx buffer */
CANBufferType CAN2_txBuffer;

/* CAN2 receive callback function */
void on_can2_receive(const CAN_message_t &msg)
{
    uint8_t buf[sizeof(CAN_message_t)];
    memmove(buf, &msg, sizeof(msg));
    CAN2_rxBuffer.push_back(buf, sizeof(CAN_message_t));
}
