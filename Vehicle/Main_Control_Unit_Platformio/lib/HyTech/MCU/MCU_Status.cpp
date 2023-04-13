#include "MCU_Status.h"
#include "CAN_Decl.h"
#include "CAN_ID.h"
#include <Task_Container.h>

MCU_Status::MCU_Status() {
    taskContainer.emplace("Send MCU Status CAN", INSTANCE_FUNC(sendMsg), 5);
}

void MCU_Status::sendMsg() {
    CAN_message_t msg;
    msg.id = ID_MCU_STATUS;
    msg.len = sizeof(data);
    memcpy(msg.buf, &data, msg.len);
    telem_can.write(msg);
}
