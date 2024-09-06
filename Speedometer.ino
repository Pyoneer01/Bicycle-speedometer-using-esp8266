#include <math.h>
#include <Servo.h>

#define hallPin D7

#define servoPin D4

Servo servo;

const double dpr = 2.0747; //distance covered per one wheel rotation in meters

unsigned long prevTime = 0;

double speed = 0.0;

unsigned long duration = 0;

unsigned long servoWriteTime = 0;

int prevDeg = 180;

int maxSpeed = 40; //assuming max speed is 40 kmph

const int k = 3;  //assumed time taken in milliseconds for servo to write one degree

int started = 0;

volatile int newValue = 0;

void IRAM_ATTR updateSpeed(){
  newValue = 1;
}

void setup() {
    servo.attach(servoPin, 500, 2500);
    pinMode(hallPin, INPUT);
    attachInterrupt(hallPin, updateSpeed, RISING);
    delay(2000);
    servo.write(180); //initializing servo to show 0 speed
    delay(1000);
}

void loop() {      
  if(newValue){
    duration = millis() - prevTime;
    if(duration > 187){  //maxSpeed allowed is 40kmph(min duration 187ms)
      prevTime = millis();
      if(!started){
        started = 1;
        return;
      }
      if(duration > servoWriteTime){  //checking if servo finished writing previous speed value
        speed = (dpr/(duration*0.001))*3.6; 
        int deg = 180 - (int)round((speed * 180.0) / maxSpeed); //calulate how much degrees servo writes based on speed
        servo.write(deg);
        servoWriteTime = abs(deg - prevDeg)*k;
        prevDeg = deg;
      }
      else{ //servo may still be moving
        servoWriteTime = servoWriteTime - duration;
      }
    }
    newValue = 0;
  }            
  if(millis() - prevTime > 5000){ //speed too less to be significant
    prevTime = millis();
    speed = 0;
    servo.write(180);
    servoWriteTime = abs(180 - prevDeg)*k; 
    prevDeg = 180;
  }
}
