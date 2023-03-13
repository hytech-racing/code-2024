#pragma once

#include "drivers.h"

#ifdef DRIVER
    #if DRIVER == DEFAULT_DRIVER
      #define BRAKE_ACTIVE 1290                // Threshold for brake pedal active  

        #define MIN_ACCELERATOR_PEDAL_1 1000    // Low accelerator implausibility threshold
        #define START_ACCELERATOR_PEDAL_1 1140  // Position to start acceleration //1140 + pressure 1151
        #define END_ACCELERATOR_PEDAL_1 1905    // Position to max out acceleration //1905
        #define MAX_ACCELERATOR_PEDAL_1 2150    // High accelerator implausibility threshold

        #define MIN_ACCELERATOR_PEDAL_2 490    // Low accelerator implausibility threshold
        #define START_ACCELERATOR_PEDAL_2 560  // Position to start acceleration 560 + pressure 570
        #define END_ACCELERATOR_PEDAL_2 948    // Position to max out acceleration 948
        #define MAX_ACCELERATOR_PEDAL_2 1020   // High accelerator implausibility threshold

        #define MIN_BRAKE_PEDAL_1 1876    // Low brale implausibility threshold
        #define START_BRAKE_PEDAL_1 2075  // Position to start brake // 2078 + pressure
        #define END_BRAKE_PEDAL_1 2718    // Position to max out brake 2718 +-5
        #define MAX_BRAKE_PEDAL_1 2918
         

        #define MIN_BRAKE_PEDAL_2 1845    // Low brake implausibility threshold
        #define START_BRAKE_PEDAL_2 2045  // Position to start brake //2045 - pressure 
        #define END_BRAKE_PEDAL_2 1410    // Position to max out brake //1410 +- 5
        #define MAX_BRAKE_PEDAL_2 1610

        #define BRAKE_THRESHOLD_MECH_BRAKE_1 2357 // brake 1, mech brake threshold +-5
        #define BRAKE_THRESHOLD_MECH_BRAKE_2 1770 // brake 2, mech brake threshold +-5
    #else
        #error "Bad driver definition"
    #endif
#else
    #error "Driver required"
#endif

#define HALF_ACCELERATOR_PEDAL_1 ((START_ACCELERATOR_PEDAL_1 + END_ACCELERATOR_PEDAL_1)/2)
#define HALF_ACCELERATOR_PEDAL_2 ((START_ACCELERATOR_PEDAL_2 + END_ACCELERATOR_PEDAL_2)/2)
