/*
Author: Alec Korver
Date: 4/10/2017

Description:
Principia College ASME Tank Competition Arduino Mega 2560 Shield
The following code is for controlling (2) track motors and (1 or 2) tennis
ball launcher motors. 

Each shield uses
(1) Custom PCB Shield
(2) Pololu VNH5019 motor drivers (5V - 24V) 12A Cont. 30A Peak 20kHz max PWM
(1) Turnigy 9x8C V2 Receiver - 8 Channel 
(1) Pololu 5V S18V20F5 Voltage Regulator

To control the motors you will need (1) Turnigy 9x 8 Channel handheld 
remote control. The controll will need to be synced with the reciever prior 
to use. Only 3 channels are initially used for this system. There are 5
additional channels, which can be used for future expansions.

Handheld remote control info
https://hobbyking.com/media/file/725056143X2037269X20.pdf

//Good information on reading the PWM signals from the reciever
http://www.impulseadventure.com/elec/attiny-spi-rc-pwm.html

IMPORTANT NOTE ABOUT LAUNCHER
The launcher motor should be up to full speed before a tennis ball is dropped in. 
This ensures that it will work similar to a pitching machine. Otherwise the launcher will be
underpowered. To increase the firing distance of the launcher there are a few options:
1)Increase the wheel inertia (increase mass of wheel)
2)Increase diameter of the launcher wheel (Larger diameter means higher speed at edge)
3)Both 1 and 2
4)Increase the voltage to the motor (Do not run continuously at higher voltage because the motor WILL burn up!!)
*/

//CODE FOR CUSTOM CONTRLOL

#include <avr/wdt.h> //WATCHDOG TIMER
//https://tushev.org/articles/arduino/5/arduino-and-watchdog-timer

//Pins for Pololu PWM motor driver 1
//The first 4 pins need to be set either high or low
//How they are set determines whether the motor will be 
//rotating CW or CCW or if it is braking or regenerating.
//See datasheet on motor driver for more details.
const int M1InA_pin = 22;
const int M1EnA_pin = 24;
const int M1InB_pin = 26;
const int M1EnB_pin = 28;
const int M1Pwm_pin = 5;

//Pins for Pololu PWM motor driver 2
//The first 4 pins need to be set either high or low (1 or 0)
//How they are set determines whether the motor will be 
//rotating CW or CCW or if it is braking or regenerating.
//See datasheet on motor driver for more details.
const int M2InA_pin = 23;
const int M2EnA_pin = 25;
const int M2InB_pin = 27;
const int M2EnB_pin = 29;
const int M2Pwm_pin = 6;

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

//Pins for launcher output - aka they trip the relay
const int relay1 = 8;
const int relay2 = 9;

//Variables to store the PWM values read from each receiver channel(pin) - (micro seconds)
volatile long Ch1Pwm;
volatile long Ch2Pwm;
volatile long Ch3Pwm;
volatile long Ch4Pwm;
volatile long Ch5Pwm;

//Maximum pulse width (micro seconds) - TEST AND SET THESE VALUES!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
long Ch1MaxPw = 1864;
long Ch2MaxPw = 1864;
long Ch3MaxPw = 1864;
long Ch4MaxPw = 1864;
long Ch5MaxPw = 1826;

//Middle value for pulse width (micro seconds) - This is the center position for the thumbsticks - TEST AND SET THESE VALUES!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
int Ch1MidPw = 1448;
int Ch2MidPw = 1448;
int Ch3MidPw = 1448;
int Ch4MidPw = 1448;
int Ch5MidPw = 1448;

//Min value for pulse width (micro seconds) - This is the center position for the thumbsticks - TEST AND SET THESE VALUES!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
int Ch1MinPw = 1028;
int Ch2MinPw = 1028;
int Ch3MinPw = 1028;
int Ch4MinPw = 1028;
int Ch5MinPw = 1028;

const float M_Ratio = 0.607;

//Variables for holding the PWM value (0 - 255) that we will send to the
//motor driver. This will set our left and right motor speeds
//255 is 100% output aka FULL SPEEED (FULL 24V)
long M1_Pwm;
long M2_Pwm;

