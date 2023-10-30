#include "Dashboard_Controls.h"
#include "DashboardCAN.h"
#include "hytech_dashboard.h"

// initialize buttons
void Dashboard_Controls::startup() {
    btn_safe_ctrl.begin(BTN_SAFE_CTRL, 100);
    btn_mc_cycle.begin(BTN_MC_CYCLE, 100);
    btn_start.begin(BTN_START, 100);
    btn_torque_mode.begin(BTN_TORQUE_MODE, 100);
    btn_led_dimmer.begin(BTN_LED_DIMMER, 100);
}

void Dashboard_Controls::update(DashboardCAN* CAN) {
  // this sets the button to be high: it is set low in send can
  hytech_dashboard::getInstance()->set_neopixel(1, LED_OFF);
  if (btn_safe_ctrl.isPressed())  {
    CAN->dashboard_status.toggle_mode_btn();
    SerialUSB.println("BTN SAFE CTRL PRESSED");
    hytech_dashboard::getInstance()->set_neopixel(1, LED_YELLOW);
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
  CAN->dashboard_status.set_start_btn(btn_start.isPressed());
}