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
  inertia_read = false;
  brb_read = false;
}

void DashboardCAN::read_CAN()
{
  while (_CAN->read(_msg)) {
    // SerialUSB.printf("message received %d\n", _msg.id);
    // parse message based on ID
    switch (_msg.id) {

    case MCU_STATUS_CANID:
      // SerialUSB.println("Received MCU Status Reading");
      Unpack_MCU_STATUS_hytech(&mcu_status, _msg.buf, _msg.len);
      // reset heartbeat timer if message received from ECU
      heartbeat_timer.reset();
      heartbeat_timer.interval(MCU_HEARTBEAT_TIMEOUT);
      // mcu_status_received();
      break;

    case MCU_ANALOG_READINGS_CANID:
      Unpack_MCU_ANALOG_READINGS_hytech(&mcu_analog_readings, _msg.buf, _msg.len);
      heartbeat_timer.reset();
      heartbeat_timer.interval(MCU_HEARTBEAT_TIMEOUT);
      // mcu_analog_readings_received();
      break;

    case VN_STATUS_CANID:
      // SerialUSB.println("VN_status");
      Unpack_VN_STATUS_hytech(&vn_status, _msg.buf, _msg.len);
      break;

    case BMS_VOLTAGES_CANID:
      Unpack_BMS_VOLTAGES_hytech(&bms_voltages, _msg.buf, _msg.len);
      // bms_voltages.low_voltage_ro =  HYTECH_low_voltage_ro_fromS(bms_voltages.low_voltage_ro);
      // include bms timer
      // bms_voltages_received();
      break;

    case MCU_PEDAL_READINGS_CANID:
      // SerialUSB.println("Received MCU Pedal Reading");
      Unpack_MCU_PEDAL_READINGS_hytech(&mcu_pedal_readings, _msg.buf, _msg.len);
      break;

    case MCU_SUSPENSION_CANID:
      // SerialUSB.println("Received MCU Suspension");
      Unpack_MCU_SUSPENSION_hytech(&mcu_suspension, _msg.buf, _msg.len);
      break;

    case EM_MEASUREMENT_CANID:
      Unpack_EM_MEASUREMENT_hytech(&em_measurement, _msg.buf, _msg.len);
      // SerialUSB.println("Received EM measurement");
      break;

    case MCU_PEDAL_RAW_CANID:
      Unpack_MCU_PEDAL_RAW_hytech(&pedal_raw, _msg.buf, _msg.len);
      // SerialUSB.println("Received Pedal measurement");
      break;

    case DRIVETRAIN_RPMS_TELEM_CANID:
      // SerialUSB.println("Received Drivetrain RPMs");
      Unpack_DRIVETRAIN_RPMS_TELEM_hytech(&drivetrain_rpms, _msg.buf, _msg.len);
      // SerialUSB.println(drivetrain_rpms.fr_motor_rpm);
      break;

     case CONTROLLER_BOOLEAN_CANID:
      SerialUSB.println("Received Controller Boolean msg");
      Unpack_CONTROLLER_BOOLEAN_hytech(&controller_boolean, _msg.buf, _msg.len);
      break;

    case CONTROLLER_POWER_LIM_CANID:
      SerialUSB.println("Received Controller Power Lim msg");
      Unpack_CONTROLLER_POWER_LIM_hytech(&controller_power_lim, _msg.buf, _msg.len);
      break;
    
    case SAB_SUSPENSION_CANID:
      // SerialUSB.println("Received SAB Suspension");
      Unpack_SAB_SUSPENSION_hytech(&sab_suspension, _msg.buf, _msg.len);
      break;

    case TCU_LAP_TIMES_CANID:
      // SerialUSB.println("Received TCU lap times");
      Unpack_TCU_LAP_TIMES_hytech(&lap_times, _msg.buf, _msg.len);
      break;

    case TCU_DRIVER_MSG_CANID:
      // SerialUSB.println("Received TCU drive message");
      Unpack_TCU_DRIVER_MSG_hytech(&driver_msg, _msg.buf, _msg.len);
      break;


    case DASHBOARD_MCU_STATE_CANID:

      if (mcu_state_timer.check() || (&prev_dash_mcu_state, _msg.buf, sizeof(_msg.buf)) != 0) {
        int prev_brake_state = dash_mcu_state.mechanical_brake_led;
        Unpack_DASHBOARD_MCU_STATE_hytech(&dash_mcu_state, _msg.buf, _msg.len);
        dash_mcu_state.mechanical_brake_led = prev_brake_state;
        memcpy(&prev_dash_mcu_state, _msg.buf, sizeof(_msg.buf));
        mcu_state_update = true;
        mcu_state_timer.reset();
      } else {
        mcu_state_update = false;
      }
      // SerialUSB.printf("LEDS:\n%d\n%d\n%d\n",dash_mcu_state.ams_led, dash_mcu_state.imd_led, dash_mcu_state.bots_led);
      break;

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

  if ((should_send && send_timer.check())) {
    //update button flags
    // use hytech pack function, passing in reference to message and length
    // length will be set in pack function. Returns msg id.
    _msg.id = Pack_DASHBOARD_STATE_hytech(&dash_state, _msg.buf, &_msg.len, (uint8_t*) &_msg.flags.extended);
    _CAN->write(_msg);

    SerialUSB.printf("Message Sent %d\n", millis());
    memcpy(&prev_dash_state, &dash_state, sizeof(DASHBOARD_STATE_t));
    send_now = false;
    send_timer.reset();
  }

}

/*

If there is going to be this much LED logic, then this should
be moved out of DashCAN, possibly to its own neopixel class
that can handle the color logic and that inherits neopixel stuff

The exception may be with very important LEDs that need to be refreshed
immediately, but this should be used only if default refresh is decided to
be too slow. It better not be if it is driving a display.

*/