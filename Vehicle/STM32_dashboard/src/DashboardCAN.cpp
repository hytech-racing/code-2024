#include <DashboardCAN.h>
// this should be a class that takes in or inherits an STM32_CAN object so that we can
// use a different hytech_CAN object for each CAN interface

// ctor
DashboardCAN::DashboardCAN(STM32_CAN* CAN)
{
  _CAN = CAN;

  // begin can and set baud rate to 500kb
  _CAN->begin();
  _CAN->setBaudRate(500000);
  // CAN onReceive not supported in STM32 CAN library
  // instead of using interrupts on received can message
  // we will just poll the can bus for now
  // _CAN.enableMBInterrupts();
  // _CAN.onReceive();
}

void DashboardCAN::read_CAN()
{
  if (_CAN->read(_msg)) {
    // SerialUSB.println("message received");
    // parse message based on ID
    switch (_msg.id) {
    case ID_MCU_STATUS:
      mcu_status.load(_msg.buf);
      // reset heartbeat timer if message received from ECU
      heartbeat_timer.reset();
      heartbeat_timer.interval(MCU_HEARTBEAT_TIMEOUT);
      mcu_status_received();
      break;
    case ID_MCU_ANALOG_READINGS:
      mcu_analog_readings.load(_msg.buf);
      heartbeat_timer.reset();
      heartbeat_timer.interval(MCU_HEARTBEAT_TIMEOUT);
      mcu_analog_readings_received();
      break;
    case ID_BMS_VOLTAGES:
      bms_voltages.load(_msg.buf);
      // include bms timer
      bms_voltages_received();
      break;
    case ID_MCU_PEDAL_READINGS:
      // SerialUSB.println("Received MCU Pedal Reading");
      pedal_readings.load(_msg.buf);
      break;
    case ID_SAB_LAP_TIMES:
      // SerialUSB.println("Received SAB Lap Time");
      lap_times.load(_msg.buf);
    default:
      break;
    }
  }
}

void DashboardCAN::send_status() {
  // boolean to prevent CAN flooding
  static bool should_send = true;

  // if heartbeat timer is true, set interval to 0
  // acts as a latch to prevent sending message until
  // new CAN message is read
  // if (heartbeat_timer.check()) {
  //   heartbeat_timer.interval(0);
  //   should_send = false;
  // } else {
  //   should_send = true;
  // }

  if (should_send && send_timer.check()) {
    _msg.id = ID_DASHBOARD_STATUS;
    //update button flags
    // SerialUSB.println("Message Sent");
    _msg.len = sizeof(dashboard_status);
    dashboard_status.write(_msg.buf);
    _CAN->write(_msg);

    send_timer.reset();
  }

}

uint32_t DashboardCAN::color_wheel_bms_glv(bool isBms) {

  int r = 255;
  int g = 255;

  if(!isBms) {
    // for glv
    uint8_t max_voltage = 25;
    uint8_t min_threshold = 23;
    uint8_t mid_threshold = max_voltage - (max_voltage - min_threshold) / (float) 2;
    uint8_t converted_voltage = (float) mcu_analog_readings.get_glv_battery_voltage() / 819 * 5.61;
    if (converted_voltage > mid_threshold) {
      g = 255;
      r = map(mcu_analog_readings.get_glv_battery_voltage(), mid_threshold * 819 / 5.61, max_voltage * 819 / 5.61, 255, 0);
    } else if (converted_voltage < mid_threshold) {
      r = 255;
      g = map(mcu_analog_readings.get_glv_battery_voltage(), min_threshold * 819 / 5.61, mid_threshold * 819 / 5.61, 0, 255);
    } else {
      g = 255;
      r = 255;
    }
  } else {
    // for bms
    uint16_t max_voltage = 36000 / 10;
    uint16_t min_threshold = PACK_THRESHOLD / 10;
    uint16_t mid_threshold = max_voltage - (max_voltage - min_threshold) / (float)2;
    uint16_t bms_voltage = bms_voltages.get_low() / 10;
    if (bms_voltage > mid_threshold) {
      g = 255;
      r = map(bms_voltage, mid_threshold, max_voltage, 255, 0);
    }
    else if (bms_voltage < mid_threshold) {
      r = 255;
      g = map(bms_voltage, min_threshold, mid_threshold, 0, 255);
    } else {
      g = 255;
      r = 255;
    }
  }   
  // clamp r and g in the range [0,255]
  r = (r < 0) ? 0 : ((r > 255) ? 255 : r);
  g = (g < 0) ? 0 : ((g > 255) ? 255 : g);
  return (r << 16) | (g << 8);
}

