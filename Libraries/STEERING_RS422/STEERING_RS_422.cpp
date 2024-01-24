#include "STEERING_RS422.h"

// Init STEERING_RS422 using default hardware serial port
STEERING_RS422::STEERING_RS422() {
    STEERING_RS422(DEFAULT_HARDWARE_SERIAL);
}

//Init STEERING_RS422 using configurable hardware serial port
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
    this->set_zero_position(zero_position);
}

int16_t STEERING_RS422::read_steering() {
    
    request_status();

    //check if _serial is available and the first byte is ascii "d"
        
    return read_steering_continuous();    

}

void STEERING_RS422::request_status() {
    _serial->write(0x64);
    delay(1);
}

int16_t STEERING_RS422::read_steering_continuous() {
    uint8_t readByte[3] = {0};
    //check if _serial is available and the first byte is ascii "d"
    if (_serial->available() >= 4 && _serial->read() == 0x64) {
        //encoder position left aligned, MSB first
        readByte[0] = _serial->read(); //encoder position b15-b8 of steering
        readByte[1] = _serial->read(); //encoder position b7-b0 of steering 
        readByte[2] = _serial->read(); //detailed status b7-b0
        encoder_position = (int16_t)(((uint16_t)readByte[0]) << 8) | ((uint16_t)readByte[1]);
        encoder_position = encoder_position >> 2; //b1-b0 general status
        error = readByte[1] & 2; //if high, current data is not valid, previous data is sent
        warning = readByte[1] & 1; //if high, some operating conditions are close to limits
        status = readByte[2];
        Serial.println(encoder_position);
    
        interpret_error_messages(status);
        //_serial->clear();
    }
    return encoder_position;   
}

void STEERING_RS422::interpret_error_messages(uint8_t status_byte) {
    if (status_byte & 0x80) {
            Serial.println("Warning: Limit of lower ride height tolerance. Error: Signal amplitude too high.");
        }
    if (status_byte & 0x40) {
        Serial.println("Warning: Limit of upper ride height tolerance. Error: Signal amplitude low.");
    }
    if (status_byte & 0x20) {
        Serial.println("Warning: The readhead temperature is out of specified range.");
    }
    if (status_byte & 0x10) {
        Serial.println("Warning: Speed too high.");
    }
    if (status_byte & 0x08) {
        Serial.println("Error: Multiturn counter error.");
    }
}

void STEERING_RS422::set_zero_position(uint16_t new_zero_position) {
    zero_position = new_zero_position;
}

void STEERING_RS422::calculate_zero_position() {
    request_status();
    delay(5);
    uint8_t readByte[3] = {0};
    //check if _serial is available and the first byte is ascii "d"
    if (_serial->available() >= 4 && _serial->read() == 0x64) {
        //encoder position left aligned, MSB first
        readByte[0] = _serial->read(); //encoder position b15-b8 of steering
        readByte[1] = _serial->read(); //encoder position b7-b0 of steering 
        readByte[2] = _serial->read(); //detailed status b7-b0
        set_zero_position(((readByte[0] << 8) | readByte[1]) >> 2);
        Serial.println(((readByte[0] << 8) | readByte[1]) >> 2);
    }
}

uint16_t STEERING_RS422::return_zero_position() {
    return zero_position;
}

void STEERING_RS422::calibrate_steering(uint32_t pos) {
    //start programming sequence
    //this sends data to essentially add a zero offset
    Serial.println(pos);
    command_sequence();
    //0x5A is programming command byte for position offset setting
    _serial->write(0x5A);
    delay(1);
    _serial->write((pos & 0xFF000000) >> 24);
    //_serial->write(0);
    delay(1);
    _serial->write((pos & 0xFF0000) >> 16);
    //_serial->write(0);
    delay(1);
    _serial->write((pos & 0xFF00) >> 8);
    //_serial->write(0x2D);
    delay(1);
    _serial->write(pos & 0xFF);
    //_serial->write(0x12);
    delay(1);
}

//Unlock sequence for self-calibration, must be followed by programming command byte
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

void STEERING_RS422::continuous_setup(uint16_t period, bool auto_start = 0) {
    command_sequence();
    //0x54 is programming command byte for continuous response setting
    _serial->write(0x54);
    delay(1);
    //1 to enable automatic start after power on of encoder, 0 to disable
    _serial->write((auto_start) ? 0x01 : 0x00);
    delay(1);
    _serial->write(0x64);
    delay(1);
    //in microseconds
    _serial->write(period & 0xFF00 >> 8);
    delay(1);
    _serial->write(period & 0xFF);

}
void STEERING_RS422::continuous_start() {
    command_sequence();
    //0x53 programming command byte for continuous response start
    _serial->write(0x53);
    delay(1);
}
void STEERING_RS422::continuous_stop() {
    command_sequence();
    //0x50 programming command byte for continuous response stop
    _serial->write(0x50);
    delay(1);
}
void STEERING_RS422::save_parameters() {
    command_sequence();
    //0x63 programming command byte for configuration parameters save
    _serial->write(0x63);
    delay(1);
}
void STEERING_RS422::reset_sensor() {
    command_sequence();
    //0x72 programming command byte for configuration parameters reset
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