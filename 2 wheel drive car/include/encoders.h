#pragma once

#include <Arduino.h>
#include <stdint.h>
#include "config.h"
#include <math.h>

class Encoders;

extern Encoders encoders;

class Encoders
{
public:
    void begin()
    {
        pinMode(LeftEncoderPin1, INPUT_PULLUP);
        pinMode(LeftEncoderPin2, INPUT_PULLUP);

        pinMode(RightEncoderPin1, INPUT_PULLUP);
        pinMode(RightEncoderPin2, INPUT_PULLUP);

        attachInterrupt(digitalPinToInterrupt(LeftEncoderPin1), updateLeftEncoderISR, CHANGE);
        attachInterrupt(digitalPinToInterrupt(LeftEncoderPin2), updateLeftEncoderISR, CHANGE);

        attachInterrupt(digitalPinToInterrupt(RightEncoderPin1), updateRightEncoderISR, CHANGE);
        attachInterrupt(digitalPinToInterrupt(RightEncoderPin2), updateRightEncoderISR, CHANGE);

        reset();
    }
    

    void reset()
    {
        noInterrupts();

        encoderCounterLeft = 0;
        encoderCounterRight = 0;
        robot_distance = 0;
        robot_angle = 0;

        interrupts();
    }

    static void updateLeftEncoderISR()
    {
        encoders.updateLeftEncoder();
    }


    static void updateRightEncoderISR()
    {
        encoders.updateRightEncoder();
    }

    void updateLeftEncoder()
    {
        int MSB = digitalRead(LeftEncoderPin1);
        int LSB = digitalRead(LeftEncoderPin2);

        int encoded = (MSB << 1) | LSB;

        int sum = (lastEncodedLeft << 2) | encoded;

        if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
        {
            encoderCounterLeft--;
        }
        else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)
        {
            encoderCounterLeft++;
        }

        lastEncodedLeft = encoded;
    }

    void updateRightEncoder()
    {
        int MSB = digitalRead(RightEncoderPin1);
        int LSB = digitalRead(RightEncoderPin2);

        int encoded = (MSB << 1) | LSB;

        int sum = (lastEncodedRight << 2) | encoded;

        if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
        {
            encoderCounterRight--;
        }
        else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)
        {
            encoderCounterRight++;
        }

        lastEncodedRight = encoded;
    }

    void update()
    {
        unsigned long currentTime = micros();

        left_delta = 0;
        right_delta = 0;
        

        time_change_1 = prevTime;
        time_change_u = currentTime-prevTime;
        if (time_change_u==0){
            time_change_u = 1;
        }
    

        prevTime = currentTime;
        noInterrupts();
        left_delta = encoderCounterLeft;
        right_delta = encoderCounterRight;
        encoderCounterLeft = 0;
        encoderCounterRight = 0;
        interrupts();

        float left_change = (float)left_delta * MM_PER_ROTATION/ PULSES_PER_ROTATION;
        float right_change = (float)right_delta * MM_PER_ROTATION/ PULSES_PER_ROTATION;


        fwd_change = 0.5 * (right_change + left_change); // taking average, distance in millimeters
        robot_distance += fwd_change;
        rot_change = (right_change - left_change) * DEG_PER_MM_DIFFERENCE;
        robot_angle += rot_change;
    }

    inline int loopTime_us(){
        int looptime = time_change_u;
        return looptime;
    }

    inline float loopTime_s(){
        float time = (float)time_change_u/1000000.0;
        return time;
    }
    
    inline float robotDistance()
    {
        float distance;

        noInterrupts();
        distance = robot_distance; //in mm
        interrupts();

        return distance;
    }

    inline float robotAngle()
    {
        float angle;

        noInterrupts();
        angle = robot_angle;
        interrupts();

        return angle;
    }

    inline float robot_speed(){
        float speed ;

        noInterrupts();
        speed = (fwd_change/ time_change_u)* 1000000;
        interrupts();

        return speed;
    }

    inline float robot_omega(){   /////given in degrees per second!!!!!
        float omega;

        noInterrupts();
        omega = (rot_change/time_change_u)* 1000000;
        interrupts();

        return omega;
    }

    inline float robot_fwd_change()
    {
        float distance;

        noInterrupts();
        distance = fwd_change;
        interrupts();

        return distance;
    }

    inline float robot_rot_change()
    {
        float distance;
        
        noInterrupts();
        distance = rot_change;
        interrupts();

        return distance;
    }

    inline float leftRPS(){
        float rps;

        noInterrupts();
        float left_delta_read = left_delta;
        interrupts();

        rps = (left_delta_read/time_change_u)*(1000000.0/PULSES_PER_ROTATION); //encoderCounterLeft

        return rps;
    }

    inline float rightRPS(){
        float rps;

        noInterrupts();
        float right_delta_read = right_delta;
        interrupts();

        rps = right_delta_read/time_change_u*(1000000.0/PULSES_PER_ROTATION); 
        
        return rps;
    }


private:
    volatile long encoderCounterLeft; // Encoder roatation count, this gets reset every time we call update
    volatile long lastEncodedLeft;    // Last encoded value

    int left_delta; //this variable holds the number of encoder counts during two update calls
    int right_delta ;

    volatile long encoderCounterRight;// Encoder roatation count, this gets reset every time we call update
    volatile long lastEncodedRight;

    volatile float robot_distance; // the complete distance travel by robot, this get's incremented using the update function
    volatile float robot_angle; // same like above

    unsigned long prevTime;
    // the change in distance or angle in the last tick.
    float fwd_change; //difference 
    float rot_change;
    float time_change_u;
    int time_change_1;
    int time_change_2;
    int time_change_3;
};