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

    for (int i = 0; i < DIAL_SIZE; i++){
      // walker u trolled
      pinMode(dial_pins[i], INPUT_PULLUP);
    }
}

void Dashboard_Controls::update(DashboardCAN* CAN) {
  
  if (btn_start.isPressed()) {
    SerialUSB.println("Start button pressed.");
  }

  if (btn_led_dimmer.isPressed() && dimmer_timer.check()) {
    hytech_dashboard::getInstance()->dim_neopixels();
    dimmer_timer.reset();
  }

  if (btn_mark.isPressed() && page_switch_timer.check()) {
    hytech_dashboard::getInstance()->increment_page();
    page_switch_timer.reset();
  }

  DASHBOARD_STATE_t* s = &CAN->dash_state;

  s->start_button = btn_start.isPressed();

  s->mark_button = btn_mark.isPressed();

  s->torque_mode_button = btn_torque_mode.isPressed();

  s->motor_controller_cycle_button = btn_mc_cycle.isPressed();

  s->led_dimmer_button = btn_led_dimmer.isPressed();

  s->left_shifter_button = btn_left_shifter.isPressed();

  s->right_shifter_button = btn_right_shifter.isPressed();

  for(int i = 0; i < DIAL_SIZE; i++){
    int pin_read = digitalRead(dial_pins[i]);
    if (!pin_read) {
      SerialUSB.print(dial_pins[i]);
      SerialUSB.print(": ");
      SerialUSB.println(DIAL_MODES[i]);
      s->dial_state = i;
      break;
    }
  }

  digitalWrite(BUZZER_PIN, CAN->dash_mcu_state.drive_buzzer);
  s->drive_buzzer = CAN->dash_mcu_state.drive_buzzer;

  if (CAN->dash_mcu_state.drive_buzzer) {
    SerialUSB.println("ACTIVE BUZZER");
  }

  // old code

  // // this sets the button to be high: it is set low in send can
  // hytech_dashboard::getInstance()->set_neopixel(1, LED_OFF);
  // if (btn_safe_ctrl.isPressed())  {
  //   CAN->dashboard_status.toggle_mode_btn();
  //   if(!previousState) {
  //     // SerialUSB.println("state incrementing");
  //     // SerialUSB.println("Previous state false");
  //     hytech_dashboard::getInstance()->increment_state();
  //     previousState = true;
  //   }
  //   SerialUSB.println("BTN SAFE CTRL PRESSED");
  //   hytech_dashboard::getInstance()->set_neopixel(1, LED_YELLOW);
  // } else {
  //   // SerialUSB.println("Previous state reset");
  //   previousState = false;
  // }
  // if (btn_mc_cycle.isPressed())    {
  //   CAN->dashboard_status.toggle_mc_cycle_btn();
  //   SerialUSB.println("BTN MC CYCLE PRESSED");
  //   hytech_dashboard::getInstance()->set_neopixel(1, LED_BLUE);
  // }
  // if (btn_torque_mode.isPressed()) {
  //   CAN->dashboard_status.toggle_torque_mode_btn();
  //   SerialUSB.println("BTN TORQUE MODE PRESSED");
  //   hytech_dashboard::getInstance()->set_neopixel(1, LED_RED);
  // }
  // if (btn_led_dimmer.isPressed())  {
  //   CAN->dashboard_status.toggle_led_dimmer_btn();
  //   SerialUSB.println("BTN LED DIMMER PRESSED");
  //   hytech_dashboard::getInstance()->set_neopixel(1, LED_ON_GREEN);
  // }
  // if(btn_restart_timer.isPressed()) {
  //   SerialUSB.println("BTN RESTART TIMER PRESSED");
  //   hytech_dashboard::getInstance()->restart_current_timer();
  // }
  // CAN->dashboard_status.set_start_btn(btn_start.isPressed());
}