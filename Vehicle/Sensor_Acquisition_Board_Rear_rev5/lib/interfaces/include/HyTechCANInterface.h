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
void on_can2_receive(const CAN_message_t &msg);

/**
 * Send CAN function
*/
void send_all_CAN_msg(CANBufferType &buffer, FlexCAN_T4_Base *CAN_interface) {
    while (buffer.available())
    {
        CAN_message_t msg;
        uint8_t buf[sizeof(CAN_message_t)];
        buffer.pop_front(buf, sizeof(CAN_message_t));
        memmove(&msg, buf, sizeof(msg));
        CAN_interface->write(msg);
    }    
}

/**
 * Receive CAN function
 * 
 * Not needed atm
 */
// void process_CAN_buffer();

#endif  /* __HYTECHCAN_INTERFACE__ */