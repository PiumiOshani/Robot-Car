//config files to match our robot specificatiojns

#pragma once
#include "esp_system.h"



//*****************************communications******************************* */

const char* SSID = "Nilakna";//"SLT-ADSL-92776";//"SLT-4G_WataNandun";//;   // network credentials
const char* PASSWORD = "gopt1728";//"J1234567890";//"Nwata@#com";//

const int LOCAL_PORT = 12345;  // UDP port to listen on

const char* REMOTE_IP = "192.168.253.202";//"192.168.1.8";//"192.168.1.3";
const int REMOTE_PORT = 3333;




//*****************************encoder pin allocation***********************
const int LeftEncoderPin1 = 16;//15;
const int LeftEncoderPin2 = 17;//4;

const int RightEncoderPin1 = 4;//17;
const int RightEncoderPin2 = 15;//16; 

//****************************motor********************************************* */
const int LEFT_MOTOR_PWM = 13;
const int LEFT_MOTOR_IN1 = 12;
const int LEFT_MOTOR_IN2 = 14;

const int RIGHT_MOTOR_PWM = 5;
const int RIGHT_MOTOR_IN1 = 18;
const int RIGHT_MOTOR_IN2 = 19;

const int PWM_RESOLUTION_BITS = 10;
const int PWM_RESOLUTION = 1023; //2^10-1

const int MAX_MOTOR_PERCENTAGE = 95;

const int MOTOR_RIGHT_POLARITY = 1;// change this to change direction of motor direction
const int MOTOR_LEFT_POLARITY = 1;

const int M_BALNCE_PWM = 0; //adjust this value to bring both the motors to same speed

const int LEFT_MIN_MOTOR_PERCENTAGE = 20;
const int RIGHT_MIN_MOTOR_PERCENTAGE = 13;
const int MIN_MOTOR_BIAS = 5;
const int maxMotorPercentage = 100;

//******************************robot dimensions*****************************/
const float MM_PER_ROTATION = 201.0;//tyre
const float PULSES_PER_ROTATION = 1320.0;
const float ROBOT_RADIUS = 92.5;  //there is some error in this. Although it says radius put Wheel to wheel diameter
const float DEG_PER_MM_DIFFERENCE = 180.0/(2*ROBOT_RADIUS*PI);

const float ARRAY_TO_WHEEL_DISTANCE = 60;//65; //change this accordingly



const float RADIANS_PER_DEGREE = 0.0175;

const float ROT_KP = 1.5;//7;
const float ROT_KD = 0.6;//19;
const float ROT_KI = 0;
const float FWD_KP = 1.2;//2.5;//6.0;//7.0//1.0
const float FWD_KD = 0.5;//10;
const float FWD_KI = 0;
const float STR_KP = 2.5;//100;
const float STR_KD = 0;

//****************************************Robot*****************************************//
const float MOVE_SPEED = 100;//100//60.0;
const float MOVE_ACC = 100;//10//1000; //720//1000
const float IMMEDIATE_ACC = 2000;

const float OMEGA_TURN = 120;
const float ALPHA_TURN = 90;//5.0;//1500
const float LEFT = 85.0;
const float RIGHT = -85.0;
const float ABOUTTURN = 180;



//*****************************************Sensors************************** */
const float STEERING_ADJUST_LIMIT = 100.0;
const int ERROR_POLARITY = 1;

const int NUM_SENSORS = 8;
const float SENSOR_THRESHOLD = 80.0;//97.0;//93.5;
const bool INVERT_COLOR = false;

const int BUTTON_PIN = 0;
const int LED_PIN = 2;

const int LEFT_LINE_PIN = 36;
const int RIGHT_LINE_PIN = 39;
const int POTATO_IR_PIN = 25;

const int XSHUT_PIN = 25;
const float OBJECT_DETECT_RANGE = 300;

//******************************************Arena Specs********************** */
const float LINE_LENGTH = 300.0;
const float LINE_WIDTH = 30.0;
const float MAX_BARCODE_LENGTH = 870.0;
const float TASK2_TOTAL_LENGTH = 1050.0;
//******************************************Mechanisms************************ */

const int LIFTING_SERVO_PIN = 27;
const int GRABBING_SERVO_PIN = 26;