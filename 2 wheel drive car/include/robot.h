#pragma once

#include <Arduino.h>
#include "encoders.h"
#include "config.h"
#include "motion.h"
#include "sensors.h"

class Robot;
extern Robot robot;

class Robot
{
public:

    bool barcode_finished = false;
    int inputArray[40] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int box_position;
    int last_scanned_pos = 0;

    void move(float distance)
    {
        sensors.set_steering_mode(STEER_NORMAL);
        motion.reset_drive_system();
        motion.start_move( distance, MOVE_SPEED, 0, MOVE_ACC);
        while (!motion.move_finished())
        {   
            // if (sensors.line_state == LINE){
            //     sensors.g_steering_mode = STEER_NORMAL;
            // }
            // else {
            //     sensors.g_steering_mode = STEERING_OFF;
            // }
            delayMicroseconds(2);

            //if (sensors.frontWallExist)
            //{
            //    stopAndAdjust();
            //    break;
            //}
        }
    }
  


    void move_straight(float distance)
    {
        sensors.set_steering_mode(STEERING_OFF);
        motion.reset_drive_system();
        motion.start_move( distance, MOVE_SPEED, 0, MOVE_ACC);
        while (!motion.move_finished())
        {   
            // if (sensors.line_state == LINE){
            //     sensors.g_steering_mode = STEER_NORMAL;
            // }
            // else {
            //     sensors.g_steering_mode = STEERING_OFF;
            // }
            delayMicroseconds(2);

            //if (sensors.frontWallExist)
            //{
            //    stopAndAdjust();
            //    break;
            //}
        }
    }

    void move_till_junction(float distance)
    {   
        bool junction_detected = false;
        sensors.set_steering_mode(STEER_NORMAL);
        motion.reset_drive_system();
        motion.start_move( distance , MOVE_SPEED, 0, MOVE_ACC);
        while (!motion.move_finished())
        {   
            if (sensors.line_state == LINE){
                sensors.g_steering_mode = STEER_NORMAL;
            }
            else {
                sensors.g_steering_mode = STEERING_OFF;
            }
            
            if (sensors.line_state ==LEFT_LINE or sensors.line_state ==RIGHT_LINE){  //detect if the expected junction is reached
                junction_detected = true;
                delay(6);
                sensors.last_junction = sensors.line_state;
            }
            if (sensors.line_state == CROSS_OR_T){  //detect if the expected junction is reached
                junction_detected = true;
                sensors.last_junction = sensors.line_state;
            }
            if (junction_detected){//only allign to the junction if (expected junction distance reached and) junction passed.
                align_to_juction();
                break;
            }
            delayMicroseconds(2);

        }
    }


    void move_till_line(float distance)
    {   
        bool junction_detected = false;
        sensors.set_steering_mode(STEERING_OFF);
        motion.reset_drive_system();
        motion.start_move( distance , MOVE_SPEED, 0, MOVE_ACC);
        while (!motion.move_finished())
        {   
            if (sensors.line_state == LINE){
                sensors.g_steering_mode = STEER_NORMAL;
            }
            else {
                sensors.g_steering_mode = STEERING_OFF;
            }
            
            if (sensors.line_state ==LEFT_LINE or sensors.line_state ==RIGHT_LINE){  //detect if the expected junction is reached
                junction_detected = true;
                delay(61);
                sensors.last_junction = sensors.line_state;
            }
            if (sensors.line_state == CROSS_OR_T){  //detect if the expected junction is reached
                junction_detected = true;
                sensors.last_junction = sensors.line_state;
            }
            if (junction_detected){//only allign to the junction if (expected junction distance reached and) junction passed.
                align_to_juction();
                break;
            }
            delayMicroseconds(2);

        }
    }


    bool move_till_wall_task2(float distance)//input the distance travelled in the task2 so far
    {   
        distance = TASK2_TOTAL_LENGTH - distance;
        bool junction_detected = false;
        sensors.set_steering_mode(STEERING_OFF);
        motion.reset_drive_system();
        motion.start_move( distance , MOVE_SPEED, 0, MOVE_ACC);
        while (!motion.move_finished())
        {   
           if (sensors.is_wall_present()){
            	motion.stop();
                return false;
           }

            delayMicroseconds(2);

        }
        return true;
    }

