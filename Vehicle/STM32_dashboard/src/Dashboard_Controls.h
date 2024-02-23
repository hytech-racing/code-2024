#include "DebouncedButton.h"
#include "Metro.h"

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
// Dial pin definition
#define DIAL_SIZE 6
#define DIAL_MODE_ONE PB13
#define DIAL_MODE_TWO PB14
#define DIAL_LAUNCH_CONTROL PB15
#define DIAL_SKIDPAD PC6
#define DIAL_AUTOCROSS PC7
#define DIAL_ENDURANCE PB12

class DashboardCAN;

enum DIAL_MODES { MODE_ONE = 0, MODE_TWO = 1, ACCELERATION_LAUNCH_CONTROL = 2, SKIDPAD = 3, AUTOCROSS = 4, ENDURANCE = 5};

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
        DIAL_MODES dial_mode = MODE_ONE;
        int dial_pins[DIAL_SIZE] = {DIAL_MODE_ONE, DIAL_MODE_TWO, DIAL_LAUNCH_CONTROL, DIAL_AUTOCROSS, DIAL_ENDURANCE};

        bool previousState = false;
        Metro dimmer = Metro(200);
        
};