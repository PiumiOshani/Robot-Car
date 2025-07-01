#include <Arduino.h>
#include "motors.h"
#include "Ticker.h"
#include "motion.h"
#include "robot.h"
#include "profile.h"
#include "sensors.h"
#include "Wire.h"
#include "communications.h"
#include "mechanisms.h"
#include "tasks.h"

// put function declarations here:
Motors motors;
Encoders encoders;
Ticker sendTicker;
Ticker controlTicker;
Robot robot;
Motion motion;
Profile forward;
Profile rotation;
Sensors sensors;
Communications communications;
Mechanisms mechanisms;
Tasks tasks;
bool pressed=false;


void setup() {
  motors.begin();
  encoders.begin();
  encoders.reset();
  Serial.begin(115200);
  Wire.begin();
  sensors.begin();
  communications.begin();
  mechanisms.begin();

  sensors.calibrate();

  controlTicker.attach(0.005,[](){
      encoders.update();
      motion.update();
      motors.update(motion.velocity(), motion.omega(), sensors.get_steering_feedback());

  });

  sendTicker.attach(0.02, [](){

      // encoders.update();
      // motion.update();
      // motors.update(motion.velocity(), motion.omega(), sensors.get_steering_feedback());
      //communications.send("HI THERE");
      //communications.send("IRSENSORS",sensors.all_IR_readings, NUM_SENSORS+2);
      communications.send_velocity();
      //communications.check(); 
      sensors.update();


      });
  


  //delay(5000);
  //encoders.update();
  //motors.set_right_motor_pwm(500);
  //motors.set_left_motor_pwm(500);
  // delay(1000);
  motors.enable_controllers();
 
  motors.omega = 0;
  motors.speed = 0;
}

