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
}
uint16_t STEERING_RS422::read_steering() {
    uint8_t readByte[3] = {0};
    _serial->write(0x64);


    //check if _serial is available and the first byte is ascii "d"


    if (_serial->available() >= 4 && _serial->read() == 0x64) {

        readByte[0] = _serial->read(); //lower half of steering
        readByte[1] = _serial->read(); //upper half of steering
        readByte[2] = _serial->read(); //error stream 
        encoder_position = readByte[1] << 6 | readByte[0] >> 2;
        error = readByte[0] & 2; //if high, current data is not valid, previous data is sent
        warning = readByte[0] & 1; //if high, some operating conditions are close to limits
        status = readByte[2];
        if ((MAX_POSITION + (encoder_position - zero_position)) % MAX_POSITION <= (MAX_POSITION / 2)) {
            steering_position = -((MAX_POSITION + (encoder_position - zero_position)) % MAX_POSITION);
        } else {
            steering_position = MAX_POSITION - ((MAX_POSITION + (encoder_position - zero_position)) % MAX_POSITION);
        }
    }

    

        
    return steering_position;    

}