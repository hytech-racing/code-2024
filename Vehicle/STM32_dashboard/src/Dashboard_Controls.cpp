#include "Dashboard_Controls.h"
#include "DashboardCAN.h"
#include "hytech_dashboard.h"

// initialize buttons
void Dashboard_Controls::startup() {
    btn_start.begin(BTN_START, 10);
    btn_mark.begin(BTN_MARK, 10);
    btn_mc_cycle.begin(BTN_MC_CYCLE, 10);
    btn_torque_mode.begin(BTN_TORQUE_MODE, 10);
    btn_led_dimmer.begin(BTN_LED_DIMMER, 10);
    btn_left_shifter.begin(BTN_LEFT_SHIFTER, 10);
    btn_right_shifter.begin(BTN_RIGHT_SHIFTER, 10);

    pinMode(BUZZER_PIN, OUTPUT);

}

void Dashboard_Controls::update(DashboardCAN* CAN) {

  DASHBOARD_STATE_t new_state;

  if (btn_start.isPressed())
    new_state.start_button = true;

  if (btn_mark.isPressed())
    new_state.start_button = true;

  if (btn_mode.isPressed())
    new_state.mark_button = true;

  if (btn_mc_cycle.isPressed())
    new_state.motor_controller_cycle_button = true;

  // if (btn_launch_ctrl.isPressed())
  //   new_state.launch_ctrl_button = true;

  if (btn_torque_mode.isPressed())
    new_state.torque_mode_button = true;

  if (btn_led_dimmer.isPressed())
    new_state.led_dimmer_button = true;

  // if (btn_left_shifter.isPressed())
  //   new_state.left_shifter_button = true;

  // if (btn_right_shifter.isPressed())
  //   new_state.right_shifter_button = true;

  if (buzzer_active)
    new_state.drive_buzzer = true;

  //copy new state over previous dash state
  memcpy(&CAN->dash_state, &new_state, sizeof(DASHBOARD_STATE_t));

  // this sets the button to be high: it is set low in send can
  hytech_dashboard::getInstance()->set_neopixel(1, LED_OFF);
  if (btn_safe_ctrl.isPressed())  {
    CAN->dashboard_status.toggle_mode_btn();
    if(!previousState) {
      // SerialUSB.println("state incrementing");
      // SerialUSB.println("Previous state false");
      hytech_dashboard::getInstance()->increment_state();
      previousState = true;
    }
    SerialUSB.println("BTN SAFE CTRL PRESSED");
    hytech_dashboard::getInstance()->set_neopixel(1, LED_YELLOW);
  } else {
    // SerialUSB.println("Previous state reset");
    previousState = false;
  }
  if (btn_mc_cycle.isPressed())    {
    CAN->dashboard_status.toggle_mc_cycle_btn();
    SerialUSB.println("BTN MC CYCLE PRESSED");
    hytech_dashboard::getInstance()->set_neopixel(1, LED_BLUE);
  }
  if (btn_torque_mode.isPressed()) {
    CAN->dashboard_status.toggle_torque_mode_btn();
    SerialUSB.println("BTN TORQUE MODE PRESSED");
    hytech_dashboard::getInstance()->set_neopixel(1, LED_RED);
  }
  if (btn_led_dimmer.isPressed())  {
    CAN->dashboard_status.toggle_led_dimmer_btn();
    SerialUSB.println("BTN LED DIMMER PRESSED");
    hytech_dashboard::getInstance()->set_neopixel(1, LED_ON_GREEN);
  }
  if(btn_restart_timer.isPressed()) {
    SerialUSB.println("BTN RESTART TIMER PRESSED");
    hytech_dashboard::getInstance()->restart_current_timer();
  }
  CAN->dashboard_status.set_start_btn(btn_start.isPressed());
}