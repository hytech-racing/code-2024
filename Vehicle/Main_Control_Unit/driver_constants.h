#pragma once

#include "drivers.h"

#ifdef DRIVER
    #if DRIVER == DEFAULT_DRIVER
      #define BRAKE_ACTIVE 2800                // Threshold for brake pedal active  // brake1 0.05

//        #define MIN_ACCELERATOR_PEDAL_1 1000    // Low accelerator implausibility threshold
//        #define MIN_ACCELERATOR_PEDAL_1 700    // Low accelerator implausibility threshold

        #define MIN_ACCELERATOR_PEDAL_1 2130  // accel1 pos slope. 2160 actual
        
//        #define START_ACCELERATOR_PEDAL_1 1370  // Position to start acceleration //1140 + pressure 1151
//        #define END_ACCELERATOR_PEDAL_1 2108    // Position to max out acceleration //1905
//        #define START_ACCELERATOR_PEDAL_1 850  // Position to start acceleration //1140 + pressure 1151
        
        #define START_ACCELERATOR_PEDAL_1 2275  // 0.1
        
        #define END_ACCELERATOR_PEDAL_1 3195    // Position to max out acceleration //1905
        
//        #define MAX_ACCELERATOR_PEDAL_1 2400    // High accelerator implausibility threshold

        #define MAX_ACCELERATOR_PEDAL_1 3340    // High accelerator implausibility threshold. 3310 actual
        

//        #define MIN_ACCELERATOR_PEDAL_2 500    // Low accelerator implausibility threshold
//        #define MIN_ACCELERATOR_PEDAL_2 1350    // Low accelerator implausibility threshold

        #define MIN_ACCELERATOR_PEDAL_2 1530  // accel2 neg slope. 1500 actual
        
//        #define START_ACCELERATOR_PEDAL_2 680  // Position to start acceleration 560 + pressure 570
//        #define END_ACCELERATOR_PEDAL_2 1050    // Position to max out acceleration 948
//        #define START_ACCELERATOR_PEDAL_2 1700  // Position to start acceleration 560 + pressure 570
        
        #define START_ACCELERATOR_PEDAL_2 1388  // 
        
        #define END_ACCELERATOR_PEDAL_2 492    // Position to max out acceleration 948
        
//        #define MAX_ACCELERATOR_PEDAL_2 1250   // High accelerator implausibility threshold

        #define MAX_ACCELERATOR_PEDAL_2 350   // High accelerator implausibility threshold. actual 380
        

        #define MIN_BRAKE_PEDAL_1 2920    // Low brake implausibility threshold // brake1 neg slope. 2890 actual
        
        #define START_BRAKE_PEDAL_1 2800  // Position to start brake // 2078 + pressure
        
        #define END_BRAKE_PEDAL_1 1237    // Position to max out brake 2718 +-5
        
        #define MAX_BRAKE_PEDAL_1 1125  // actual 1150
         
//brake 2 is negative slope
        #define MIN_BRAKE_PEDAL_2 770    // Low brake implausibility threshold  // brake2 pos slope. 810 actual
        
        #define START_BRAKE_PEDAL_2 900  // Position to start brake //2045 - pressure
        
        #define END_BRAKE_PEDAL_2 2450    // Position to max out brake //1410 +- 5
        
        #define MAX_BRAKE_PEDAL_2 2600  // actual 2540

        #define BRAKE_THRESHOLD_MECH_BRAKE_1 2000 // brake 1, mech brake threshold +-5
        
        #define BRAKE_THRESHOLD_MECH_BRAKE_2 2020 // brake 2, mech brake threshold +-5
        
    #else
        #error "Bad driver definition"
    #endif
#else
    #error "Driver required"
#endif

#define HALF_ACCELERATOR_PEDAL_1 ((START_ACCELERATOR_PEDAL_1 + END_ACCELERATOR_PEDAL_1)/2)
#define HALF_ACCELERATOR_PEDAL_2 ((START_ACCELERATOR_PEDAL_2 + END_ACCELERATOR_PEDAL_2)/2)
