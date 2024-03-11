#include "DebouncedButton.h"
#include "FlexCAN_T4.h"
#include "HyTech_CAN.h"
#include "MCP23S08.h"
#include "Metro.h"
#include "DialVectoring.h"
#include <Adafruit_NeoPixel.h>
#include "Dashboard.h"
// only send if receiving mcu status messages

// NEOPIXEL Variables
Adafruit_NeoPixel dashboard_neopixels(NEOPIXEL_COUNT, NEOPIXEL_CTRL, NEO_GRBW + NEO_KHZ800);
uint8_t curr_brightness = OUTSIDE_BRIGHTNESS;

Metro timer_led_ams   (LED_MIN_FAULT); //Do I need this?
Metro timer_led_imd   (LED_MIN_FAULT);
Metro timer_led_mc_err(LED_MIN_FAULT);

// bool init_ams = true, init_imd = true;

// Button debouncing variables
uint8_t prev_buttons{}, curr_buttons{}, temp_buttons{};
bool prev_start_state = 0;
DebouncedButton btn_safe_ctrl;
DebouncedButton btn_mc_cycle;
DebouncedButton btn_start;
DebouncedButton btn_torque_mode;
DebouncedButton btn_led_dimmer;
uint8_t toggle_led_dimmer = 0;
uint8_t prev_led_dimmer_state = 0;
//init dial variable
DialVectoring dial_torque_vectoring;

// CAN Variables
Metro timer_can_update = Metro(1000);
Metro timer_can_read = Metro(100);

FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> CAN;
CAN_message_t msg;


// CAN Messages
Dashboard_status dashboard_status{};
MCU_status mcu_status{};
MCU_analog_readings mcu_analog_readings;
BMS_voltages bms_voltages{};

