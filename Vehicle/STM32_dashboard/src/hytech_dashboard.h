#ifndef HYTECH_DASH_H
#define HYTECH_DASH_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>
#include <Adafruit_NeoPixel.h>
#include <DashboardCAN.h>
#include <bitmaps.h>

// Display defines
#define SHARP_SCK  PA5
#define SHARP_MOSI PA7
#define SHARP_SS   PC4

#define BLACK 0
#define WHITE 1

// Neopixel defines
#define NEOPIXEL_PIN PA2
#define NEOPIXEL_COUNT 13

// OFF: OFF, ON: GREEN/OK, YELLOW : WARNING/MISC RED : CRITICAL
enum class LED_MODES { OFF = 0, ON = 1, YELLOW = 2, RED = 3};
enum LED_LIST { AMS = 0, IMD = 1, MC_ERR = 2, GEN_PURP = 3, INERTIA = 4, BOTS = 5, 
                COCKPIT_BRB = 6, CRIT_CHARGE = 7, GLV = 8, BRAKE_ENGAGE = 9, LAUNCH_CTRL = 10, 
                TORQUE_MODE = 11, RDY_DRIVE = 12};

#define LED_OFF 0x00
#define LED_ON_GREEN 0xFF00
#define LED_YELLOW 0xFFFF00
#define LED_RED 0xFF0000
#define LED_INIT 0xFF007F
#define LED_BLUE 0xFF
#define LED_WHITE 0xFFFFFFFF

class Singleton {
    public:
        // Function to get the instance of the Singleton
        static Singleton& getInstance() {
            static Singleton instance; // The instance is created only once
            return instance;
        }
    
        // Other member functions and data members here
    private:
        // Private constructor to prevent external instantiation
        Singleton() { }
    
        // Private destructor to prevent external deletion
        ~Singleton() { }
};
 
int main() {
    Singleton& mySingleton = Singleton::getInstance(); // Access the Singleton instance
    // Use mySingleton as needed
 
    // You cannot create another instance of the Singleton like this:
    // Singleton anotherInstance; // This will not work
    return 0;
}

namespace hytech_dashboard {
    extern Adafruit_NeoPixel neopixels;
    extern Adafruit_SharpMem display;
    void startup();
    void refresh(int* i);
    void set_neopixel(uint16_t id, uint32_t c);
}

#endif



