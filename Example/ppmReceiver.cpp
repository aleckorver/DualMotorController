// Arduino VNH5019 Motor Driver Library
// https://github.com/aleckorver/DualMotorController
// Copyright (C) 2019 by Alec Korver and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html

#include "ppmReceiver.h"
  
Receiver::Receiver(int _min_uS, int _mid_uS, int _max_uS){
  min_uS = _min_uS;
  mid_uS = _mid_uS;
  max_uS = _max_uS;    
}

void Receiver::setRisingEdgeTime(long risingTime){
  risingEdgeTime = risingTime;
}

void Receiver::setFallingEdgeTime(long fallingTime){
   fallingEdgeTime = fallingTime;
   calcTimeDiff();
}

void Receiver::calcTimeDiff(){
  timeDif = risingEdgeTime - fallingEdgeTime;
  timeDif = abs(timeDif);
  if(timeDif >= mid_uS && timeDif <= max_uS){
    val = map(timeDif, mid_uS, max_uS, 0, 255);
    forward = true;
  }
  else if(timeDif <= mid_uS && timeDif >= min_uS){
    val = map(timeDif, mid_uS, min_uS, 0, 255);
    forward = false;
  }
}

uint8_t Receiver::get8bitVal(){
  uint8_t _val = val;
  if(_val <=20){
    _val = 0;
  }
  else if(_val >= 240){
    _val = 255;
  }
  return _val;
}
