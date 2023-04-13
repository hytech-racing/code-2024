#pragma once


class MCU_Status {
public:
    enum class MCU_STATE {
        STARTUP,
        TRACTIVE_SYSTEM_INACTIVE,
        TRACTIVE_SYSTEM_ACTIVE,
        ENABLING_INVERTER,
        WAITING_READY_TO_DRIVE_SOUND,
        READY_TO_DRIVE
    };

    struct __attribute__((packed)) data_s {
        MCU_STATE mcu_state;
    };

    MCU_Status();

    data_s data{};

private:
    void sendMsg();
};
