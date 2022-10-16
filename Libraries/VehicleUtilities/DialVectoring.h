#pragma once
#include "Arduino.h"
#include <Metro.h>


/*
Class for dealing with dial vectoring 

//



*/

enum class DIAL_MODES { MODE_ONE = 0, MODE_TWO = 1, ACCELERATION_LAUNCH_CONTROL = 2, SKIDPAD = 3, AUTOCROSS = 4, ENDURANCE = 5};

class DialVectoring {

    private:
        DIAL_MODES mode;
        bool debouncing;
        bool dialed;
        int *pins;
        Metro metro;

    public:
        DialVectoring() : metro(0) {}

        void begin(int *pins, unsigned long interval, int mode = INPUT_PULLUP) {
            this->pins = pins;
            for(unsigned int i = 0; i < sizeof(pins); i++){
                this->pins[i] = *(pins + i);
                pinMode(this->pins[i], mode);
                dialed = false;
                debouncing = false; 
                metro = Metro(interval);
            }
            readMode();
        }

        DIAL_MODES readMode(){
            DIAL_MODES newState = DIAL_MODES::MODE_ONE; //set to default enum value
            for(unsigned int i =0; i< sizeof(pins); i++){
                
                int reading = digitalRead(pins[i]);
                if(reading == dialed && !debouncing ){
                    debouncing  = true;
                    metro.reset();
                }
                if(debouncing && reading != dialed){
                    debouncing = false;
                }

                if(debouncing && metro.check()){
                    newState = DIAL_MODES(i);
                    break;
                }
            }
            if(mode != newState){
                mode = newState;
                
            }
            return mode;
        }

        
    

    
};