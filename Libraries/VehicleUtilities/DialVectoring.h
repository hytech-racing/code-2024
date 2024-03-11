#pragma once
#include "Arduino.h"
#include <Metro.h>


/*
Class for dealing with dial vectoring 

//



*/

enum DIAL_MODES { MODE_ONE = 0, MODE_TWO = 1, ACCELERATION_LAUNCH_CONTROL = 2, SKIDPAD = 3, AUTOCROSS = 4, ENDURANCE = 5};

class DialVectoring {

    private:
        DIAL_MODES mode = MODE_ONE;
        unsigned int size;
        int pins[6];
        Metro metro;

    public:
        DialVectoring() : metro(0) {}
        void begin(int *pins, unsigned int size, unsigned long interval, int mode = INPUT_PULLUP) {
            
            this->size = size;
            for(unsigned int i = 0; i < size; i++){
                
                this->pins[i] = pins[i];
                pinMode(this->pins[i], mode);
                metro = Metro(interval);
            }
            // readMode();
        }

        DIAL_MODES readMode(){
            DIAL_MODES newState = mode; //set to default enum value
            for(unsigned int i =0; i< size; i++){
                
                bool reading = digitalRead(pins[i]);
              
                if(!reading) {
                    newState = DIAL_MODES(i);
                    break;
                }
            }
            mode = newState;

            return mode;
            }
            
    

        
    

    
};