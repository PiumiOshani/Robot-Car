#ifndef TASKS_H
#define TASKS_H

// #include <Arduino.h>
#include "robot.h"
#include "encoders.h"
#include "raspi.h"
#include <vector>

# define GOOD 1
# define BAD 0

class Tasks; // Forward declaration of the tasks class

extern Tasks tasks; // Declare the tasks object

class Tasks
{
public:
    // robotigation robot; // robotigation object
    // encodersoders encoders; // encodersoders object
    // raspi raspi; // Raspberry Pi object
    // tasks(); // Constructor with robotigation object;
    bool task1(); // IMPLEMENTED
    bool task2(); // IMPLEMENTED
    bool task3(); // IMPLEMENTED
    bool task4();
    bool task5();
    bool task6(); // IMPLEMENTED

    bool task2nodist(); // just in case...

private:
    // task 2
    bool task2_done = false; // Flag to indicate if task 2 is done
    bool goodRed = false; // Flag to indicate if the red box is good

};

// FINAL FINAL SIDE OF THE CAM - RIGHT SIDE

bool Tasks::task1()
{   
    int potatoJuncs = 0;
    bool potatoFound = false;

    // iterate through all 5 rows
    for (int junc = 0; junc < 5; junc++) {
        // reset values
        potatoJuncs = 0;
        potatoFound = false;

        // move to the next row
        robot.move_till_junction(350); // Move until a junction is foun
        robot.move_till_junction(350); // Move until a junction is found
        robot.turn(RIGHT); // Turn 90 degrees clockwise
        
        // move to the potato
        while(!potatoFound || (potatoJuncs < 3)) { // Continue moving until a potato is found or 2 junctions are crossed
            // move till potatoFound 
            potatoFound = robot.move_till_potato(350); // Move until a potato is detected. false if junction.
            if (!potatoFound) {
                potatoJuncs++; // Increment the junction count if a junction is found
            }
        } 

        // take the potato
        raspi.takePotato(); // Ask the Raspberry Pi to take the potato
        
        // go to the next junction and face
        robot.turn(ABOUTTURN); // Turn 90 degrees anticlockwise
        for (int i = 0; i < potatoJuncs+1; i++) {
            robot.move_till_junction(350); // Move until a junction is found
        }
        robot.turn(RIGHT); // Move straight for 150 mm
    }
    
    // go to the start of task 2
    robot.move_till_junction(350); // Move until a junction is found
    return true; // Return true to indicate task 1 is done
}

bool Tasks::task2()
{
    bool wallDone = false;
    float side = 1; // right line (should turn left)
    
    float startDist = encoders.robotDistance(); // Get the initial distance from the encodersoders
    float turnDist = 0; // Initialize the end distance
    float tempDist; // Initialize the temporary distance
    float task2Dist = 0; // Initialize the task 2 distance

    robot.move_straight(150); // Move straight for 150 mm
    robot.turn(RIGHT); // Turn 90 degrees clockwise

    // WITH DISTANCE MEASUREMENT BUT CANNOT CHECK TASK 2 END =(. NOW CAN. YAY.
    while (!wallDone) {
        
        // MOVE TILL THE RIGHT WALL
        task2Dist = encoders.robotDistance() - startDist - turnDist; // Calculate the distance travelled

        wallDone = robot.move_till_wall_task2(task2Dist); // Move straight until wall or line is found
        if (wallDone) {
            break;
        }
        


        // CHANGE TO LEFT SIDE

        // if (foundWall) {
        // turn left
        robot.turn(90); // Turn 90 degrees anticlockwise 
        
        //  move forward
        tempDist = encoders.robotDistance();
        robot.move_straight(300); // Move straight for 150 mm
        turnDist = turnDist + (encoders.robotDistance() - tempDist);
        
        //turn right to original direction
        robot.turn(RIGHT); // Turn 90 degrees clockwise
        

        

        // MOVE TILL THE LEFT WALL
        task2Dist = encoders.robotDistance() - startDist - turnDist; // Calculate the distance travelled

        wallDone = robot.move_till_wall_task2(task2Dist); // Move straight until wall or line is found
        if (wallDone) {
            side = -1; // Set wall to true
            break; // Exit the loop if task 2 is done
        }



        // CHANGE TO RIGHT SIDE
        // turn right
        robot.turn(RIGHT); // Turn 90 degrees clockwise
        
        // move forward
        tempDist = encoders.robotDistance();
        robot.move_straight(300); // Move straight for 150 mm
        turnDist = turnDist + encoders.robotDistance() - tempDist;
        
        // turn left to original direction
        robot.turn(LEFT); // Turn 90 degrees clockwise
        // }
    }

    // LETS GO RAMP!!!!

    // turn right if on left line.
    // robot.turn((float)90*side); // Turn 90 degrees clockwise if on left line
    if (side==1){
        robot.turn(LEFT); // Turn 90 degrees clockwise if on left line
    } else {
        robot.turn(RIGHT); // Turn 90 degrees clockwise if on right line
    }
    robot.move_straight(150); // Move straight for 150 mm
    // robot.turn((float)(-90)*side);
    if (side==1){
        robot.turn(RIGHT); // Turn 90 degrees clockwise if on left line
    } else {
        robot.turn(LEFT); // Turn 90 degrees clockwise if on right line
    }
    // robot.move_till_line(); 
    robot.move_till_wall(1500); // Move straight until wall is found
    task2_done = true; // Set task 2 done to true

    return task2_done; // Return the task 2 status
}


