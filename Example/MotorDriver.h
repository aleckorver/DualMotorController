// Arduino VNH5019 Library
// https://github.com/
// Copyright (C) 2018 by Alec Korver and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html

#ifndef MotorDriver_H
#define MotorDriver_H

#include <Arduino.h>

class Motor
{
    public:
        // Initialize a Button object and the pin it's connected to
        Motor(int _pinInA, int _pinInB, int _pinEnA, int _pinEnB, int _pinPwm);
        void setDirection(bool dir);
        bool getDirection();
        void setPwm(uint8_t pwm);
        uint8_t getPwm();
        int _direction = 1;
        uint8_t pwm = 0;
        
    private:
        const int forward = 1;
        const int reverse = 0;
        uint8_t pinInA = 0;
        uint8_t pinInB = 0;
        uint8_t pinEnA = 0;
        uint8_t pinEnB = 0;
        uint8_t pinPwm = 0;
};
#endif
