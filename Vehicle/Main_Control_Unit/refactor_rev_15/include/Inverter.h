/**
 * @file Inverter.h
 * @author Eric Galluzzi
 * @brief object for individual inverters to make htX inverter agnostic
 * @version 0.1
 * @date 2023-12-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include "HyTech_CAN.h"

class Inverter {
    private:
    bool isFront;
    bool isLeft;
    public:
    /**
     * @brief Construct a new Inverter object
     * 
     * @param isFront is it a front or rear inverter
     * @param isLeft is it a left or right inverter
     */
    Inverter(bool isFront, bool isLeft) {
        this->isFront = isFront;
        this->isLeft = isLeft;
    }
    /**
     * @brief is inverter a front or rear
     * 
     * @return true 
     * @return false 
     */
    bool is_front() {
        return isFront;
    }; 
    /**
     * @brief is inverter a left or right
     * 
     * @return true 
     * @return false 
     */
    bool is_left() {
        return isLeft;
    }

    MC_status mc_status;
    MC_temps mc_temp;
    MC_energy mc_energy;
    MC_setpoints_command mc_setpoints_command;
};