// bool tasks::task2nodist()
// {
//     // Task 2 implementation without distance measurement
//     robot.move_straight(150); // Move straight for 150 mm
//     robot.turn(-90); // Turn 90 degrees clockwise
//     bool foundLine = false; // Initialize foundLine to false

//     while (!task2_done) {
//         // CANNOT FIND TASK 2 DONEEEEEEEEEEEEEEEEEEEEEE
//         foundLine = robot.move_till_wallorLine(); // Move straight until wall or line is found
//         if (!foundLine) {
//             // turn left
//             robot.turn(90); // Turn 90 degrees anticlockwise 
//             //  move forward
//             robot.move_straight(300); // Move straight for 150 mm
//             //turn right to original direction
//             robot.turn(-90); // Turn 90 degrees clockwise
//             // move again till left wall found
//             foundLine = robot.move_till_wallorLine();
//             if (!foundLine) {
//                 task2_done = true; // Set task 2 done to true
//                 break; // Exit the loop if task 2 is done
//             }
//             // turn right
//             robot.turn(-90); // Turn 90 degrees clockwise
//             // move forward
//             robot.move_straight(300); // Move straight for 150 mm
//             // turn left to original direction
//             robot.turn(90); // Turn 90 degrees clockwise
//         }
//         if (!foundLine) {
//             task2_done = true; // Set task 2 done to true
//             break; // Exit the loop if task 2 is done
//         }
//     }
//     return task2_done; // Return the task 2 status
// }


bool Tasks::task3()
{   
    // READ APRILTAG
    // robot start facing the wall
    robot.turn(RIGHT);
    robot.move_till_wall(2000);
    robot.turn(RIGHT); // Turn 90 degrees clockwise
    robot.move_till_line(1500);
    robot.turn(LEFT);
    robot.move_till_junction(350); // align also
    robot.move_straight(-150);
    // robot.turn(RIGHT);
    goodRed = raspi.isRedGood(); // ask raspberry to find tag


    // BASKETING POTATOES 

    // go to the basket and find colour
    robot.move_straight(-300); // reverse to the first basket
    // robot.turn(-90);
    // robot.move_till_line(500); // align also
    // // robot.move_straight(300);
    // robot.move_straight(-150);
    // robot.turn(90); // turn towards the basket to read 
    bool redBox = raspi.isBoxRed(); // ask raspberry to find tag

    // turn rear to put the potatoes
    robot.turn(LEFT);
    robot.move_straight(-50);
    
    bool openGood = false;

    if ((goodRed && redBox)||(!goodRed && !redBox)) {
        raspi.openGate(GOOD); //ASK OSHANI ABOUT SERVO CONTROLLING
        openGood = true;
    } else {
        // goodRed && !redBox || !goodRed && redBox
        raspi.openGate(BAD);
    }

    // go to the next basket
    // robot.move_straight(50);
    // robot.turn(-90);
    // robot.move_till_line(1500); // and align
    // robot.move_straight(-150); // move towards the box
    // robot.turn(90); // turn rear towards the box
    // robot.move_straight(-50); // move a bit back to align with the box
    robot.move_till_junction(350); // Move until a junction is found
    robot.move_till_junction(350); // Move until a junction is found
    robot.move_till_junction(350); // Move until a junction is found
    robot.move_straight(-150); // move towards the box
    robot.turn(LEFT); // turn rear towards the box
    robot.move_straight(-50); // move a bit back to align with the box

    // put the other set of potatoes
    if (openGood) {
        raspi.openGate(BAD);
    } else {
        raspi.openGate(GOOD);
    }

    // go to the start of task4
    robot.move_straight(50);
    robot.turn(RIGHT);
    robot.move_till_line(350); // and align

    return true; // Return true to indicate task 3 is done
}

