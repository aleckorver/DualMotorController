// Arduino VNH5019 Motor Driver Library
// https://github.com/aleckorver/DualMotorController
// Copyright (C) 2019 by Alec Korver and licensed under
// GNU GPL v3.0, https://www.gnu.org/licenses/gpl.html

#include "MotorDriver.h"

Motor::Motor(int _pinInA, int _pinInB, int _pinEnA, int _pinEnB, int _pinPwm){
  pinMode(_pinInA,OUTPUT);
  pinMode(_pinEnA,OUTPUT);
  pinMode(_pinInB,OUTPUT);
  pinMode(_pinEnB,OUTPUT);
  pinMode(_pinPwm,OUTPUT); 

  pinInA = _pinInA;
  pinInB = _pinInB;
  pinEnA = _pinEnA;
  pinEnB = _pinEnB;
  pinPwm = _pinPwm;

  digitalWrite(pinEnA,HIGH);
  digitalWrite(pinEnB,HIGH);

  //Sets motor direction to FORWARD
  digitalWrite(pinInA,HIGH);
  digitalWrite(pinInB,LOW);
}

//TRUE = "FORWARD"
void Motor::setDirection(bool dir){
  if(dir == forward){
    _direction = forward;
    digitalWrite(pinInA,HIGH);
    digitalWrite(pinInB,LOW);
  }
  else{
    _direction = reverse;
    digitalWrite(pinInA,LOW);
    digitalWrite(pinInB,HIGH);   
  }
}

bool Motor::getDirection(){
  return _direction;
}

void Motor::setPwm(uint8_t _pwm){
  pwm = _pwm;
  analogWrite(pinPwm,pwm);
}

uint8_t Motor::getPwm(){
  return pwm;
}
