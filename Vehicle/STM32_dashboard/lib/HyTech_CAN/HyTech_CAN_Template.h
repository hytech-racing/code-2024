// Makes sure the compiler only includes this header once
// Prevents multiple inlcusions
#pragma once
// C++ libraries for use of memcpy function, and (u)intX_t datatypes
#include <string.h>
#include <stdint.h>
#ifdef HT_DEBUG_EN
    // Arduino.h should inlcude the above, so might want to use it
    // in debugging cases
    #include "Arduino.h"
#endif

// Make sure that class members are packed tightly without padding/slack
#pragma pack(push,1)

// Note that CAN message IDs are set in CAN_ID.h

// class that defines the data and methods of the CAN packet
// Allows the data to be somewhat portable, can pass pointer/reference to the
// class to access the data in other parts of the program.
class Template{
    // public methods for data manipulation
public:

    // default constructor 
  Template() = default;

    // constructor that initializes the class with a CAN packet
  Template(uint8_t buf[]) { load(buf); }

    // load() takes in a buffer, and copies the buffer to the class's member
    // data. (This is usually used with the FlexCAN or STM32CAN CAN_message_t.buf)
    // This effectively loads the CAN message.
  inline void load(uint8_t buf[])         { memcpy(this, buf, sizeof(*this)); }
    
    // write() takes in a buffer, and copies the member data of the class to
    // the location of the buffer. This effectively writes the CAN message.
  inline void write(uint8_t buf[])  const { memcpy(buf, this, sizeof(*this)); }

    // The getter functions allow another part of the program to access
    // the private member variables. The getter functions can implement
    // scaling by a factor and offset to fit a range of data into smaller
    // variables by sacrificing precision.
    //
    // If multiple pieces of data are stored in a single variable (such as
    // IDs of BMS segments), shift and logical operations can be performed
    // to properly separate the data
    inline uint8_t get_eightBit_1()      const { return eightBit_1; }
    inline int8_t get_eightBit_2()      const { return eightBit_2; }
    inline uint16_t get_sixteenBit()      const { return sixteenBit; }

    // example of shifting and factor/offset from BMS_balancing_status.h
    inline uint16_t get_ic_balancing(uint8_t ic_id)     const { return (thirtyTwoBit >> (0x4 + 0x9 * ic_id)) & 0x1FF; }

    // Setter functions allow for other parts of the program to write data
    // to the member variables so that it can be sent. Not every CAN message
    // will have setter functions, if it is reading from a CAN device such
    // as the energy meter.
    inline void set_eightBit_1(uint8_t eightBit_1)     {this->eightBit_1 = eightBit_1;}
    inline void set_eightBit_2(uint8_t eightBit_2)     {this->eightBit_2 = eightBit_2;}
    inline void set_sixteenBit(uint16_t sixteenBit)     {this->sixteenBit = sixteenBit;}
    // example setter from BMS_balancing.h
    inline void set_ic_balancing(uint8_t ic_id, uint16_t balancing_status) {
        int shift = 4 + 9 * ic_id;
        thirtyTwoBit &= ~(((uint64_t) 0x1FF) << shift);
        thirtyTwoBit |= ((uint64_t) balancing_status & 0x1FF) << shift;
    }

    // private variables that hold CAN buffer data
private:
    // the member variables must fit in a CAN message's 8-byte buffer
    // some CAN messages use a single 64 bit variable and pack/unpack
    // the data manually.

    //sizeof() will return the size of these variables

    uint8_t eightBit_1;
    int8_t eightBit_2;
    uint16_t sixteenBit;
    uint32_t thirtyTwoBit;
  
};

#pragma pack(pop)
