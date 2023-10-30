#include "DebouncedButton.h"
#include "Dashboard_status.h"

// Button pin definition
#define BTN_SAFE_CTRL PB5  // gpio 1
#define BTN_START PB6 // gpio 2
#define BTN_MC_CYCLE PB7 // gpio 3
#define BTN_TORQUE_MODE PC13 // gpio 4
#define BTN_LED_DIMMER PC14 // gppio 5

class DashboardCAN;

class Controls {
    public:
        void startup();
        void update(DashboardCAN* CAN);
    private:
        DebouncedButton btn_safe_ctrl;
        DebouncedButton btn_mc_cycle;
        DebouncedButton btn_start;
        DebouncedButton btn_torque_mode;
        DebouncedButton btn_led_dimmer;
};