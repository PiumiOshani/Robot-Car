#pragma once

#include <Arduino.h>
#include "config.h"
#include "encoders.h"


class Motors;

extern Motors motors;

class Motors
{  
public:
float speed = 0;
float omega = 0;
volatile float fwdKp = FWD_KP;
volatile float fwdKd = FWD_KD;
volatile float fwdKi = FWD_KI;
volatile float rotKp = ROT_KP;
volatile float rotKd = ROT_KD;
volatile float rotKi = ROT_KI;

void begin()
  {
    pinMode(LEFT_MOTOR_PWM, OUTPUT);
    pinMode(LEFT_MOTOR_IN1, OUTPUT);
    pinMode(LEFT_MOTOR_IN2, OUTPUT);
    pinMode(RIGHT_MOTOR_IN2, OUTPUT);
    pinMode(RIGHT_MOTOR_IN1, OUTPUT);
    pinMode(RIGHT_MOTOR_PWM, OUTPUT);

    digitalWrite(LEFT_MOTOR_PWM, 0);
    digitalWrite(LEFT_MOTOR_IN1, 0);
    digitalWrite(LEFT_MOTOR_IN2, 0);
    digitalWrite(RIGHT_MOTOR_IN2, 0);
    digitalWrite(RIGHT_MOTOR_IN1, 0);
    digitalWrite(RIGHT_MOTOR_PWM, 0);
    setupPWM();
  }
    void reset_controllers()
  {
    m_fwd_error = 0;
    m_rot_error = 0;
    m_previous_fwd_error = 0;
    m_previous_rot_error = 0;
  }
  float position_controller()
  {
    float increment = m_velocity * encoders.loopTime_s();
    m_fwd_error += increment - encoders.robot_fwd_change();
    float diff = m_fwd_error - m_previous_fwd_error;
    m_previous_fwd_error = m_fwd_error;
    acc_fwd_error += m_fwd_error;

    // change them to config kp kd later
    float output = fwdKp * m_fwd_error + fwdKd * diff+fwdKi*acc_fwd_error;
    return output;
  }

  float angle_controller(float steering_adjustment)
  {
    float increment = m_omega * encoders.loopTime_s();
    m_rot_error += increment - encoders.robot_rot_change();
    float diff = m_rot_error - m_previous_rot_error;

    // Serial.print(" m_rot_error ");
    // Serial.print(m_rot_error);
    // Serial.print("  ");

    m_previous_rot_error = m_rot_error;
    m_rot_error -= steering_adjustment;
    acc_rot_error += m_rot_error;

    // Serial.print("Steering  :");
    // Serial.print(steering_adjustment);

    // changethis kp kd to config kp kd later
    float output = rotKp * m_rot_error + rotKd * diff+rotKi*acc_rot_error;
    return output;
  }

  void update(float velocity, float omega, float steering)
  {
    m_velocity = velocity;
    m_omega = omega;

    float pos_output = position_controller();
    float rot_output = angle_controller(steering);
    float left_output = 0;
    float right_output = 0;

    left_output = pos_output - rot_output;
    right_output = pos_output + rot_output;

    float tangent_speed = m_omega * ROBOT_RADIUS * RADIANS_PER_DEGREE;
    float left_speed = m_velocity - tangent_speed;
    float right_speed = m_velocity + tangent_speed;
    float left_ff = left_feed_forward_percentage(left_speed);
    float right_ff = right_feed_forward_percentage(right_speed);
    if (m_feedforward_enabled)
    {
      left_output += left_ff;
      right_output += right_ff;
    }
    if (m_controller_output_enabled)
    {
      set_left_motor_percentage(left_output);
      set_right_motor_percentage(right_output);


      // Serial.print("  left  : ");
      // Serial.print(left_output);

      // Serial.print("   right  : ");
      // Serial.println(right_output);
    }
  }

