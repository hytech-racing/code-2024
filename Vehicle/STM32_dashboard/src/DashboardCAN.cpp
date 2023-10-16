#include <DashboardCAN.h>
//this should be a class that takes in or inherits an STM32_CAN object so that we can
//use a different hytech_CAN object for each CAN interface

DashboardCAN::DashboardCAN(STM32_CAN* CAN) : _CAN(*CAN) {

    // begin can and set baud rate to 500kb
    _CAN.begin();
    _CAN.setBaudRate(500000);
    // CAN onReceive not supported in STM32 CAN library
    // instead of using interrupts on received can message
    // we will just poll the can bus for now
    // _CAN.enableMBInterrupts();
    // _CAN.onReceive();

}

void DashboardCAN::read_CAN() {
    CAN_message_t RX_message;
    _CAN.read(RX_message);

    // this is copied code from dashboard, needs to be translated
    // msg is now RX_message
    // not sure how heartbeat timer works, might need to be changed
    switch (RX_message.id) {
        case ID_MCU_STATUS:
      //        Serial.println("mcu status received");
      mcu_status.load(msg.buf);
      timer_mcu_heartbeat.reset();
      timer_mcu_heartbeat.interval(MCU_HEARTBEAT_TIMEOUT);
      mcu_status_received();
      break;
    case ID_MCU_ANALOG_READINGS:
      //        Serial.println("mcu analog readings received");
      mcu_analog_readings.load(msg.buf);
      timer_mcu_heartbeat.reset();
      timer_mcu_heartbeat.interval(MCU_HEARTBEAT_TIMEOUT);
      mcu_analog_readings_received();
      break;
    case ID_BMS_VOLTAGES:
      bms_voltages.load(msg.buf);
      //include bms timer
      bms_voltages_received();
    default:
      break;
    }

}