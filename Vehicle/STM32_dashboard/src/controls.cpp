#include "controls.h"

// initialize buttons
void controls::startup() {
    btn_safe_ctrl.begin(BTN_SAFE_CTRL, 100);
    btn_mc_cycle.begin(BTN_MC_CYCLE, 100);
    btn_start.begin(BTN_START, 100);
    btn_torque_mode.begin(BTN_TORQUE_MODE, 100);
    btn_led_dimmer.begin(BTN_LED_DIMMER, 100);
}

void controls::update(Dashboard_status* status) {
  // this sets the button to be high: it is set low in send can
  if (btn_safe_ctrl.isPressed())  {
    status->toggle_mode_btn();
  }
  if (btn_mc_cycle.isPressed())    {
    status->toggle_mc_cycle_btn();
  }
  if (btn_torque_mode.isPressed()) {
    status->toggle_torque_mode_btn();
  }
  if (btn_led_dimmer.isPressed())  {
    status->toggle_led_dimmer_btn();
  }
  status->set_start_btn(btn_start.isPressed());
}