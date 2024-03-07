#include "HytechCANInterface.h"

Circular_Buffer<uint8_t, (uint32_t)16, sizeof(CAN_message_t)> CAN1_rxBuffer;
Circular_Buffer<uint8_t, (uint32_t)16, sizeof(CAN_message_t)> CAN2_rxBuffer;
Circular_Buffer<uint8_t, (uint32_t)16, sizeof(CAN_message_t)> CAN3_rxBuffer;

Circular_Buffer<uint8_t, (uint32_t)16, sizeof(CAN_message_t)> CAN1_txBuffer;
Circular_Buffer<uint8_t, (uint32_t)16, sizeof(CAN_message_t)> CAN2_txBuffer;
Circular_Buffer<uint8_t, (uint32_t)16, sizeof(CAN_message_t)> CAN3_txBuffer;


void on_can1_receive(const CAN_message_t &msg)
{
    uint8_t buf[sizeof(CAN_message_t)];
    memmove(buf, &msg, sizeof(msg));
    CAN1_rxBuffer.push_back(buf, sizeof(CAN_message_t));
}

void on_can2_receive(const CAN_message_t &msg)
{
    uint8_t buf[sizeof(CAN_message_t)];
    memmove(buf, &msg, sizeof(msg));
    CAN2_rxBuffer.push_back(buf, sizeof(CAN_message_t));
}

void on_can3_receive(const CAN_message_t &msg)
{
    uint8_t buf[sizeof(CAN_message_t)];
    memmove(buf, &msg, sizeof(msg));
    CAN3_rxBuffer.push_back(buf, sizeof(CAN_message_t));
}