// IO Expander Variables
MCP23S08 expander(IO_ADDR, IO_CS);
uint8_t number_encodings[11] = {0b01000000, 0b01111001, 0b00100100, 0b00110000, 0b00011001, 0b00010010, 0b00000010, 0b01111000, 0b10000000, 0b00011000, 0b11111111};
uint8_t display_list[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
uint8_t imd_ams_flags = 0;
Metro timer_mcu_heartbeat(0, 1);

inline void neopixel_update();
void read_can();
inline void btn_update();
inline void dial_update();
inline void mcu_status_received();
inline void mc_fault_codes_received();
inline void inertia_status();
inline void shutdown_signals_read();

void setup() {
  expander.begin();

  for (int i = 0; i < 8; i++) {
    expander.pinMode(i, OUTPUT);
    expander.digitalWrite(i, HIGH);
  }

  btn_safe_ctrl.begin(BTN_SAFE_CTRL, 100);
  btn_mc_cycle.begin(BTN_MC_CYCLE, 100);
  btn_start.begin(BTN_START, 100);
  btn_torque_mode.begin(BTN_TORQUE_MODE, 100);
  btn_led_dimmer.begin(BTN_LED_DIMMER, 100);

  int dial_pins[DIAL_SIZE] = {DIAL_MODE_ONE, DIAL_MODE_TWO, DIAL_ACCELERATION_LAUNCH_CONTROL, DIAL_SKIDPAD, DIAL_AUTOCROSS, DIAL_ENDURANCE};
  dial_torque_vectoring.begin(dial_pins, DIAL_SIZE, 50);



  pinMode(BUZZER_CTRL, OUTPUT);

  pinMode(BOTS_READ, INPUT);
  pinMode(INERTIA_READ, INPUT);
  pinMode(BRB_READ, INPUT);

  //Initializes CAN
  CAN.begin();
  CAN.setBaudRate(500000);
  CAN.enableMBInterrupts();
  CAN.onReceive(read_can);
  //
  //  mcu_status.set_imd_ok_high(true);
  //  mcu_status.set_bms_ok_high(true);


  neo_pixel_init();


  delay(1000);
}

void loop() {
  CAN.events();
  inertia_status();
  btn_update();
  dial_update();
  neopixel_update();

  //Serial.println(bms_voltages.get_low());


  static bool should_send = false;

  if (timer_mcu_heartbeat.check()) {
    timer_mcu_heartbeat.interval(0);
    should_send = false;
  }
  else {
    should_send = true;
  }


  prev_buttons = curr_buttons;
  curr_buttons = dashboard_status.get_button_flags();
  temp_buttons = curr_buttons & (curr_buttons ^ prev_buttons);




  // Send CAN message
  // Timer to ensure dashboard isn't flooding data bus, also fires after a button is pressed
  // How does the check for button press work
  // the xor against previous buttons removes the button flags that were sent previously
  // the and enforces that only buttons that are currently pressed are allowed to be sent
  if (should_send &&
      (timer_can_update.check() || (temp_buttons) || (prev_start_state != dashboard_status.get_start_btn()))
     ) {
    //  if (timer_can_update.check()) {

    msg.id = ID_DASHBOARD_STATUS;
    dashboard_status.set_button_flags(temp_buttons);
    //    Serial.println(temp_buttons, BIN);
    msg.len = sizeof(dashboard_status);
    dashboard_status.write(msg.buf);
    CAN.write(msg);
    //rest update timer
    timer_can_update.reset();
  }
  // clear buttons so they can be retoggled on in the loop
  dashboard_status.set_button_flags(0);
  prev_start_state = dashboard_status.get_start_btn();
}

inline void neo_pixel_init() {

  dashboard_neopixels.begin();
  dashboard_neopixels.setBrightness(curr_brightness);
  for (int i = 0; i < NEOPIXEL_COUNT - 1; i++) {

    dashboard_neopixels.setPixelColor(i, LED_INIT);
    if (i == 3) {
      dashboard_neopixels.setPixelColor(i, 0);
    }
    if (i == 0 || i == 1) {
      dashboard_neopixels.setPixelColor(i, LED_OFF);
    }

  }
  dashboard_neopixels.show();



}

inline void neopixel_update() {

  if (dashboard_status.get_led_dimmer_btn() == 1) {
    if (prev_led_dimmer_state == 0) {
      //set brightnesses
      toggle_led_dimmer ^= 0x1;
    }
    prev_led_dimmer_state = 1;


  } else {
    prev_led_dimmer_state = 0;
  }

  //
  if (toggle_led_dimmer) {
    curr_brightness = LOW_BRIGHTNESS;
  } else curr_brightness = OUTSIDE_BRIGHTNESS;


  //    if (brightness != prevBrightness)
  dashboard_neopixels.setBrightness(curr_brightness);

  shutdown_signals_read();
  dashboard_neopixels.show();
  // checks display list for first available flag

  // if no flags set, display turns off (writes 10th entry; sets all IO exp pins high)
//  for (int i = 0; i < 11; i++) {
//    if (display_list[i] == 1) {
//      expander.digitalWrite(number_encodings[i]);
//      break;
//    }
//  }


}

inline void dial_update() {
  dashboard_status.set_dial_state((uint8_t)(dial_torque_vectoring.readMode()));
  if (dashboard_status.get_dial_state() == 2) {
    dashboard_neopixels.setPixelColor(LED_LIST::LAUNCH_CTRL, LED_ON_GREEN);
    dashboard_status.set_launch_control_led(static_cast<uint8_t>(LED_MODES::ON));
  } else {
    dashboard_neopixels.setPixelColor(LED_LIST::LAUNCH_CTRL, LED_OFF);
    dashboard_status.set_launch_control_led(static_cast<uint8_t>(LED_MODES::OFF));
  }
  //display dial state on 7 segment
  expander.digitalWrite(number_encodings[dashboard_status.get_dial_state()]);
  
}
////inline void btn_toggle(void (Dashboard_status::*toggle)()) {
////  if (timer_toggle_button.check()) {
////
////    (dashboard_status.*toggle)();
////    Serial.println(dashboard_status.get_led_dimmer_btn());
////  }
////}
//inline void btn_update(){
//    // this sets the button to be high: it is set low in send can
//
////    if (btn_safe_ctrl.isPressed())        {
////      btn_toggle(&dashboard_status.toggle_mode_btn);        }
////    if (btn_mc_cycle.isPressed())    { btn_toggle(dashboard_status.toggle_mc_cycle_btn);    }
////    if (btn_torque_mode.isPressed()) { btn_toggle(dashboard_status.toggle_torque_mode_btn); }
////    if (btn_led_dimmer.isPressed())  { btn_toggle(dashboard_status.toggle_led_dimmer_btn); Serial.println(dashboard_status.get_led_dimmer_btn);}
////
//      if (btn_safe_ctrl.isPressed()) {
//        btn_toggle(&Dashboard_status::toggle_mode_btn);
//      }
//      if (btn_mc_cycle.isPressed()) {
//        btn_toggle(&Dashboard_status::toggle_mc_cycle_btn);
//      }
//      if (btn_torque_mode.isPressed()) {
//        btn_toggle(&Dashboard_status::toggle_torque_mode_btn);
//      }
//      if (btn_led_dimmer.isPressed()) {
//        btn_toggle(&Dashboard_status::toggle_led_dimmer_btn);
//
//      }
//
//    dashboard_status.set_start_btn(btn_start.isPressed());
//}

inline void btn_update() {
  // this sets the button to be high: it is set low in send can
  if (btn_safe_ctrl.isPressed())  {

    dashboard_status.toggle_mode_btn();
  }
  if (btn_mc_cycle.isPressed())    {

    dashboard_status.toggle_mc_cycle_btn();
  }
  if (btn_torque_mode.isPressed()) {

    dashboard_status.toggle_torque_mode_btn();
  }
  if (btn_led_dimmer.isPressed())  {
    dashboard_status.toggle_led_dimmer_btn();
  }

  dashboard_status.set_start_btn(btn_start.isPressed());
}

inline uint32_t color_wheel_glv(uint16_t voltage) {
  // 1V = 819 adc read, gain is 6.33 1v = 6.33V, max 4.737
  //Max voltage is 4.737 == 30 V
  //Threshold warning is 25 volts == 3.947
  //threshold warning is 24 volts = 3.789
  uint8_t max_voltage = 25;
  uint8_t min_threshold = 23;
  uint8_t mid_threshold = max_voltage - (max_voltage - min_threshold) / (float) 2;
  

  uint8_t converted_voltage = (float) voltage / 819 * 5.61; //make this a float
  int g = 255;
  int r = 255;
  if (converted_voltage > mid_threshold) {
    g = 255;
    r = map(voltage, mid_threshold * 819 / 5.61, max_voltage * 819 / 5.61, 255, 0);
  } else if (converted_voltage < mid_threshold) {
    r = 255;
    g = map(voltage, min_threshold * 819 / 5.61, mid_threshold * 819 / 5.61, 0, 255);
  } else {
    g = 255;
    r = 255;
  }

  if ( r < 0) {
    r = 0;
  } else if (r > 255) {
    r = 255;
  }

  if (g < 0) {
    g = 0;
  } else if ( g > 255) {
    g = 255;
  }


  return (r << 16) | (g << 8);


}



inline void shutdown_signals_read() { // if one trips al lwill go red, set the other ones orange
  dashboard_neopixels.setPixelColor(LED_LIST::BOTS, (digitalRead(BOTS_READ)) ? LED_ON_GREEN : LED_RED);
  dashboard_status.set_bots_led(digitalRead(BOTS_READ));
  dashboard_neopixels.setPixelColor(LED_LIST::COCKPIT_BRB, (digitalRead(BRB_READ) || !digitalRead(BRB_READ) && !digitalRead(INERTIA_READ)) ? LED_ON_GREEN : LED_RED);
  dashboard_status.set_cockpit_brb_led(digitalRead(BRB_READ));

}


inline void mcu_analog_readings_received() {

  if (mcu_analog_readings.get_glv_battery_voltage() < GLV_THRESHOLD) {

    dashboard_status.set_glv_led(static_cast<uint8_t>(LED_MODES::RED));
  } else {

    dashboard_status.set_glv_led(static_cast<uint8_t>(LED_MODES::ON));
  }
  dashboard_neopixels.setPixelColor(LED_LIST::GLV, color_wheel_glv(mcu_analog_readings.get_glv_battery_voltage()));

}
inline void bms_voltages_received() {
  //Read the lowest cell voltage. Max 4.2, min 3.3
  //voltages scaled by 10000
  uint16_t max_voltage = 36000 / 10;
  uint16_t min_threshold = PACK_THRESHOLD / 10;
  uint16_t mid_threshold = max_voltage - (max_voltage - min_threshold) / (float) 2;
  
  uint16_t bms_voltage = bms_voltages.get_low() / 10;
  int g = 255;
  int r = 255;
  if (bms_voltage > mid_threshold) {
    g = 255;
    r = map(bms_voltage, mid_threshold, max_voltage, 255, 0);


  } else if (bms_voltage < mid_threshold) {
    r = 255;
    g = map(bms_voltage, min_threshold, mid_threshold, 0, 255);

  } else {
    g = 255;
    r = 255;
  }

  if ( r < 0) {
    r = 0;
  } else if (r > 255) {
    r = 255;
  }

  if (g < 0) {
    g = 0;
  } else if ( g > 255) {
    g = 255;
  }



  dashboard_neopixels.setPixelColor(LED_LIST::CRIT_CHARGE, (r << 16) | (g << 8));
}
inline void mcu_status_received() {
  // control BUZZER_CTRL
  digitalWrite(BUZZER_CTRL, mcu_status.get_activate_buzzer());
  Serial.print("AMS: ");
  Serial.println(mcu_status.get_bms_ok_high());
  Serial.print("IMD: ");
  Serial.println(mcu_status.get_imd_ok_high());
  Serial.print("IMD/AMS flags: ");
  Serial.println(imd_ams_flags, BIN);
  if (mcu_status.get_bms_ok_high()) {

    if (((imd_ams_flags >> 1) & 1) == 0) {
      dashboard_neopixels.setPixelColor(LED_LIST::AMS, LED_OFF);
      dashboard_status.set_ams_led(static_cast<uint8_t>(LED_MODES::OFF));
    //display_list[4] = 1;
      imd_ams_flags |= (1 << 1);
    }

  } else if (((imd_ams_flags >> 1) & 1) == 1) {
    if(mcu_status.get_imd_ok_high()) {
    dashboard_neopixels.setPixelColor(LED_LIST::AMS, LED_RED);
    dashboard_status.set_ams_led(static_cast<uint8_t>(LED_MODES::RED));
    //display_list[4] = 0;
    }

  }

  //IMD LED
  if (mcu_status.get_imd_ok_high()) {
    if ((imd_ams_flags & 1) == 0) {
      dashboard_neopixels.setPixelColor(LED_LIST::IMD, LED_OFF);
      dashboard_status.set_imd_led(static_cast<uint8_t>(LED_MODES::OFF));
    //display_list[3] = 1;
      imd_ams_flags |= 1;
    }
  } else if ((imd_ams_flags & 1) == 1) {
    
      dashboard_neopixels.setPixelColor(LED_LIST::IMD, LED_RED);
      dashboard_status.set_imd_led(static_cast<uint8_t>(LED_MODES::RED));
    //display_list[3] = 0;

    
  } 

  //Start LED
  switch (mcu_status.get_state()) {
    case MCU_STATE::STARTUP:
      dashboard_neopixels.setPixelColor(LED_LIST::RDY_DRIVE, LED_OFF);
      dashboard_status.set_start_led(static_cast<uint8_t>(LED_MODES::OFF));
      break;
    case MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE:
      dashboard_neopixels.setPixelColor(LED_LIST::RDY_DRIVE, LED_OFF);
      dashboard_status.set_start_led(static_cast<uint8_t>(LED_MODES::RED));
      break;
    case MCU_STATE::TRACTIVE_SYSTEM_ACTIVE:

      dashboard_neopixels.setPixelColor(LED_LIST::RDY_DRIVE, LED_ON_GREEN);
      dashboard_status.set_start_led(static_cast<uint8_t>(LED_MODES::YELLOW));
      break;
    case MCU_STATE::ENABLING_INVERTER:
    case MCU_STATE::WAITING_READY_TO_DRIVE_SOUND:
    case MCU_STATE::READY_TO_DRIVE:

      dashboard_neopixels.setPixelColor(LED_LIST::RDY_DRIVE, LED_BLUE);
      dashboard_status.set_start_led(static_cast<uint8_t>(LED_MODES::ON));
      break;
    default:

      dashboard_neopixels.setPixelColor(LED_LIST::RDY_DRIVE, LED_OFF);
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

      dashboard_neopixels.setPixelColor(LED_LIST::TORQUE_MODE, LED_OFF);
      dashboard_status.set_mode_led(static_cast<uint8_t>(LED_MODES::OFF));
      break;
    case 2:
      dashboard_neopixels.setPixelColor(LED_LIST::TORQUE_MODE, LED_YELLOW);
      dashboard_status.set_mode_led(static_cast<uint8_t>(LED_MODES::YELLOW));
      break;
    case 3:
      dashboard_neopixels.setPixelColor(LED_LIST::TORQUE_MODE, LED_ON_GREEN);
      dashboard_status.set_mode_led(static_cast<uint8_t>(LED_MODES::ON));
      break;
    default:
      //led_mode.setMode(LED_MODES::OFF);
      //dashboard_status.set_mode_led(static_cast<uint8_t>(LED_MODES::OFF));
      break;
  }

  //Mechanical Braking LED
  if (!mcu_status.get_mech_brake_active()) {
    dashboard_neopixels.setPixelColor(LED_LIST::BRAKE_ENGAGE, LED_OFF);
    dashboard_status.set_mech_brake_led(static_cast<uint8_t>(LED_MODES::OFF));

  } else {
    dashboard_neopixels.setPixelColor(LED_LIST::BRAKE_ENGAGE, LED_BLUE);
    dashboard_status.set_mech_brake_led(static_cast<uint8_t>(LED_MODES::ON));


  }

  if (!mcu_status.get_launch_ctrl_active()) {
    dashboard_neopixels.setPixelColor(LED_LIST::LAUNCH_CTRL, LED_OFF);
    dashboard_status.set_launch_control_led(static_cast<uint8_t>(LED_MODES::OFF));
  } else {
    dashboard_neopixels.setPixelColor(LED_LIST::LAUNCH_CTRL, LED_ON_GREEN);
    dashboard_status.set_launch_control_led(static_cast<uint8_t>(LED_MODES::ON));
  }

  if (!mcu_status.get_inverters_error()){
    dashboard_neopixels.setPixelColor(LED_LIST::MC_ERR, LED_ON_GREEN);
    dashboard_status.set_mc_error_led(0);
  } else {
    dashboard_neopixels.setPixelColor(LED_LIST::MC_ERR, LED_YELLOW);
    dashboard_status.set_mc_error_led(1);
  }
}

//inline void mc_fault_codes_received(){
//    bool is_mc_err = false;
//
//    if (mc_fault_codes.get_post_fault_hi() ||
//        mc_fault_codes.get_post_fault_lo() ||
//        mc_fault_codes.get_run_fault_hi() ||
//        mc_fault_codes.get_run_fault_lo())
//    {
//        is_mc_err = true;
//    }
//    //MC Error LED
//
//    if (is_mc_err){
//        dashboard_neopixels.setPixelColor(LED_LIST::MC_ERR, LED_RED);
//        dashboard_status.set_mc_error_led(static_cast<uint8_t>(LED_MODES::ON));
//        display_list[2] = 1;
//        timer_led_mc_err.reset();
//    // display fault for 1 second and then it clears
//    } else if (dashboard_neopixels.getPixelColor(LED_LIST::MC_ERR) != LED_OFF && timer_led_mc_err.check()){
//        dashboard_neopixels.setPixelColor(LED_LIST::MC_ERR, LED_OFF);
//        dashboard_status.set_mc_error_led(static_cast<uint8_t>(LED_MODES::OFF));
//        display_list[2] = 0;
//    }
//
//    /*if (is_mc_err){
//        led_mc_err.setMode(LED_MODES::ON);
//        dashboard_status.set_mc_error_led(static_cast<uint8_t>(LED_MODES::ON));
//        timer_led_mc_err.reset();
//    }
//    else if (led_mc_err.getMode() != LED_MODES::OFF && timer_led_mc_err.check()){
//        led_mc_err.setMode(LED_MODES::SLOW);
//        dashboard_status.set_mc_error_led(static_cast<uint8_t>(LED_MODES::SLOW));
//    }*/
//}
void read_can(const CAN_message_t &msg) {
  switch (msg.id) {
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

inline void inertia_status() {
  if (digitalRead(INERTIA_READ) || !digitalRead(INERTIA_READ) && !digitalRead(BOTS_READ)) {

    dashboard_neopixels.setPixelColor(LED_LIST::INERTIA, LED_ON_GREEN);
    dashboard_status.set_inertia_led(static_cast<uint8_t>(LED_MODES::OFF));
//    display_list[1] = 1;
  } else {
    
        dashboard_neopixels.setPixelColor(LED_LIST::INERTIA, LED_RED);
    dashboard_status.set_inertia_led(static_cast<uint8_t>(LED_MODES::ON));
//    display_list[1] = 0;
  }
}