  float left_feed_forward_percentage(float left_feed_velocity)
  {
    ///////give the percentage required to acheive a given velocity--- |v|<500
    //int l_rps = (left_feed_velocity * PULSES_PER_ROTATION) / MM_PER_ROTATION;
    float v = left_feed_velocity;//     /2 is due to a mistake in calculations
    //Serial.print("left feed  ");
    //Serial.println(v);
    //v = 0.0533*v*v*v-0.1899*v*v+1.1948*v;  //compensation for motor miss match
    float l_feed_percentage;
    if (v>=0){
        l_feed_percentage = 0.14*v+2.0;//(-0.0004*v*v)+(0.3113*v)+43.2991;//23.2991
    }
    else {
      //v=-v;
      l_feed_percentage = 0.14*v-6.0;//-((-0.0004*v*v)+(0.3113*v)+53.2991);//23.2991
    }
    // Serial.print("  left   ");
    // Serial.print(l_feed_percentage);
    return l_feed_percentage;
  }

  float right_feed_forward_percentage(float right_feed_velocity)
  {
    ///////give the percentage required to acheive a given velocity--- |v|<500
    //int r_rps = (left_feed_velocity * PULSES_PER_ROTATION) / MM_PER_ROTATION;
    float v = right_feed_velocity;//     /2 is due to a mistake in calculations
    //Serial.print("  right feed  ");
    //Serial.print(v);
    float r_feed_percentage;
    if (v>0){
      r_feed_percentage = 0.12*v+3.3;}
    else {
      r_feed_percentage = 0.12*v-3.3;
    }
    // if(v>=0){
    //   r_feed_percentage = (0.0003*v*v*v)-(0.0177*v*v)+(0.4125*v)+2.8691;//(0.2*v*v+68.83*v+45000)/1023.0;
    // }
    // else{
    //   v = -v;
    //   r_feed_percentage = -((0.0003*v*v*v)-(0.0177*v*v)+(0.4125*v)+2.8691);
    // }
    // Serial.print("   right   ");
    // Serial.println(r_feed_percentage);
    return r_feed_percentage;
  }
  void stop()
  {
    set_left_motor_percentage(0);
    set_right_motor_percentage(0);
  }
    void set_left_motor_percentage(float percentage)
  {
    percentage = constrain(percentage, -maxMotorPercentage, maxMotorPercentage);
    // if (percentage > LEFT_MIN_MOTOR_PERCENTAGE)
    // {
    //   //percentage = map(percentage, LEFT_MIN_MOTOR_PERCENTAGE, maxMotorPercentage, MIN_MOTOR_BIAS, maxMotorPercentage);
    // }
    // else if (percentage < -LEFT_MIN_MOTOR_PERCENTAGE)
    // {
    //   //percentage = map(percentage, -maxMotorPercentage, -LEFT_MIN_MOTOR_PERCENTAGE, -maxMotorPercentage, -MIN_MOTOR_BIAS);
    // }
    // else if (-LEFT_MIN_MOTOR_PERCENTAGE <= percentage <= LEFT_MIN_MOTOR_PERCENTAGE)
    // {
    //   percentage = 0;
    // }
    int left_pwm = calculate_pwm(percentage);
    

    set_left_motor_pwm(left_pwm);
  }
    void set_right_motor_percentage(float percentage)
  {
    percentage = constrain(percentage, -maxMotorPercentage, maxMotorPercentage);
    // if (percentage > RIGHT_MIN_MOTOR_PERCENTAGE)
    // {
    //   //percentage = map(percentage, RIGHT_MIN_MOTOR_PERCENTAGE, maxMotorPercentage, MIN_MOTOR_BIAS, maxMotorPercentage);
    // }
    // else if (percentage < -RIGHT_MIN_MOTOR_PERCENTAGE)
    // {
    //   //percentage = map(percentage, -maxMotorPercentage, -RIGHT_MIN_MOTOR_PERCENTAGE, -maxMotorPercentage, -MIN_MOTOR_BIAS);
    // }
    // else if (-RIGHT_MIN_MOTOR_PERCENTAGE <= percentage <= RIGHT_MIN_MOTOR_PERCENTAGE)
    // {
    //   percentage = 0;
    // }

    m_right_motor_percentage = percentage;
    int right_pwm = calculate_pwm(percentage);
    

    // Serial.print("   right pwm percentage: ");
    // Serial.println(percentage);
    
    set_right_motor_pwm(right_pwm);
  }
  void set_left_motor_pwm(int pwm)
  {
    pwm = MOTOR_LEFT_POLARITY * pwm;
    if (pwm < 0)
    {
      pwm = -pwm + M_BALNCE_PWM;
      digitalWrite(LEFT_MOTOR_IN1, HIGH);
      digitalWrite(LEFT_MOTOR_IN2, LOW);
      ledcWrite(2, pwm);
    }
    else
    {
      pwm = pwm + M_BALNCE_PWM;
      digitalWrite(LEFT_MOTOR_IN1, LOW);
      digitalWrite(LEFT_MOTOR_IN2, HIGH);
      ledcWrite(2,pwm);
    }
  }
    void set_right_motor_pwm(int pwm)
  {
    pwm = MOTOR_RIGHT_POLARITY * pwm;
    if (pwm < 0)
    {
      pwm = -pwm - M_BALNCE_PWM;
      digitalWrite(RIGHT_MOTOR_IN1, HIGH);
      digitalWrite(RIGHT_MOTOR_IN2, LOW);
      ledcWrite(1, pwm);
    }
    else
    {
      pwm = pwm - M_BALNCE_PWM;
      digitalWrite(RIGHT_MOTOR_IN1, LOW);
      digitalWrite(RIGHT_MOTOR_IN2, HIGH);
      ledcWrite(1, pwm);
    }
  }