//These are global variables which are used to transfer data from the
//ISR to the main code. These must be declared as VOLATILE.
volatile long Ch1RisingEdgeTime, Ch1FallingEdgeTime, Ch1TimeDif;
volatile long Ch2RisingEdgeTime, Ch2FallingEdgeTime, Ch2TimeDif;
volatile long Ch3RisingEdgeTime, Ch3FallingEdgeTime, Ch3TimeDif;
volatile long Ch4RisingEdgeTime, Ch4FallingEdgeTime, Ch4TimeDif;

//SETUP CODE - RUNS ONE TIME AT STARTUP TO INITIALIZE MICRO SETTINGS
void setup() {
Serial.begin(9600);//This tells our microcontroller what our clock rate is. Allows for communication protocols.

//Enables and disables relay 2
attachInterrupt(digitalPinToInterrupt(Ch1_pin),Channel1DataRising,RISING);
attachInterrupt(digitalPinToInterrupt(Ch1_pin),Channel1DataFalling,FALLING);

//Checks Motor 1 for rising and falling edge to determine PWM length
attachInterrupt(digitalPinToInterrupt(Ch2_pin),Channel2DataRising,RISING);
attachInterrupt(digitalPinToInterrupt(Ch2_pin),Channel2DataFalling,FALLING);

//Checks Motor 2 for rising and falling edge to determine PWM length
attachInterrupt(digitalPinToInterrupt(Ch3_pin),Channel3DataRising,RISING);
attachInterrupt(digitalPinToInterrupt(Ch3_pin),Channel3DataFalling,FALLING);

//Enables and disables the launcher relay 1
attachInterrupt(digitalPinToInterrupt(Ch4_pin),Channel4DataRising,RISING);
attachInterrupt(digitalPinToInterrupt(Ch4_pin),Channel4DataFalling,FALLING);

pinMode(M1InA_pin,OUTPUT);
pinMode(M1EnA_pin,OUTPUT);
pinMode(M1InB_pin,OUTPUT);
pinMode(M1EnB_pin,OUTPUT);
pinMode(M1Pwm_pin,OUTPUT);

pinMode(M2InA_pin,OUTPUT);
pinMode(M2EnA_pin,OUTPUT);
pinMode(M2InB_pin,OUTPUT);
pinMode(M2EnB_pin,OUTPUT);
pinMode(M2Pwm_pin,OUTPUT);

pinMode(Ch1_pin,INPUT);
pinMode(Ch2_pin,INPUT);
pinMode(Ch3_pin,INPUT);
pinMode(Ch4_pin,INPUT);
pinMode(Ch5_pin,INPUT);

pinMode(relay1,OUTPUT);
pinMode(relay2,OUTPUT);

//Sets the motor driver to forward and reverse functions only
digitalWrite(M1EnA_pin,HIGH);
digitalWrite(M1EnB_pin,HIGH);
digitalWrite(M2EnA_pin,HIGH);
digitalWrite(M2EnB_pin,HIGH);

//Sets motor1 and motor2 to forward direction on startup
digitalWrite(M1InA_pin,HIGH);
digitalWrite(M1InB_pin,LOW);
digitalWrite(M2InA_pin,HIGH);
digitalWrite(M2InB_pin,LOW);
wdt_enable(WDTO_1S);

}

//MAIN LOOP - THIS WILL BE RUNNING AND LOOPING CONTINUOUSLY
void loop() {
  wdt_reset();//Resets watchdog timer
  MotorControl();//Sets the output values to all motors
  launcher();//Turns the launcher on or off
  
}


