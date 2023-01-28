#include "MC_Energy.h"
#include <Task_Container.h>
#include "CAN_Decl.h"
#include "CAN_ID.h"

MC_Energy::MC_Energy() {
    taskContainer.emplace("MC Energy CAN FWD", INSTANCE_FUNC(sendMsg), 5);
}

void MC_Energy::sendMsg() {
    uint32_t id = ID_MC1_ENERGY;
    for (auto &i: data.iter()) {
        CAN_message_t msg;
        msg.id = id;
        msg.len = sizeof(i.get());
        memcpy(msg.buf, &(i.get()), msg.len);
        if (id < ID_MC3_TEMPS) {
            front_inv_can.write(msg);
        } else {
            rear_inv_can.write(msg);
        }
        ++id;
    }
}

void MC_Energy::load(const uint8_t buf[]) {
    memcpy(&data, buf, sizeof(*this));
}