#ifndef __HYTECHCAN_INTERFACE__
#define __HYTECHCAN_INTERFACE__

#include "FlexCAN_T4.h"
#include "HyTech_CAN.h"
#include "hytech.h"

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
inline void send_all_CAN_msg(CANBufferType &buffer, FlexCAN_T4_Base *CAN_interface) {
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
 * Prototype. Not needed atm
 */
inline void process_ring_buffer(CANBufferType &rx_buffer, unsigned long curr_millis) {
  while (rx_buffer.available()) {
    CAN_message_t recvd_msg;
    uint8_t buf[sizeof(CAN_message_t)];
    rx_buffer.pop_front(buf, sizeof(CAN_message_t));
    memmove(&recvd_msg, buf, sizeof(recvd_msg));
    switch (recvd_msg.id)
    {
    case ID_MCU_STATUS:
      /* code */
      break;
    
    default:
      break;
    }
  }
}

#endif  /* __HYTECHCAN_INTERFACE__ */