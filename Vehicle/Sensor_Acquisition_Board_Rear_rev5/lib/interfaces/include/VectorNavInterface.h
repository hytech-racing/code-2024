#ifndef __VECTORNAV_INERFACE__
#define __VECTORNAV_INERFACE__

#include <Arduino.h>

#include <stdint.h>

#include "VNConsts.h"

class VectorNavInterface
{
private:
    // Data
    HardwareSerial *serial_;
    int serialSpeed_;



public:
    // Constructors
    VectorNavInterface(HardwareSerial *serial, int serialSpeed):
        serial_(serial),
        serialSpeed_(serialSpeed) {};
    VectorNavInterface(HardwareSerial *serial):
        VectorNavInterface(serial, VN_SERIAL_BAUDRATE5) {};

    



};

#endif  /* __VECTORNAV_INERFACE__ */