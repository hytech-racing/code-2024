#include "Dashboard.h"
#include "DebouncedButton.h"
#include "FlexCAN_T4.h"
#include "HyTech_CAN.h"
#include "MCP23S08.h"
#include "Metro.h"
#include "DialVectoring.h"
#include <Adafruit_NeoPixel.h>

// only send if receiving mcu status messages

// NEOPIXEL Variables
Adafruit_NeoPixel dashboard_neopixels(NEOPIXEL_COUNT, NEOPIXEL_CTRL, NEO_GRBW + NEO_KHZ800);
uint8_t brightness = 255;

Metro timer_led_ams   (LED_MIN_FAULT); //Do I need this?
Metro timer_led_imd   (LED_MIN_FAULT);
Metro timer_led_mc_err(LED_MIN_FAULT);

// bool init_ams = true, init_imd = true;

// Button debouncing variables
DebouncedButton btn_mode;
DebouncedButton btn_mc_cycle;
DebouncedButton btn_start;
DebouncedButton btn_torque_mode;
DebouncedButton btn_led_dimmer;


//init dial variable
DialVectoring dial_torque_vectoring;

// CAN Variables
Metro timer_can_update = Metro(100);
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> CAN;
CAN_message_t msg;


// CAN Messages
Dashboard_status dashboard_status{};
MC_fault_codes mc_fault_codes{};
MCU_status mcu_status{};
MCU_analog_readings mcu_analog_readings;

// IO Expander Variables
MCP23S08 expander(IO_ADDR, IO_CS);

Metro timer_mcu_heartbeat(0, 1);

inline void neopixel_update();
inline void read_can();
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
  
    btn_mode.begin(BTN_MODE, 100);
    btn_mc_cycle.begin(BTN_MC_CYCLE, 100);
    btn_start.begin(BTN_START, 100);
    btn_torque_mode.begin(BTN_TORQUE_MODE, 100);
    btn_led_dimmer.begin(BTN_LED_DIMMER, 100);

    int dial_pins[DIAL_SIZE] = {DIAL_MODE_ONE, DIAL_MODE_TWO, DIAL_ACCELERATION_LAUNCH_CONTROL, DIAL_SKIDPAD, DIAL_AUTOCROSS, DIAL_ENDURANCE};
    dial_torque_vectoring.begin(dial_pins, DIAL_SIZE, 100);

    

    pinMode(BUZZER_CTRL,OUTPUT);

    pinMode(SSOK_READ, INPUT);
    pinMode(INERTIA_READ, INPUT);
    pinMode(SHUTDOWN_H_READ, INPUT);

    //Initializes CAN
    CAN.begin();
    CAN.setBaudRate(500000);

    mcu_status.set_imd_ok_high(true);
    mcu_status.set_bms_ok_high(true);

    dashboard_neopixels.begin();
    dashboard_neopixels.setPixelColor(LED_LIST::AMS, LED_ON_GREEN);
    dashboard_neopixels.show();
    dashboard_neopixels.setBrightness(brightness);

    delay(7000);
}

