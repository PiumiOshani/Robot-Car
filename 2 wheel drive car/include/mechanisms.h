#pragma once
#include <ESP32Servo.h>
#include "config.h"
#include  "Arduino.h"
#include "sensors.h"

class Mechanisms;
extern Mechanisms mechanisms;

enum{
  UP,
  DOWN,
  OPEN,
  CLOSED,
  FREE,
};

class Mechanisms
{
public:
        Servo lifting_servo;
        Servo grabbing_servo;
        int arm_state = FREE;
        int arm_position = FREE;

void begin()
{
        lifting_servo.attach(LIFTING_SERVO_PIN);
        grabbing_servo.attach(GRABBING_SERVO_PIN);
}
void lift(){
  if (arm_state != UP){
    for (int pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    lifting_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
  }
}
void lower(){
    if (arm_position != DOWN){
        for (int pos = 180; pos >=0; pos -= 1) { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
        lifting_servo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(15);                       // waits 15 ms for the servo to reach the position
        }
    }
}
void grab(){

  if (arm_state != CLOSED){
    for (int pos = 180; pos >=0; pos -= 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    lifting_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
  }
}
void release(){
  if (arm_state != OPEN){
      for (int pos = 180; pos >=0; pos -= 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    lifting_servo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
  }
}
};