void loop() {
  tasks.task1();
}
  // // sensors.set_steering_mode(STEERING_OFF);
  // // motors.disable_controllers();
  // // for (int i=-100;i<101;i++){

  // //   motors.set_left_motor_percentage(i);
  // //   motors.set_right_motor_percentage(0);
  // //   float avg_speed = 0;
  // //   for (int j=0;j<100; j++){
  // //     avg_speed += encoders.robot_speed();
  // //     delay(2);
  // //   }
  // //   avg_speed = (avg_speed/100)*2;
  // //   Serial.println(String(i) + " ,"+ String(avg_speed));
  // //   //communications.send(String(i * 100 / 1023) + "," + String(avg_speed));
  // // }
  // // while(1){}

  // motion.reset_drive_system();
  // robot.move_staright(0.1);
  // delay(1000);
  // // for (int i=0; i<10;i++){
  // //   robot.turn(360);
  // //   delay(1000);
  // //   communications.send("Distance ", encoders.robotAngle());
  // // }
  // Serial.println("Start");
  // //robot.move_till_potato(6000);
  // robot.move_till_junction(6000);
  // delay(2000);
  // robot.move_till_potato(6000);
  // delay(2000);
  //  robot.turn(RIGHT);
  //  delay(2000);
  // // robot.move_till_junction(6000);
  // // delay(2000);
  // // robot.move_till_junction(6000);
  // // delay(2000);
  // // robot.turn(LEFT);
  // // delay(2000);
  // // robot.move_till_junction(6000);
  // // delay(2000);
  // // robot.move_till_junction(6000);
  // // delay(2000);
  // // robot.turn(LEFT);
  // // delay(2000);
  // // robot.move_till_junction(6000);
  // // delay(2000);
  // // robot.move_till_junction(6000);
  // // delay(2000);
  // // robot.turn(RIGHT);
  // // delay(2000);
  // // robot.move_till_junction(6000);
  // // delay(2000);
  // // robot.move_till_junction(6000);
  // // delay(2000);
  // // robot.turn(RIGHT);
  // // delay(2000);
  // // robot.move_till_junction(6000);
  // // delay(2000);
  // // robot.move_till_junction(6000);
  // // delay(2000);
  // // robot.move_staright(150);
  // while (true){
  //     // robot.move_staright(315*2);
  //     // delay(1000);
  //     // robot.turn(-90);
  //     // delay(1000);
  //     // robot.move_staright(315*2);
  //     // delay(250);
  //     // robot.turn(-90);
  //     // delay(1000);

      

  //     }
  // sensors.led_indicator(1);
  // robot.scan_barcode();
  // sensors.led_indicator(1);
  // delay(3000);
  // sensors.led_indicator(0);
  // //robot.decode_barcode();
  // sensors.led_indicator(1);
  // delay(3000);
  // sensors.led_indicator(0);
  // robot.move_till_junction(3000);
  // robot.turn(RIGHT);


  


  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  

  // }



  




 


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // while (true){
  //   Serial.println(encoders.robotAngle());
  // }
  //encoders.update();
  //motors.update(0.0, 360.0, 0.0);
  //motors.set_right_motor_pwm(motors.right_feed_forward_percentage(60.0)*1023/100);
  // Serial.print("Speed ");
  // Serial.print(encoders.robot_speed());
  // Serial.print("  omega  ");
  // Serial.println(encoders.robot_omega());


  // float distance = encoders.robotDistance();
  // float travel = 1000;
  // while (encoders.robotDistance()<distance+travel){
  //     if (motors.speed<360){
  //         motors.speed ++;
  //     }
  //     delay(2);
  // }

  // float angle = encoders.robotAngle();
  // float sweep = 180;
  // while (encoders.robotAngle()<angle+sweep){
  //     if (motors.omega<115){
  //         motors.omega ++;
  //     }
  //     delay(2);
  // }
  // while (motors.omega>5){
  //     motors.omega --;
  //     delay(2);
  // }


  //motors.omega = 0;

  //delay(1000);

   
   //Serial.println("h");
  //delay(18);


  //  while (true){
  //     robot.move_staright(300);
  //     delay(250);
  //     robot.move_staright(-300);
  //     delay(250);
  //     robot.turn(-90);
  //     delay(250);
  //     robot.turn(90);
  //     delay(250);
  //     }
  // while(1){
  //    robot.move_till_junction_and_turn();
  //    delay(1000);
  // }
  // robot.move_till_junction_and_turn();
  // robot.scan_barcode();
  // delay(1000);
  // robot.move_till_junction_and_turn();
  // robot.move_till_junction(3000);
  // robot.turn(RIGHT);
  // robot.move_till_junction(3000);
  // robot.turn(LEFT);
  // robot.move_till_junction(3000);
  // robot.turn(RIGHT);
  // robot.move_till_junction(3000);
  // //robot.turn(LEFT);
  // robot.move_till_junction(3000);
  // robot.turn(RIGHT);
  // while (1){}

  // delay(5000);
  // sensors.led_indicator(1);
  // robot.scan_barcode();
  
  // while (true){
  //   //robot.move_staright(500);
  //   robot.turn(-77);
  //   motion.stop();
  //   delay(1000);



  //   robot.turn(-77);
  //   motion.stop();
  //   //delay(1000);

  //   //robot.move_staright(500);
  //   //delay(1000);

  //   robot.turn(75);
  //   motion.stop();
  //   //delay(1000);

  //   robot.turn(75);
  //   motion.stop();
  //   //delay(1000);

  //   //robot.move_staright(500);
  //   //delay(1000);
  // }
  // Serial.print("  Moving    ");

  // robot.move_till_junction(3000);
  // robot.turn(RIGHT);
  // robot.move_till_junction(3000);
  // robot.turn(LEFT);
  // robot.move_till_junction(3000);
  // robot.turn(RIGHT);
  // robot.move_till_junction(3000);
  // robot.turn(LEFT);
  // robot.move_till_junction(3000);
  // robot.turn(RIGHT);
  // while (1){}
  
  // while (true){
  //   robot.move_till_junction_and_turn();
  // }

  // while (true){
  //     robot.move(300);
  //     delay(250);
  //     robot.move(-300);
  //     delay(250);
  //     robot.turn(-90);
  //     delay(250);
  //     robot.turn(-90);
  //     delay(250);
  //     }

  //   robot.turn(90);
  //   delay(250);
  //   robot.move(300);
  //   delay(250);
  //   robot.turn(-90);
  //   delay(250);
  //   robot.move(300);
  //   delay(250);
  //   robot.move(-300);