void loop() {
    read_can();
    inertia_status();
    neopixel_update();
    btn_update();

    static bool should_send = false;

    if (timer_mcu_heartbeat.check()){
        timer_mcu_heartbeat.interval(0);
        should_send = false;
    }
    else {
        should_send = true;
    }

    static uint8_t prev_buttons{}, curr_buttons{}, temp_buttons{};
    prev_buttons = curr_buttons;
    curr_buttons = dashboard_status.get_button_flags();
    temp_buttons = curr_buttons & (curr_buttons ^ prev_buttons);
    static bool prev_start_state;


    // Send CAN message
    // Timer to ensure dashboard isn't flooding data bus, also fires after a button is pressed
    // How does the check for button press work
    // the xor against previous buttons removes the button flags that were sent previously
    // the and enforces that only buttons that are currently pressed are allowed to be sent
    if(should_send &&
        (timer_can_update.check() || (temp_buttons) || (prev_start_state != dashboard_status.get_start_btn()))
      ){
        //create message to send
        msg.id = ID_DASHBOARD_STATUS;
        dashboard_status.set_button_flags(temp_buttons);
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

inline void neopixel_update(){
    uint8_t prevBrightness = brightness;
    if (dashboard_status.get_led_dimmer_btn()) brightness = 128;
    else brightness = 255;
    
    if (brightness != prevBrightness) dashboard_neopixels.setBrightness(brightness);
    
    shutdown_signals_read();
    dashboard_neopixels.show();
    // checks display list for first available flag
    
    // if no flags set, display turns off (writes 10th entry; sets all IO exp pins high)
    for (int i = 0; i < 11; i++) {
        if (display_list[i] == 1) {
            expander.digitalWrite(number_encodings[i]);
            break;
        } 
    }
}

inline void dial_update(){
  dashboard_status.set_dial_state((dial_torque_vectoring.readMode()));
  if(dashboard_status.get_dial_state() == 2){
    dashboard_neopixels.setPixelColor(LED_LIST::LAUNCH_CTRL, LED_ON_GREEN);
    dashboard_status.set_launch_control_led(static_cast<uint8_t>(LED_MODES::ON));
  } else {
    dashboard_neopixels.setPixelColor(LED_LIST::LAUNCH_CTRL, LED_OFF);
    dashboard_status.set_launch_control_led(static_cast<uint8_t>(LED_MODES::OFF));
  }
}
inline void btn_update(){
    // this sets the button to be high: it is set low in send can
    if (btn_mode.isPressed())        { dashboard_status.toggle_mode_btn();        }
    if (btn_mc_cycle.isPressed())    { dashboard_status.toggle_mc_cycle_btn();    }
    if (btn_torque_mode.isPressed()) { dashboard_status.toggle_torque_mode_btn(); }
    if (btn_led_dimmer.isPressed())  { dashboard_status.toggle_led_dimmer_btn();  }

    dashboard_status.set_start_btn(btn_start.isPressed());
}
inline void shutdown_signals_read() {
  dashboard_neopixels.setPixelColor(LED_LIST::BOTS, (digitalRead(SSOK_READ)) ? LED_RED : LED_OFF);
  dashboard_neopixels.setPixelColor(LED_LIST::COCKPIT_BRB, (digitalRead(SHUTDOWN_H_READ)) ? LED_RED : LED_OFF);
}
inline void read_can(){

    while(CAN.read(msg)){

        switch(msg.id){
            case ID_MCU_STATUS:
                mcu_status.load(msg.buf);
                timer_mcu_heartbeat.reset();
                timer_mcu_heartbeat.interval(MCU_HEARTBEAT_TIMEOUT);
                mcu_status_received();
                break;
            case ID_MCU_ANALOG_READINGS:
                mcu_analog_readings.load(msg.buf);
                timer_mcu_heartbeat.reset();
                timer_mcu_heartbeat.interval(MCU_HEARTBEAT_TIMEOUT);
                mcu_analog_readings_received();
                break;
                
            case ID_MC_FAULT_CODES:
                mc_fault_codes.load(msg.buf);
                mc_fault_codes_received();
                break;
            default:
                break;
        }
    }
}

inline void mcu_analog_readings_received() {
 
    if(mcu_analog_readings.get_glv_battery_voltage() < GLV_THRESHOLD) {
      dashboard_neopixels.setPixelColor(LED_LIST::GLV, LED_RED); 
      dashboard_status.set_glv_led(static_cast<uint8_t>(LED_MODES::RED));
  } else {
      dashboard_neopixels.setPixelColor(LED_LIST::GLV, LED_ON_GREEN); 
      dashboard_status.set_glv_led(static_cast<uint8_t>(LED_MODES::ON));
  }
  
  
}
inline void mcu_status_received(){
    // control BUZZER_CTRL
    digitalWrite(BUZZER_CTRL, mcu_status.get_activate_buzzer());

    //BMS/AMS LED (bms and ams are the same thing)
    if (mcu_status.get_bms_ok_high()){
        dashboard_neopixels.setPixelColor(LED_LIST::AMS, LED_ON_GREEN);
        dashboard_status.set_ams_led(static_cast<uint8_t>(LED_MODES::ON));
        display_list[4] = 1;
        timer_led_ams.reset();
    }
    // else if (init_ams){
    //     led_ams.setMode(LED_MODES::OFF);
    //     dashboard_status.set_ams_led(static_cast<uint8_t>(LED_MODES::OFF));
    //     init_ams = false;
    // }
    else if (dashboard_neopixels.getPixelColor(LED_LIST::AMS) != LED_OFF && timer_led_ams.check()){
  
        dashboard_neopixels.setPixelColor(LED_LIST::AMS, LED_RED);
        dashboard_status.set_ams_led(static_cast<uint8_t>(LED_MODES::RED));
        display_list[4] = 0;
    }

    //IMD LED
    if (mcu_status.get_imd_ok_high()){
        dashboard_neopixels.setPixelColor(LED_LIST::IMD, LED_ON_GREEN);
        dashboard_status.set_imd_led(static_cast<uint8_t>(LED_MODES::ON));
        display_list[3] = 1;
        timer_led_imd.reset();
    }
    // else if (init_imd){
    //     led_imd.setMode(LED_MODES::OFF);
    //     dashboard_status.set_imd_led(static_cast<uint8_t>(LED_MODES::OFF));
    //     init_imd = false;
    // }
    else if (dashboard_neopixels.getPixelColor(LED_LIST::IMD) != LED_OFF && timer_led_imd.check()){
  
        dashboard_neopixels.setPixelColor(LED_LIST::IMD, LED_RED);
        dashboard_status.set_imd_led(static_cast<uint8_t>(LED_MODES::RED));
        display_list[3] = 0;
    }

    //Start LED
    switch(mcu_status.get_state()){
        case MCU_STATE::STARTUP:
            dashboard_neopixels.setPixelColor(LED_LIST::RDY_DRIVE, LED_OFF);
            dashboard_status.set_start_led(static_cast<uint8_t>(LED_MODES::OFF));
            break;
        case MCU_STATE::TRACTIVE_SYSTEM_NOT_ACTIVE:
            dashboard_neopixels.setPixelColor(LED_LIST::RDY_DRIVE, LED_RED);
            dashboard_status.set_start_led(static_cast<uint8_t>(LED_MODES::RED));
            break;
        case MCU_STATE::TRACTIVE_SYSTEM_ACTIVE:
        
            dashboard_neopixels.setPixelColor(LED_LIST::RDY_DRIVE, LED_YELLOW);
            dashboard_status.set_start_led(static_cast<uint8_t>(LED_MODES::YELLOW));
            break;
        case MCU_STATE::ENABLING_INVERTER:
        case MCU_STATE::WAITING_READY_TO_DRIVE_SOUND:
        case MCU_STATE::READY_TO_DRIVE:
           
            dashboard_neopixels.setPixelColor(LED_LIST::RDY_DRIVE, LED_ON_GREEN);
            dashboard_status.set_start_led(static_cast<uint8_t>(LED_MODES::ON));
            break;
        default:
            
            dashboard_neopixels.setPixelColor(LED_LIST::RDY_DRIVE, LED_OFF);
            dashboard_status.set_start_led(static_cast<uint8_t>(LED_MODES::OFF));
            break;
    }

    //Critical Charge LED

    switch(mcu_status.get_pack_charge_critical()){
      case 1: // GREEN, OK
        dashboard_neopixels.setPixelColor(LED_LIST::CRIT_CHARGE, LED_ON_GREEN); 
        dashboard_status.set_mode_led(static_cast<uint8_t>(LED_MODES::ON));
        break;
      case 2: // YELLOW, WARNING
        dashboard_neopixels.setPixelColor(LED_LIST::CRIT_CHARGE, LED_YELLOW); 
        dashboard_status.set_mode_led(static_cast<uint8_t>(LED_MODES::YELLOW));
        break;
      case 3: // RED, CRITICAL
        dashboard_neopixels.setPixelColor(LED_LIST::CRIT_CHARGE, LED_RED); 
        dashboard_status.set_mode_led(static_cast<uint8_t>(LED_MODES::RED));
        break;
      default: 
        dashboard_neopixels.setPixelColor(LED_LIST::CRIT_CHARGE, LED_OFF); 
        dashboard_status.set_mode_led(static_cast<uint8_t>(LED_MODES::OFF));
        break;
    }

    // Mode LED
    switch(mcu_status.get_torque_mode()){
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
    switch(mcu_status.get_mech_brake_active()) {
      case 0:
        dashboard_neopixels.setPixelColor(LED_LIST::BRAKE_ENGAGE, LED_OFF);
        dashboard_status.set_mech_brake_led(static_cast<uint8_t>(LED_MODES::OFF));
        break;
      case 1:
        dashboard_neopixels.setPixelColor(LED_LIST::BRAKE_ENGAGE, LED_ON_GREEN);
        dashboard_status.set_mech_brake_led(static_cast<uint8_t>(LED_MODES::ON));
        break;
       
    }

    switch(mcu_status.get_launch_ctrl_active()) {
      case 0:
        dashboard_neopixels.setPixelColor(LED_LIST::LAUNCH_CTRL, LED_OFF);
        dashboard_status.set_mech_brake_led(static_cast<uint8_t>(LED_MODES::OFF));
      case 1:
        dashboard_neopixels.setPixelColor(LED_LIST::LAUNCH_CTRL, LED_ON_GREEN);
        dashboard_status.set_mech_brake_led(static_cast<uint8_t>(LED_MODES::ON));
    }
}

inline void mc_fault_codes_received(){
    bool is_mc_err = false;

    if (mc_fault_codes.get_post_fault_hi() ||
        mc_fault_codes.get_post_fault_lo() ||
        mc_fault_codes.get_run_fault_hi() ||
        mc_fault_codes.get_run_fault_lo())
    {
        is_mc_err = true;
    }
    //MC Error LED

    if (is_mc_err){
        dashboard_neopixels.setPixelColor(LED_LIST::MC_ERR, LED_ON_RED);
        dashboard_status.set_mc_error_led(static_cast<uint8_t>(LED_MODES::ON));
        display_list[2] = 1;
        timer_led_mc_err.reset();   
    // display fault for 1 second and then it clears
    } else if (dashboard_neopixels.getPixelColor(LED_LIST::MC_ERR) != LED_OFF && timer_led_mc_err.check()){
        dashboard_neopixels.setPixelColor(LED_LIST::MC_ERR, LED_OFF);
        dashboard_status.set_mc_error_led(static_cast<uint8_t>(LED_MODES::OFF));
        display_list[2] = 0;
    }

    /*if (is_mc_err){
        led_mc_err.setMode(LED_MODES::ON);
        dashboard_status.set_mc_error_led(static_cast<uint8_t>(LED_MODES::ON));
        timer_led_mc_err.reset();
    }
    else if (led_mc_err.getMode() != LED_MODES::OFF && timer_led_mc_err.check()){
        led_mc_err.setMode(LED_MODES::SLOW);
        dashboard_status.set_mc_error_led(static_cast<uint8_t>(LED_MODES::SLOW));
    }*/
}

inline void inertia_status() {
    if (digitalRead(INERTIA_READ) && !digitalRead(SHUTDOWN_H_READ)) {
      
        dashboard_neopixels.setPixelColor(LED_LIST::INERTIA, LED_ON_GREEN);
        dashboard_status.set_inertia_led(static_cast<uint8_t>(LED_MODES::ON));
        display_list[1] = 1;
    } else {
        dashboard_neopixels.setPixelColor(LED_LIST::INERTIA, LED_OFF);
        dashboard_status.set_inertia_led(static_cast<uint8_t>(LED_MODES::OFF));
        display_list[1] = 0;
    }
}
