#ifndef __STEERING_RS_422_H__
#define __STEERING_RS_422_H__
#include <Arduino.h>
#include <stdint.h>
#include <stddef.h>


#define DEFAULT_HARDWARE_SERIAL 5

#define MAX_POSITION 8192
#define ZERO_POSITION 8180

class STEERING_RS422 {
    public: 
        STEERING_RS422();
        STEERING_RS422(uint8_t serial);
        void init(unsigned long baudrate);
        int16_t read_steering();
        int16_t read_steering_continous();
        void request_status();
        uint16_t get_encoder_position() const { return encoder_position;};
        void calibrate_steering(uint32_t pos);    
        void command_sequence();
        void save_parameters();
        void continous_setup(uint16_t period, bool auto_start);
        void interpret_error_messages(uint8_t status_byte);
        void continous_start();
        void continous_stop();
        void reset_sensor();
        void set_zero_position(uint16_t new_zero_position);
        //uint8_t self_calibration();
    private:
        static uint16_t buf;
        HardwareSerial* _serial;
        uint16_t encoder_position;
        bool error;
        bool warning;
        uint8_t status;
        /*
        r7 - Warning: limit of lower ride height tolerance. Error: Signal Ampltiude too high. Redhead too close to magentor external magnetic field is interfering.
        r6 - Warning: Limit of upper ride height tolerance. Error: Signal Amplitude too low. Distance bewteen redhead and ring is too large.
        r5 - Warning: Redhead temp out of range
        r4 - Warning: Speed too high
        */
        uint16_t zero_position;
        int16_t steering_position; 
};

#endif