//CUSTOM FUNCTIONS - ALL CODE BELOW HERE IS CUSTOM FUNCTIONS WHICH 
//ARE CALLED DURING THE PROGRAM OPERATION
//https://www.arduino.cc/en/Tutorial/PWM
void MotorControl(){
//  Serial.print("\n");
//  Serial.print("Ch2: ");
//  Serial.print(Ch2Pwm);
  //Motor 1
  //If thumbstick is down
  if(Ch2Pwm < Ch2MidPw && Ch2Pwm >= 1000){
      M1_Pwm = M_Ratio * ((Ch2MidPw) - (Ch2Pwm));
        if (M1_Pwm >= 240){
            M1_Pwm = 255;
  }
        else if(M1_Pwm <= 20){//DEADBAND ZONE
             M1_Pwm = 0;
        }
        else{
            M1_Pwm = M1_Pwm;
        }
      Serial.print("\n");
      Serial.print("M1 Reverse: ");
      Serial.print(M1_Pwm);
      Motor1Reverse();
  }
  //If thumbstick is in the middle
  else if ((Ch2Pwm == Ch2MidPw or Ch2Pwm < 1000)){
    M1_Pwm = 0;
    analogWrite(M1Pwm_pin,M1_Pwm);
  }
  //If thumbstick is up
  else if (Ch2Pwm > Ch2MidPw){
    M1_Pwm = M_Ratio * ((Ch2Pwm) - (Ch2MidPw));
      if (M1_Pwm >= 240){
          M1_Pwm = 255;
  }
      else if(M1_Pwm <= 30){//DEADBAND ZONE
          M1_Pwm = 0;
      }
      else{
          M1_Pwm = M1_Pwm;
  }
    Serial.print("\n");
    Serial.print("M1 Forward: ");
    Serial.print(M1_Pwm);
    Motor1Forward();
  }
  else{
  }
  //Motor 2
    //If thumbstick is down 
    if(Ch3Pwm < Ch3MidPw && Ch3Pwm >= 1000){
    M2_Pwm = M_Ratio * ((Ch3MidPw) - (Ch3Pwm));
        if (M2_Pwm >= 240){
            M2_Pwm = 255;
  }
        else if(M2_Pwm <= 20){//DEADBAND ZONE
            M2_Pwm = 0; 
        }
        else{
            M2_Pwm = M2_Pwm;
  }    
    Serial.print("\n");
    Serial.print("M2 Reverse: ");
    Serial.print(M2_Pwm);
    Motor2Reverse();
  }
    //If thumbstick is in the middle
  else if ((Ch3Pwm == Ch3MidPw or Ch3Pwm < 1000)){
    M2_Pwm = 0;
    analogWrite(M2Pwm_pin,M2_Pwm);
  }
  //If thumbstick is up
  else if (Ch3Pwm > Ch3MidPw){
    M2_Pwm = M_Ratio * ((Ch3Pwm) - (Ch3MidPw));
        if (M2_Pwm >= 240){
            M2_Pwm = 255;
  }
        else if(M2_Pwm <= 30){//DEADBAND ZONE
          M2_Pwm = 0;
        }
        else{
            M2_Pwm = M2_Pwm;
  }
    Serial.print("\n");
    Serial.print("M2 Forward: ");
    Serial.print(M2_Pwm);
    Motor2Forward();
  }
  else{
  }
  
}

//The code below is used for setting the dirction and speed of the track motors
//https://www.arduino.cc/en/Reference/digitalWrite
//https://www.arduino.cc/en/Reference/analogWrite
//Sets the direction of Motor1 to Forward
void Motor1Forward(){
  digitalWrite(M1InA_pin,HIGH);
  digitalWrite(M1InB_pin,LOW);
  noInterrupts();
  analogWrite(M1Pwm_pin,M1_Pwm);
  interrupts();
}

//Sets the direction of Motor2 to Forward
void Motor2Forward(){
  digitalWrite(M2InA_pin,HIGH);
  digitalWrite(M2InB_pin,LOW);
  noInterrupts();
  analogWrite(M2Pwm_pin,M2_Pwm);
  interrupts();
}

//Sets the direction of Motor1 to Reverse
void Motor1Reverse(){
  digitalWrite(M1InA_pin,LOW);
  digitalWrite(M1InB_pin,HIGH);
  noInterrupts();
  analogWrite(M1Pwm_pin,M1_Pwm);
  interrupts();
}

