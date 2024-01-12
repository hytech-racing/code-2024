#include "STEERING_RS422.h"

// Init STEERING_RS422 using default hardware serial port
STEERING_RS422::STEERING_RS422() {
    STEERING_RS422(DEFAULT_HARDWARE_SERIAL);
}

//Init STEERING_RS422 using configurable hadware serial port
STEERING_RS422::STEERING_RS422(uint8_t serial) {
    
    switch (serial) {
        case 1:
            _serial = &Serial1;
            break;
        case 2:
            _serial = &Serial2;
            break;
        case 3:
            _serial = &Serial3;
            break;
        case 4:
            _serial = &Serial4;
            break;
        case 5:
            _serial = &Serial5;
            break;
        case 6:
            _serial = &Serial6;
            break;
        case 7:
            _serial = &Serial7;
            break;
        case 8:

            //_serial = &Serial8; //not on teensy 4.0
            break;
    }
}

void STEERING_RS422::init(unsigned long baudrate) {
    _serial->begin(baudrate);
    this->set_zero_position(ZERO_POSITION);
}
int16_t STEERING_RS422::read_steering() {
    
    request_status();


    //check if _serial is available and the first byte is ascii "d"

        
    return read_steering_continous();    

}
void STEERING_RS422::request_status() {
    _serial->write(0x64);
    delay(1);
}
int16_t STEERING_RS422::read_steering_continous() {
    uint8_t readByte[3] = {0};

    //check if _serial is available and the first byte is ascii "d"
    if (_serial->available() >= 4 && _serial->read() == 0x64) {

        readByte[0] = _serial->read(); //lower half of steering
        readByte[1] = _serial->read(); //upper half of steering
        readByte[2] = _serial->read(); //error stream 
        encoder_position = readByte[0] << 6 | readByte[1] >> 2;
        error = readByte[1] & 2; //if high, current data is not valid, previous data is sent
        warning = readByte[1] & 1; //if high, some operating conditions are close to limits
        status = readByte[2];
        Serial.println(encoder_position);
        
        steering_position = encoder_position - zero_position ;
        //_serial->clear();
    }

    

        
    return steering_position;   
}
void STEERING_RS422::set_zero_position(uint16_t new_zero_position) {
    zero_position = new_zero_position;
}
void STEERING_RS422::calibrate_steering(uint32_t pos) {
    //start programming sequence
    //this sends data to essentially add a zero offset
    //pos = 0x00000E18
    command_sequence();
    _serial->write(0x5A);
    delay(1);
    _serial->write(pos &0xFF000000);
    delay(1);
    _serial->write(pos &0xFF0000);
    delay(1);
    _serial->write(pos &0xFF00);
    delay(1);
    _serial->write(pos & 0xFF);
    delay(1);
}
void STEERING_RS422::command_sequence() {
    delay(1);
    _serial->write(0xCD);
    delay(1);
    _serial->write(0xEF);
    delay(1);
    _serial->write(0x89);
    delay(1);
    _serial->write(0xAB);
    delay(1);
    
}

void STEERING_RS422::continous_setup(uint16_t period, bool auto_start = 0) {
    command_sequence();
    _serial->write(0x54);
    delay(1);
    _serial->write((auto_start) ? 0x01 : 0x00);
    delay(1);
    _serial->write(0x64);
    delay(1);
    //in microseconds
    _serial->write(period & 0xFF00);
    delay(1);
    _serial->write(period & 0xFF);

}
void STEERING_RS422::continous_start() {
    command_sequence();
    _serial->write(0x53);
    delay(1);
}
void STEERING_RS422::continous_stop() {
    command_sequence();
    _serial->write(0x50);
    delay(1);
}
void STEERING_RS422::save_parameters() {
    command_sequence();
    _serial->write(0x63);
    delay(1);
}
void STEERING_RS422::reset_sensor() {
    command_sequence();
    _serial->write(0x72);
    delay(1);
}

/*
uint8_t STEERING_RS422::self_calibration() {
    //b6 calibration alr performed
    //b3 calcualted params out of range
    //b2 timeout (no complete rotation)
    //b1 counter bit 1
    //b0 counter bit 0
    //check status
    uint8_t status_flags = 0;
    //check if calubration was already performed
    command_sequence();
    _serial->write(0x69);
    delay(1);
    if (_serial->available() && _serial->read() == 0x69) {

        status_flags = _serial->read();
    }
    if (status_flags & 0b01000000) {
        return status_flags;
    }

    //self calibrate
    command_sequence();
    _serial->write(0x41);
    delay(10000); //10 sec delay()
    
    //check status
    command_sequence();
    _serial->write(0x69);
    delay(1);
    if (_serial->available() && _serial->read() == 0x69) {
        status_flags = _serial->read();
    }
    //check if any errors
    if (status_flags & 0b0001100) {
        return status_flags;
    }
    return 1;
} */