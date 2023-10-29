#include "DebouncedButton.h"
#include "Dashboard_status.h"

// Button pin definition
#define BTN_SAFE_CTRL 20
#define BTN_START 19
#define BTN_MC_CYCLE 18
#define BTN_TORQUE_MODE 22
#define BTN_LED_DIMMER 21

class DashboardCAN;

class Controls {
    public:
        void startup();
        void update(DashboardCAN* CAN);
        
        DebouncedButton btn_safe_ctrl;
        DebouncedButton btn_mc_cycle;
        DebouncedButton btn_start;
        DebouncedButton btn_torque_mode;
        DebouncedButton btn_led_dimmer;
    private:
};