    int calculate_pwm(float desired_percentage)
  {
    int pwm = MAX_MOTOR_PERCENTAGE * PWM_RESOLUTION * desired_percentage / 10000;
    return pwm;
  }

    void setupPWM()
  {
    ledcSetup(2, 5000, PWM_RESOLUTION_BITS);
    ledcAttachPin(LEFT_MOTOR_PWM, 2);
    ledcSetup(1, 5000, PWM_RESOLUTION_BITS);
    ledcAttachPin(RIGHT_MOTOR_PWM, 1);
  }
  void enable_controllers()
  {
    m_controller_output_enabled = true;
  }

  void disable_controllers()
  {
    m_controller_output_enabled = false;
  }
  void calibrate_motors(){
    int i = 150;
     for (int j=1;j<25;j++){
      int speed = 20*j;
      int start = millis();
      int avg_PWM = 0;
      int avg_speed = 0;
      double count = 0;
      while (millis()-start<8000){
        motors.set_left_motor_pwm(i);
        if (encoders.robot_speed()*2<speed)
          i=i+1;

        if (encoders.robot_speed()*2>speed)
          i=i-1;

        
        encoders.update();
        // Serial.print(i);
        // Serial.print("  desired speed   ");
        // Serial.print(speed);
        // Serial.print("      right speed  ");
        // Serial.println(encoders.robot_speed()*2);
        delay(5);
        avg_PWM += i;
        avg_speed += encoders.robot_speed()*2;
        count++;
    }
    //Serial.print("  desired speed(mm/s)   ");
    //Serial.print(speed);
    //Serial.print("   PWM     ");
    // Serial.print(avg_PWM/count);
    // Serial.print("  ");
    // Serial.print(speed);
    // Serial.print("  ");
    // //Serial.print("   Avg_Speed   ");
    // Serial.println(avg_speed/count);
    }
  }
private:
  float m_left_motor_percentage;
  float m_right_motor_percentage;

  float m_previous_fwd_error;
  float m_previous_rot_error;
  float m_fwd_error;
  float m_rot_error;
  float acc_fwd_error;
  float acc_rot_error;

  float m_velocity;
  float m_omega;

  bool m_feedforward_enabled = true;
  bool m_controller_output_enabled;
  unsigned long i = 0;
};

//The code used to find the feed forward percentages


  //disable_controllers()
  // sensors.set_steering_mode(STEERING_OFF);
  //  for (int i=-1023;i<1023;i=i+32){

  //   motors.set_left_motor_pwm(i);
  //   motors.set_right_motor_pwm(0);
  //   float avg_speed = 0;
  //   for (int j=0;j<100; j++){
  //     avg_speed += encoders.robot_speed();
  //     delay(20);
  //   }
  //   avg_speed = avg_speed/100;
  //   Serial.print(i);
  //   Serial.print("  percentage ");
  //   Serial.print( i*100/1023);
  //   Serial.print("  velocity ");
  //   Serial.println(avg_speed);
  // }