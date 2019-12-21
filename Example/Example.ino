#include "MotorDriver.h"
#include "ppmReceiver.h"

/*
 * Basic example of a dual motor controller which 
 * uses x2 VNH5019 Dual H Bridge drivers (24V) and
 * a Turnigy 9x 8Ch Receiver/Transmitter. Example is
 * setup for Arduino Mega 2560 running ISRs on the
 * PPM/PWM Receiver inputs.
 * 
*/
//Pins for VNH5019 PWM motor driver
//InA, EnA, InB, EnB, PWM Pin
Motor mtr1(22, 26, 24, 28, 5);
Motor mtr2(23, 27, 25, 29, 6);

//PWM Receiver PINs (ISR capable), MIN uS, MID uS, MAX uS
Receiver ch1(1028,1448,1864);
Receiver ch2(1028,1448,1864);
Receiver ch3(1028,1448,1864);
Receiver ch4(1028,1448,1864);
Receiver ch5(1028,1448,1826);

uint8_t M1_Pwm;
uint8_t M2_Pwm;
long now = 0;
long lastMotorControlTime = 0;
const int motorUpdateInterval = 100;
long lastDebugTime;
const int debugUpdateInterval = 500;

//Pins for Turnigy 9x8C V2 Reciever
//Each pin maps to one channel of the receiver
//The input from the reciever to each pin will be a PWM signal
//The width of the PWM signal is set by the hand held remote control
//Checking this width tells us the different thumbstick and switch positions
const int Ch1_pin = 2;//Default Right Thumbstick L/R
const int Ch2_pin = 3;//Default Right Thumbstick Up/Down
const int Ch3_pin = 18;//Default Left Thumbstick Up/Down
const int Ch4_pin = 19;//Default Left Thumbstick L/R
const int Ch5_pin = 4;//Default mapped to right gear switch

void setup() {
Serial.begin(9600);//This tells our microcontroller what our clock rate is. Allows for communication protocols.
attachInterrupt(digitalPinToInterrupt(Ch1_pin),Channel1DataRising,RISING);
attachInterrupt(digitalPinToInterrupt(Ch2_pin),Channel2DataRising,RISING);
attachInterrupt(digitalPinToInterrupt(Ch3_pin),Channel3DataRising,RISING);
attachInterrupt(digitalPinToInterrupt(Ch4_pin),Channel4DataRising,RISING);
}

void loop() {
  now = millis();
  debug(now);
  updateMotorControl(now);
}

void debug(long tNow){
  if(tNow - lastDebugTime >= debugUpdateInterval){
    lastDebugTime = tNow;
    Serial.println();
    Serial.print("M1 PWM: ");
    Serial.print(ch2.get8bitVal());
    Serial.print("  Fwd: ");
    Serial.print(ch2.forward);
    
    Serial.print("  M2 PWM: ");
    Serial.print( ch3.get8bitVal());
    Serial.print("  Fwd: ");
    Serial.print(ch3.forward);    
  }
}

void updateMotorControl(long _tNow){
  if(now - lastMotorControlTime >= motorUpdateInterval){      
    lastMotorControlTime = now;
    //MTR1 UPDATE   
    M1_Pwm = ch2.get8bitVal();
    M2_Pwm = ch3.get8bitVal();
    
    if(ch1.forward){
      mtr1.setDirection(1);
    }
    else mtr1.setDirection(0);
    mtr1.setPwm(ch2.get8bitVal()); 

    //MTR2 UPDATE
    if(ch3.forward){
      mtr2.setDirection(1);
    }
    else mtr2.setDirection(0); 
    mtr2.setPwm(ch3.get8bitVal()); 
  }
}


////ISR Functions to read the PWM signals
void Channel1DataRising(){
    long Ch1RisingEdgeTime = micros();
    ch1.setRisingEdgeTime(Ch1RisingEdgeTime);
    attachInterrupt(digitalPinToInterrupt(Ch1_pin),Channel1DataFalling,FALLING);      
  }
void Channel1DataFalling(){
    long Ch1FallingEdgeTime = micros();
    ch1.setFallingEdgeTime(Ch1FallingEdgeTime);
    attachInterrupt(digitalPinToInterrupt(Ch1_pin),Channel1DataRising,RISING);      
  }  

void Channel2DataRising(){
    long Ch2RisingEdgeTime = micros();
    ch2.setRisingEdgeTime(Ch2RisingEdgeTime);
    attachInterrupt(digitalPinToInterrupt(Ch2_pin),Channel2DataFalling,FALLING);      
  }
void Channel2DataFalling(){
    long Ch2FallingEdgeTime = micros();
    ch2.setFallingEdgeTime(Ch2FallingEdgeTime);
    attachInterrupt(digitalPinToInterrupt(Ch2_pin),Channel2DataRising,RISING);      
  } 

void Channel3DataRising(){
    long Ch3RisingEdgeTime = micros();
    ch3.setRisingEdgeTime(Ch3RisingEdgeTime);
    attachInterrupt(digitalPinToInterrupt(Ch3_pin),Channel3DataFalling,FALLING);      
  }
void Channel3DataFalling(){
    long Ch3FallingEdgeTime = micros();
    ch3.setFallingEdgeTime(Ch3FallingEdgeTime);
    attachInterrupt(digitalPinToInterrupt(Ch3_pin),Channel3DataRising,RISING);      
  }   
   
void Channel4DataRising(){
    long Ch4RisingEdgeTime = micros();
    ch4.setRisingEdgeTime(Ch4RisingEdgeTime);
    attachInterrupt(digitalPinToInterrupt(Ch4_pin),Channel4DataFalling,FALLING);      
  }
void Channel4DataFalling(){
    long Ch4FallingEdgeTime = micros();
    ch4.setFallingEdgeTime(Ch4FallingEdgeTime);
    attachInterrupt(digitalPinToInterrupt(Ch4_pin),Channel4DataRising,RISING);      
  }  
   