//Sets the direction of Motor2 to Reverse
void Motor2Reverse(){
  digitalWrite(M2InA_pin,LOW);
  digitalWrite(M2InB_pin,HIGH);
  noInterrupts();
  analogWrite(M2Pwm_pin,M2_Pwm);
  interrupts();
}

//Turns on the relay for the launcher motor
//Relay 1 is activated
void launcher(){
 
  if (Ch4Pwm >= 1750){
    digitalWrite(relay1,HIGH);
    Serial.print("\n");
    Serial.print("Launcher running");
  }
  else if (Ch4Pwm >= 1000 && Ch4Pwm <= 1200) {
    digitalWrite(relay1,LOW);
  }

  else{
    Ch4Pwm = Ch4Pwm;
  }

    if (Ch1Pwm >= 1750){
    digitalWrite(relay2,HIGH);
    Serial.print("\n");
    Serial.print("Launcher running");
  }
  else if (Ch1Pwm >= 1000 && Ch1Pwm <= 1200) {
    digitalWrite(relay2,LOW);
  }

  else{
    Ch1Pwm = Ch1Pwm;
  }
  
}


//*********************INTERRUPT SERVICE ROUTINE FUNCTIONS***************************
//External ISRs are ideal to use because they are called whenever an external event happens
//This makes it unlikely for us to miss our signal. Otherwise we would have to constantly poll and check 
//The input pins for signal changes.
//http://www.engblaze.com/we-interrupt-this-program-to-bring-you-a-tutorial-on-arduino-interrupts/

//ISR Function to read the PWM signal from Channel 1
void Channel1DataRising(){
    Ch1RisingEdgeTime = micros();
    attachInterrupt(digitalPinToInterrupt(Ch1_pin),Channel1DataFalling,FALLING);      
  }  
void Channel1DataFalling(){
    Ch1FallingEdgeTime = micros();
    Ch1TimeDif = Ch1FallingEdgeTime - Ch1RisingEdgeTime;
    Ch1Pwm = Ch1TimeDif; 
    attachInterrupt(digitalPinToInterrupt(Ch1_pin),Channel1DataRising,RISING);      
  }
  
//ISR Function to read the PWM signal from Channel 2
void Channel2DataRising(){
    Ch2RisingEdgeTime = micros();
    attachInterrupt(digitalPinToInterrupt(Ch2_pin),Channel2DataFalling,FALLING);
  }
void Channel2DataFalling(){
//    Serial.print("Falling Edge ISR");
//    Serial.print("\n");
    Ch2FallingEdgeTime = micros();
    Ch2TimeDif = Ch2FallingEdgeTime - Ch2RisingEdgeTime;
    Ch2Pwm = Ch2TimeDif;
    attachInterrupt(digitalPinToInterrupt(Ch2_pin),Channel2DataRising,RISING); 
  }

//ISR Function to read the PWM signal from Channel 3
void Channel3DataRising(){
    Ch3RisingEdgeTime = micros();
    attachInterrupt(digitalPinToInterrupt(Ch3_pin),Channel3DataFalling,FALLING);  
  } 
void Channel3DataFalling(){
    Ch3FallingEdgeTime = micros();
    Ch3TimeDif = Ch3FallingEdgeTime - Ch3RisingEdgeTime;
    Ch3Pwm = Ch3TimeDif;
    attachInterrupt(digitalPinToInterrupt(Ch3_pin),Channel3DataRising,RISING);     
  }
  
//ISR Function to read the PWM signal from Channel 4
void Channel4DataRising(){
    Ch4RisingEdgeTime = micros();
    attachInterrupt(digitalPinToInterrupt(Ch4_pin),Channel4DataFalling,FALLING);  
     
  } 
void Channel4DataFalling(){
    Ch4FallingEdgeTime = micros();
    Ch4TimeDif = Ch4FallingEdgeTime - Ch4RisingEdgeTime;
    Ch4Pwm = Ch4TimeDif;  
    attachInterrupt(digitalPinToInterrupt(Ch4_pin),Channel4DataRising,RISING);      
  }
//****************END OF INTERRUPT SERVICE ROUTINE FUNCTIONS**********************