bool Tasks::task4()
{
    std::vector<std::vector<int>> boxColors = {{0,0,0}, {0,0,0}, {0,0,0}}; // Initialize the box array

    // FIND BOXES AND BOX COLORS
    

    // // ALONG TASK 3 LINE
    // // go infront of each column and find colours
    // boxColors[1] = raspi.boxColumnColors(); // red 1, blue -1, empty(white)0
    
    // robot.turn(90);
    // robot.move_till_junction(); // Move until a junction is found
    // robot.move_straight(-150);
    // robot.turn(-90); // Turn 90 degrees clockwise
    // boxColors[0] = raspi.boxColumnColors(goodRed);  // if goodRed then red=2 blue = 1 white 0.

    // robot.turn(-90);
    // robot.move_till_junction(); // Move until a junction is found
    // robot.move_straight(150);
    // robot.turn(90);
    // boxColors[2] = raspi.boxColumnColors(goodRed); // if goodRed then red=2 blue = 1 white 0.

    // ALONG TASK 4 LINE
    robot.turn(LEFT);
    robot.move_till_junction(800); // Move until a junction is found
    robot.turn(RIGHT); // Turn 90 degrees clockwise
    // correct this to read from left camera
    robot.move_straight(150);
    boxColors[0][0],boxColors[0][1],boxColors[0][2] = raspi.rightBoxColumnColors(goodRed); // red 1, blue -1, empty(white)0
    robot.move_straight(300);
    boxColors[1][0],boxColors[1][1],boxColors[1][2] = raspi.rightBoxColumnColors(goodRed); // red 1, blue -1, empty(white)0
    robot.move_straight(300);
    boxColors[2][0],boxColors[2][1],boxColors[2][2] = raspi.rightBoxColumnColors(goodRed); // red 1, blue -1, empty(white)0

    // FIND ThE BEST POSSIBLE PATH

    // GO TO THE GOOD BOX AND TAKE IT
    raspi.takeBox();
    return true; // Return true to indicate task 4 is done
}

bool Tasks::task5()
{
    // Task 5 implementation
    return true; // Return true to indicate task 5 is done
}

bool Tasks::task6()
{
    int dry = 0; // Initialize dry to false
    float startDist;
    float tempDist;
    int drypot;

    // iterate through all 3 boxes
    for (int pot = 0; pot < 3; pot++) {
        robot.move_till_potato(1000); // move untill a box is found
        robot.turn(LEFT); // turn to the left
        startDist = encoders.robotDistance(); // Get the initial distance from the encodersoders
        robot.move_till_line(1000);
        tempDist = encoders.robotDistance() - startDist; // Calculate the distance travelled
        dry = raspi.detectDryPot();
        if (dry) {
            raspi.ledOn(); // turn on the LED
            int drypot = pot; // Set the drypot to the current potato number
            // break; // if using break, dont use drypot
        }
        robot.turn(ABOUTTURN);
        robot.move(tempDist); // move back to the box
        robot.turn(LEFT); // turn to the left
    }

    // go to the well and take water
    robot.turn(ABOUTTURN);
    robot.move_till_potato(500); // move forward untill the water box
    robot.move_till_potato(500); // move forward untill the water box
    robot.turn(LEFT);
    robot.move_till_line(1000); // move forward untill the line
    raspi.takeWater(); // ask raspberry to take the water
    robot.move_straight(-50); // move back a bit
    robot.turn(ABOUTTURN);

    // water the dry potatoes
    robot.move_till_line(1000); // go infront of the middle potato
    robot.move_straight(-50); // move back a bit
    
    // // nice code but cannot use =(
    // robot.turn((float)(1-drypot)*90); //turn to the line of the drypot
    // robot.move_till_line(400);
    // robot.turn((float)(drypot-1)*90); // turn to the drypot
    // robot.move_till_line(400); // move forward untill the line

    if (drypot == 0) {
        robot.turn(LEFT); // turn to the left
        robot.move_till_line(400);
    } else if (drypot == 2) {
        robot.turn(RIGHT); // turn to the right
        robot.move_till_line(400);
    }
    if (drypot == 0) {
        robot.turn(RIGHT); // turn to the left
        robot.move_till_line(400);
    } else if (drypot == 2) {
        robot.turn(LEFT); // turn to the right
        robot.move_till_line(400);
    }
    raspi.waterPot(); // ask raspberry to water the potato

    // now tasks are finished
    raspi.ledOn(); 
    raspi.playStarman();

    // YAYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY!!!!!!!!!!!!!!!!!!!!
    return true; // Return true to indicate task 6 is done
}


#endif // TASKS_H