void DashboardCAN::mcu_status_received() {
  // control BUZZER_CTRL
  // digitalWrite(BUZZER_CTRL, mcu_status.get_activate_buzzer());
  SerialUSB.print("AMS: ");
  SerialUSB.println(mcu_status.get_bms_ok_high());
  SerialUSB.print("IMD: ");
  SerialUSB.println(mcu_status.get_imd_ok_high());
  SerialUSB.print("IMD/AMS flags: ");
  // SerialUSB.println(imd_ams_flags, BIN);  no work idk why
  if (mcu_status.get_bms_ok_high()) {
    if (((imd_ams_flags >> 1) & 1) == 0) {
      // hytech_dashboard::set_neopixel(LED_LIST::AMS, LED_OFF);
      dashboard_status.set_ams_led(static_cast<uint8_t>(LED_MODES::OFF));
    //display_list[4] = 1;
      imd_ams_flags |= (1 << 1);
    }
  } else if (((imd_ams_flags >> 1) & 1) == 1) {
    if(mcu_status.get_imd_ok_high()) {
    hytech_dashboard::getInstance()->set_neopixel(LED_LIST::AMS, LED_RED);
    dashboard_status.set_ams_led(static_cast<uint8_t>(LED_MODES::RED));
    //display_list[4] = 0;
    }
  }

  //IMD LED
  if (mcu_status.get_imd_ok_high()) {
    if ((imd_ams_flags & 1) == 0) {
      hytech_dashboard::getInstance()->set_neopixel(LED_LIST::IMD, LED_OFF);
      dashboard_status.set_imd_led(static_cast<uint8_t>(LED_MODES::OFF));
    //display_list[3] = 1;
      imd_ams_flags |= 1;
    }
  } else if ((imd_ams_flags & 1) == 1) {
      hytech_dashboard::getInstance()->set_neopixel(LED_LIST::IMD, LED_RED);
      dashboard_status.set_imd_led(static_cast<uint8_t>(LED_MODES::RED));
    //display_list[3] = 0;    
  } 

  //Start LED
  switch (mcu_status.get_state()) {
    case MCU_STATE::STARTUP:
      hytech_dashboard::getInstance()->set_neopixel(LED_LIST::RDY_DRIVE, LED_OFF);
      dashboard_status.set_start_led(static_cast<uint8_t>(LED_MODES::OFF));
      break;
    case MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE:
      hytech_dashboard::getInstance()->set_neopixel(LED_LIST::RDY_DRIVE, LED_OFF);
      dashboard_status.set_start_led(static_cast<uint8_t>(LED_MODES::RED));
      break;
    case MCU_STATE::TRACTIVE_SYSTEM_ACTIVE:
      hytech_dashboard::getInstance()->set_neopixel(LED_LIST::RDY_DRIVE, LED_ON_GREEN);
      dashboard_status.set_start_led(static_cast<uint8_t>(LED_MODES::YELLOW));
      break;
    case MCU_STATE::ENABLING_INVERTER:
    case MCU_STATE::WAITING_READY_TO_DRIVE_SOUND:
    case MCU_STATE::READY_TO_DRIVE:
      hytech_dashboard::getInstance()->set_neopixel(LED_LIST::RDY_DRIVE, LED_BLUE);
      dashboard_status.set_start_led(static_cast<uint8_t>(LED_MODES::ON));
      break;
    default:
      hytech_dashboard::getInstance()->set_neopixel(LED_LIST::RDY_DRIVE, LED_OFF);
      dashboard_status.set_start_led(static_cast<uint8_t>(LED_MODES::OFF));
      break;
  }

  //Critical Charge LED // handled by reading BMS lowest value

  switch (mcu_status.get_pack_charge_critical()) {
    case 1: // GREEN, OK
      dashboard_status.set_crit_charge_led(static_cast<uint8_t>(LED_MODES::ON));
      break;
    case 2: // YELLOW, WARNING
      dashboard_status.set_crit_charge_led(static_cast<uint8_t>(LED_MODES::YELLOW));
      break;
    case 3: // RED, CRITICAL
      dashboard_status.set_crit_charge_led(static_cast<uint8_t>(LED_MODES::RED));
      break;
    default:
      dashboard_status.set_crit_charge_led(static_cast<uint8_t>(LED_MODES::OFF));
      break;
  }

  // Mode LED
  switch (mcu_status.get_torque_mode()) {
    case 1:
      hytech_dashboard::getInstance()->set_neopixel(LED_LIST::TORQUE_MODE, LED_OFF);
      dashboard_status.set_mode_led(static_cast<uint8_t>(LED_MODES::OFF));
      break;
    case 2:
      hytech_dashboard::getInstance()->set_neopixel(LED_LIST::TORQUE_MODE, LED_YELLOW);
      dashboard_status.set_mode_led(static_cast<uint8_t>(LED_MODES::YELLOW));
      break;
    case 3:
      hytech_dashboard::getInstance()->set_neopixel(LED_LIST::TORQUE_MODE, LED_ON_GREEN);
      dashboard_status.set_mode_led(static_cast<uint8_t>(LED_MODES::ON));
      break;
    default:
      //led_mode.setMode(LED_MODES::OFF);
      //dashboard_status.set_mode_led(static_cast<uint8_t>(LED_MODES::OFF));
      break;
  }

  //Mechanical Braking LED
  if (!mcu_status.get_mech_brake_active()) {
    hytech_dashboard::getInstance()->set_neopixel(LED_LIST::BRAKE_ENGAGE, LED_OFF);
    dashboard_status.set_mech_brake_led(static_cast<uint8_t>(LED_MODES::OFF));
  } else {
    hytech_dashboard::getInstance()->set_neopixel(LED_LIST::BRAKE_ENGAGE, LED_BLUE);
    dashboard_status.set_mech_brake_led(static_cast<uint8_t>(LED_MODES::ON));
  }

  if (!mcu_status.get_launch_ctrl_active()) {
    hytech_dashboard::getInstance()->set_neopixel(LED_LIST::LAUNCH_CTRL, LED_OFF);
    dashboard_status.set_launch_control_led(static_cast<uint8_t>(LED_MODES::OFF));
  } else {
    hytech_dashboard::getInstance()->set_neopixel(LED_LIST::LAUNCH_CTRL, LED_ON_GREEN);
    dashboard_status.set_launch_control_led(static_cast<uint8_t>(LED_MODES::ON));
  }

  if (!mcu_status.get_inverters_error()){
    hytech_dashboard::getInstance()->set_neopixel(LED_LIST::MC_ERR, LED_ON_GREEN);
    dashboard_status.set_mc_error_led(0);
  } else {
    hytech_dashboard::getInstance()->set_neopixel(LED_LIST::MC_ERR, LED_YELLOW);
    dashboard_status.set_mc_error_led(1);
  }
}

void DashboardCAN::mcu_analog_readings_received() {
  if (mcu_analog_readings.get_glv_battery_voltage() < GLV_THRESHOLD) {
    dashboard_status.set_glv_led(static_cast<uint8_t>(LED_MODES::RED));
  } else {
    dashboard_status.set_glv_led(static_cast<uint8_t>(LED_MODES::ON));
  }
  hytech_dashboard::getInstance()->set_neopixel(LED_LIST::GLV, color_wheel_bms_glv(false));
}

void DashboardCAN::bms_voltages_received() {
  hytech_dashboard::getInstance()->set_neopixel(LED_LIST::CRIT_CHARGE, color_wheel_bms_glv(true));
}