    void move_till_wall(float distance) {
        sensors.set_steering_mode(STEERING_OFF);
        motion.reset_drive_system();
        motion.start_move( distance , MOVE_SPEED, 0, MOVE_ACC);
        while (!motion.move_finished())
        {
            if (sensors.is_wall_present()){
                motion.stop();
                break;
                }
            delayMicroseconds(2);
        }
    }

    bool move_till_potato(float distance)
    {   
        bool junction_detected = false;
        sensors.set_steering_mode(STEERING_OFF);
        motion.reset_drive_system();
        motion.start_move( distance , MOVE_SPEED, 0, MOVE_ACC);
        while (!motion.move_finished())
        {   
            if (sensors.is_potato_present()){
                motion.stop();
                return true;
            }
            if (sensors.line_state == LINE){
                sensors.g_steering_mode = STEER_NORMAL;
            }
            else {
                sensors.g_steering_mode = STEERING_OFF;
            }
            
            if (sensors.line_state ==LEFT_LINE or sensors.line_state ==RIGHT_LINE){  //detect if the expected junction is reached
                junction_detected = true;
                delay(6);
                sensors.last_junction = sensors.line_state;
            }
            if (sensors.line_state == CROSS_OR_T){  //detect if the expected junction is reached
                junction_detected = true;
                sensors.last_junction = sensors.line_state;
            }
            if (junction_detected){//only allign to the junction if (expected junction distance reached and) junction passed.
                align_to_juction();
                return false;
                break;
            }



            delayMicroseconds(2);

        }
    }


    void navigate_dashed_lines(){
        move(1000);
        
    }
    void align_to_juction(){
        sensors.set_steering_mode(STEERING_OFF);
        //motion.reset_drive_system();
        motion.start_move( ARRAY_TO_WHEEL_DISTANCE , encoders.robot_speed(), 0, 10*MOVE_ACC);
        while (!motion.move_finished()){ 
            // if (sensors.line_state == LINE){
            //     sensors.g_steering_mode = STEER_NORMAL;
            // }
            // else {
            //     sensors.g_steering_mode = STEERING_OFF;
            // }
            delay(2);
          }

    }

    void move_till_junction_and_turn(){
        robot.move_till_junction(30000);
        delay(250);
        if (sensors.last_junction == CROSS_OR_T){
            sensors.led_indicator(true);
            Serial.print("CROSS_OR_T");
            robot.move(100);
            delay(250);//robot.turn(77);
        }
        else if (sensors.last_junction == RIGHT_LINE){
            Serial.print("RIGHT_LINE");
            robot.turn(RIGHT);
            delay(1000);
            //robot.move(50);
        }
        else if (sensors.last_junction == LEFT_LINE){
            Serial.print("LEFT_LINE");
            robot.turn(LEFT);
            delay(1000);
            //robot.move(50);
        }
        sensors.last_junction = LINE;
        delay(250);
        sensors.led_indicator(false);
    }

    void turn_180()
    {
        sensors.set_steering_mode(STEERING_OFF);
        static int direction = 1;
        direction *= -1; // alternate direction each time it is called

        motion.reset_drive_system();

        motion.start_turn(direction * 180, OMEGA_TURN, 0, ALPHA_TURN);

        while (!motion.turn_finished())
        {
            delayMicroseconds(2);
        }
        delay(1);
    }

    void turn(float angle)
    {
        sensors.set_steering_mode(STEERING_OFF);
        motion.reset_drive_system();
        motion.start_turn(angle, OMEGA_TURN, 0, ALPHA_TURN);

        while (!motion.turn_finished())
        {
            delayMicroseconds(2);
        }
    }


};


//   void scan_barcode(){
//         encoders.reset();
//         sensors.set_steering_mode(STEERING_OFF);
//         motion.reset_drive_system();
//         motion.start_move( MAX_BARCODE_LENGTH, MOVE_SPEED, 0, MOVE_ACC);
//         int length_to_next_strip = 250-165-15;//70
//         while (!motion.move_finished())
//         {  
//             Serial.println(encoders.robotDistance());
//             if (encoders.robotDistance()>=(250-165+15)){//70
//                 while(!barcode_finished || !motion.move_finished()){
//                     if ((int)encoders.robotDistance()>870){
//                         return;
//                     }
//                     if ((int)encoders.robotDistance()>length_to_next_strip){
//                         length_to_next_strip +=30;
                        
