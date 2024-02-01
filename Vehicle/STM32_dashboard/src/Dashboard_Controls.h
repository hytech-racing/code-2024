#include "DebouncedButton.h"

// Button pin definition
#define BTN_START PC12
#define BTN_MARK PA8
#define BTN_MC_CYCLE PC8
// #define BTN_LAUNCH_CTRL 
#define BTN_TORQUE_MODE PD2
#define BTN_LED_DIMMER PC9

#define BTN_LEFT_SHIFTER PA10
#define BTN_RIGHT_SHIFTER PA9

#define BUZZER_PIN PC13

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
    private:
        bool buzzer_active = false;
        bool previousState = false;
        
};