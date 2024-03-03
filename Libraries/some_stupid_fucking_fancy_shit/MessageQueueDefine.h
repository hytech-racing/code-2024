#ifndef MESAGE_QUEUE_DEFINE_H
#define MESAGE_QUEUE_DEFINE_H

#include "FlexCAN_T4.h"

constexpr std::size_t CANBufferSize = 16;
// does this need to be 3 params like our original definitions of the buffers?
using CANBufferType = Circular_Buffer<uint8_t, (uint32_t)16, sizeof(CAN_message_t)>;

#endif