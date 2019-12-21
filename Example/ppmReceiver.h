// Arduino VNH5019 Library
// https://github.com/aleckorver/DualMotorController
// Copyright (C) 2018 by Alec Korver and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html

#ifndef PPMRECEIVER_H
#define PPMRECEIVER_H

#include <Arduino.h>

class Receiver
{
    public:
      Receiver(int _min_uS, int _mid_uS, int _max_uS);
      void setRisingEdgeTime(volatile long risingTime);
      void setFallingEdgeTime(volatile long fallingTime);
      uint8_t get8bitVal();
      bool forward = true;
 
    private:
      volatile long risingEdgeTime, fallingEdgeTime, timeDif, pwm;
      int min_uS = 1028;
      int mid_uS = 1448;
      int max_uS = 1864;
      const float M_Ratio = 0.607;
      void calcTimeDiff();
      uint8_t val = 0;    
};
#endif