//                         if (sensors.line_state == CROSS_OR_T){
//                             inputArray[last_scanned_pos] = 1;
//                             sensors.led_indicator(1);
//                             if (last_scanned_pos>7){
//                                 if(inputArray[last_scanned_pos]==0 &&inputArray[last_scanned_pos-1]==1 && inputArray[last_scanned_pos-2]==0 && inputArray[last_scanned_pos-3]==1 && inputArray[last_scanned_pos-4]==0 && inputArray[last_scanned_pos-5]==1 && inputArray[last_scanned_pos-6]==0){
//                                     barcode_finished = true;
//                                     for (int i=0; i<40;i++){
//                                         Serial.print(inputArray[i]);
//                                     }
//                                     decode_barcode();
//                                     align_to_juction();
                                
//                                 }
//                                 if(inputArray[last_scanned_pos]==0 && inputArray[last_scanned_pos]==0 && inputArray[last_scanned_pos]==0){
//                                     //barcode_finished = true;
//                                     for (int i=0; i<40;i++){
//                                         Serial.print(inputArray[i]);
//                                     }
//                                     // decode_barcode();
//                                     //align_to_juction();
                                
//                                 }
//                             }

//                         }
//                         else if (sensors.line_state != CROSS_OR_T){
//                             inputArray[last_scanned_pos] = 0;
//                             sensors.led_indicator(0);


//                         }
//                         // else if (sensors.line_state == LINE){
//                         //     barcode_finished = true;
//                         //     decode_barcode();
//                         //     align_to_juction();
//                         // }
//                         Serial.print(last_scanned_pos);
//                         last_scanned_pos++;

//                     }
//                     delay(2);
//                 }
//             }

//             delayMicroseconds(2);
//         }
        
//     }
//     void decode_barcode() {
//         int inputIndex = 0;   // Index for traversing the input array.
//         int outputIndex = 0;  // Index for storing the decoded values.
//         byte decodedArray[15] = {0}; // Initialize decoded array.

//         while (inputIndex < 30) {
//             // Check if the current bit and the next bit form "11".
//             if (inputArray[inputIndex] == 1 && inputArray[inputIndex + 1] == 1) {
//                 decodedArray[outputIndex] = 1; // Decode as 1.
//                 inputIndex += 2;               // Skip two bits.
//             } else if (inputArray[inputIndex] == 1) {
//                 decodedArray[outputIndex] = 0; // Decode as 0.
//                 inputIndex += 1;               // Skip one bit.
//             } else {
//                 inputIndex++;                  // Skip invalid or zero bits.
//                 continue;
//             }

//             outputIndex++; // Move to the next output position.
//         }

//         // Find the index of the last non-zero element in `decodedArray`.
//         int decimalValue = 0;
//         int effectiveLength = 0;
//         for (int i = 14; i >= 0; i--) {
//             if (decodedArray[i] != 0) {
//                 effectiveLength = i + 1;
//                 break;
//             }
//         }

//         // Convert the truncated `decodedArray` into a decimal number.
//         for (int i = 0; i < effectiveLength; i++) {
//             decimalValue = (decimalValue << 1) | decodedArray[i];
//         }

//         // Calculate the box position.
//         int box_position = decimalValue % 5;

//         // Print the results.
//         Serial.print("Decoded Decimal Value: ");
//         Serial.println(decimalValue);
//         Serial.print("Box Position: ");
//         Serial.println(box_position);
//     }

    // void hidden_task(){
    //     move_till_junction(3000);
    //     turn(LEFT);
    //     move(300);
    //     move(70);
    //     mechanisms.lower();
    //     mechanisms.grab();
    //     mechanisms.lift();
    //     turn(LEFT);
    //     turn(LEFT);
    //     move_till_junction(3000);
    //     turn(LEFT);
    //     move_till_junction(3000);
    //     turn(LEFT);
    //     move(300);
    //     mechanisms.lower();
    //     mechanisms.lift();
    //     turn(RIGHT);
    //     turn(RIGHT);
    //     move_till_junction(3000);
    //     turn(RIGHT);
    //     move_till_junction(3000);
    //     move(100);
    //     mechanisms.lower();
    //     mechanisms.release();
    //     move(-100);
    //     mechanisms.grab();
    //     move(300);
    //     move(-50);
    //     turn(LEFT);
    //     turn(LEFT);
    //     move_till_junction(3000);
    //     turn(RIGHT);


    // }

    //     void follow_dashed_line(){

    // }