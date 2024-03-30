#include "DebouncedButton.h"
#include "Metro.h"
#include "Arduino.h"

// Button pin definition
#define BTN_START PC9
#define BTN_MARK PD2
#define BTN_MC_CYCLE PC8
// #define BTN_LAUNCH_CTRL 
#define BTN_TORQUE_MODE PA8
#define BTN_LED_DIMMER PC12
#define BTN_LEFT_SHIFTER PA10
#define BTN_RIGHT_SHIFTER PA9
#define BUZZER_PIN PC13
// Dial pin definition
#define DIAL_SIZE 6
#define DIAL_MODE_ZERO PB12
#define DIAL_MODE_ONE PB13
#define DIAL_MODE_TWO PB14
#define DIAL_MODE_THREE PB15
#define DIAL_MODE_FIVE PC7
#define DIAL_MODE_FOUR PC6
// Shutdown pin definition
#define SHUTDOWN_B_BOTS_PIN PB6 // not used atm, pulls status from ECU
#define SHUTDOWN_J_INERTIA_PIN PB7
#define SHUTDOWN_K_COKPIT_BRB_PIN PB5

#define BUZZER_TIME 2000

class DashboardCAN;
class Dashboard_Controls {
    public:
        /*
            Consider rewriting this in more of an "interface" style where it holds its own data.
            The CAN class will interact with the Dashboard_Controls reference to pack data into the CAN message.
            We will keep this for now for the sake of efficiency (only writing data once)
        */
        void startup();
        void update(DashboardCAN* CAN);
        DebouncedButton btn_start;
        DebouncedButton btn_mark;
        DebouncedButton btn_mc_cycle;
        // DebouncedButton btn_launch_ctrl;
        DebouncedButton btn_torque_mode;
        DebouncedButton btn_led_dimmer;
        DebouncedButton btn_left_shifter;
        DebouncedButton btn_right_shifter;

        int buttons[5] = {BTN_START, BTN_MARK, BTN_MC_CYCLE, BTN_TORQUE_MODE, BTN_LED_DIMMER};
    private:
        // walker trolled again
        int dial_pins[DIAL_SIZE] = {DIAL_MODE_ZERO, DIAL_MODE_ONE, DIAL_MODE_TWO, DIAL_MODE_THREE,DIAL_MODE_FOUR, DIAL_MODE_FIVE};
        
        /**
         * replace values in this array with the actual modesthat we will use
        */
        String DIAL_MODES[DIAL_SIZE] = {"DIAL_MODE_ZERO", "DIAL_MODE_ONE", "DIAL_MODE_TWO", "DIAL_MODE_THREE", "DIAL_MODE_FOUR", "DIAL_FIVE"};


        bool previousState = false;
        unsigned long buzzer_last_millis = 0;
        bool buzzer_cur_state = false;
        Metro buzzer_timer = Metro(BUZZER_TIME);
        Metro dimmer_timer = Metro(200);
        Metro page_switch_timer = Metro(200);
        
};