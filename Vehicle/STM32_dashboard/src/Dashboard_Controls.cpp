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

  /* Read Buttons */
  
  if (btn_start.isPressed()) {
    // SerialUSB.println("Start button pressed.");
  }

  if (btn_led_dimmer.isPressed() && dimmer_timer.check()) {
    hytech_dashboard::getInstance()->dim_neopixels();
    dimmer_timer.reset();
  }

  if (btn_mark.isPressed() && page_switch_timer.check()) {
    hytech_dashboard::getInstance()->increment_page();
    page_switch_timer.reset();
  }

  if (btn_left_shifter.isPressed() && page_switch_timer.check()) {
    hytech_dashboard::getInstance()->decrement_page();
    page_switch_timer.reset();
  }

  if (btn_right_shifter.isPressed() && page_switch_timer.check()) {
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
      s->dial_state = i;
      break;
    }
  }

  /* Update Buzzer */
  if (CAN->dash_mcu_state.drive_buzzer
      && !buzzer_cur_state
      && (millis() - buzzer_last_millis) > BUZZER_TIME * 2) {
    digitalWrite(BUZZER_PIN, HIGH);
    buzzer_cur_state = true;
    buzzer_timer.reset();
  } else if (buzzer_cur_state && buzzer_timer.check()) {
    digitalWrite(BUZZER_PIN, LOW);
    buzzer_cur_state = false;
    buzzer_last_millis = millis();
  }

  s->drive_buzzer = buzzer_cur_state;


  /* Read Shutdown Signals*/

  CAN->inertia_read = digitalRead(SHUTDOWN_J_INERTIA_PIN);
  CAN->brb_read = digitalRead(SHUTDOWN_K_COKPIT_BRB_PIN);
}