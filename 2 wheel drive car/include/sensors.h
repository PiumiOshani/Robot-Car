#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <ADS1X15.h> // Include the ADS1X15 library
#include <Adafruit_VL53L0X.h>
#include "config.h"
#include "Adafruit_TCS34725.h"
#include "mechanisms.h"

ADS1115 ads1(0x48);
ADS1115 ads2(0x49);

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_600MS, TCS34725_GAIN_1X); //Device ID 0x12

Adafruit_VL53L0X ToF_bottom = Adafruit_VL53L0X(); // Sensor at default address (0x29)
Adafruit_VL53L0X ToF_top = Adafruit_VL53L0X();     // Sensor to readdress(0x30)


class Sensors;

extern Sensors sensors;


// Enum for different steering states
enum
{
    STEER_NORMAL,
    STEERING_OFF,
};
enum {
    LINE,
    LEFT_LINE,
    RIGHT_LINE,
    CROSS_OR_T,
    NO_LINE,
};
enum {
    RED,
    BLUE,
    BLACK,
    WHITE,
    UNKNOWN,
};
enum {
    CM5,
    CM10,
    CM15,
    INVALID,
};

class Sensors
{
public:
    // ADS1115 objects for reading ADC values
    // ADS1115 ads1;
    // ADS1115 ads2;
    volatile int16_t adcValues[NUM_SENSORS]={0,0,0,0,0,0,0,0}; // To store ADC readings
    int16_t minValues[NUM_SENSORS];          // To store minimum values
    int16_t maxValues[NUM_SENSORS];          // To store maximum values
    int16_t mappedValues[NUM_SENSORS];       // To store mapped ADC values
    bool sensor_on_line[NUM_SENSORS] = {false,false,false,false,false,false,false,false};
    bool black_surface = true;
    volatile int line_state;
    bool calibrated = false;
    bool color_sensor_available = false;
    int color_sensor_token = 0;
    int last_junction = 0;
    bool left_pin_state;
    bool right_pin_state;
    int box_height;
    uint8_t g_steering_mode = STEER_NORMAL;
    volatile float steeringKp = STR_KP;
    volatile float steeringKd = STR_KD;
    volatile int prominent_color = UNKNOWN;
    volatile float top_ToF_reading = 0;
    volatile float bottom_ToF_reading = 0;
    volatile bool object_infront_top_ToF = false;
    volatile bool object_infront_bottom_ToF = false;


    float all_IR_readings[10] = {0,0,0,0,0,0,0,0,0,0}; //values from left sensors to right sensors

    void begin()
    {   
        Serial.println("Initializing ADC  :");
        begin_ADC();
        //begin_color_sensor();
        //begin_ToF();
        Serial.println("ADC Initialized");
        pinMode(BUTTON_PIN, INPUT);
        attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPressISR, CHANGE);
        pinMode(LED_PIN, OUTPUT);
        pinMode(LEFT_LINE_PIN, INPUT);
        pinMode(RIGHT_LINE_PIN, INPUT);
        pinMode(POTATO_IR_PIN,INPUT);
    }

        float get_steering_feedback()
    {
        return m_steering_adjustment;
    };

    float get_cross_track_error()
    {
        return m_cross_track_error;
    };

    void update()
    {
        readSensors();
        //ToF_measure();
        if(calibrated){
            map_sensors();
        }

        left_pin_state = !digitalRead(LEFT_LINE_PIN);
        right_pin_state = !digitalRead(RIGHT_LINE_PIN);
        ///
        for (int i=0;i<NUM_SENSORS;i++){
            all_IR_readings[i+1] = adcValues[i];
        }
        all_IR_readings[0]= 100*left_pin_state;
        all_IR_readings[9]= 100*right_pin_state;
        ///

        float error=0;
        if (g_steering_mode == STEER_NORMAL){
            error = line_error(black_surface);
        }
        m_cross_track_error = error;
        calculate_steering_adjustment();
        //Serial.println(adcValues[0]);
    }


    // Initialize the ADS1115 sensors
    void begin_ADC()
    {   
        // Initialize the first ADS1115 with I2C address 0x48
        ads1.begin();

        ads1.setGain(0);
    

        // Initialize the second ADS1115 with I2C address 0x49
        ads2.begin();
        ads2.setGain(0);

        // Set the data rate for faster conversions (use the correct constant from the library)
        ads1.setDataRate(7); // Set to 1600 SPS (samples per second)
        ads2.setDataRate(7); // Set to 1600 SPS (samples per second)
    }

    // Function to read values from all ADS1115 channels
    void readSensors()
    {
        // Reading 4 channels from the first ADS1115 (0x48)    (The array is connected in reverse direction)
        adcValues[7] = ads1.readADC(0); // Channel 0
        adcValues[6] = ads1.readADC(1); // Channel 1
        adcValues[5] = ads1.readADC(2); // Channel 2
        adcValues[4] = ads1.readADC(3); // Channel 3

        // Reading 4 channels from the second ADS1115 (0x49)
        adcValues[3] = (ads2.readADC(1)+ads1.readADC(3))/2; // Channel 0
        adcValues[2] = ads2.readADC(1); // Channel 1
        adcValues[1] = ads2.readADC(2); // Channel 2
        adcValues[0] = ads2.readADC(3); // Channel 3




        // Serial.print("  A0  ");
        // Serial.print(adcValues[0]);
        // Serial.print("  A1  ");
        // Serial.print(adcValues[1]);
        // Serial.print("  A2  ");
        // Serial.print(adcValues[2]);
        // Serial.print("  A3  ");
        // Serial.print(adcValues[3]);
        // Serial.print("  A4  ");
        // Serial.print(adcValues[4]);
        // Serial.print("  A5  ");
        // Serial.print(adcValues[5]);
        // Serial.print("  A6  ");
        // Serial.print(adcValues[6]);
        // Serial.print("  A7  ");
        // Serial.println(adcValues[7]);
        
    }
    static void handleButtonPressISR(){
        sensors.handleButtonPress();
    }
    void handleButtonPress(){
        button_pressed = true;
    }
    void reset_button(){  // Call this fuction whenever using the button functionality before calling any other button functions
        button_pressed = false;
    }
    bool is_button_pressed(){
        bool button_state;
        noInterrupts();
        button_state = button_pressed;
        interrupts();
        return button_state;
    }

    void led_indicator(bool state){
        if (state){
            digitalWrite(LED_PIN, HIGH);
        }
        else{
            digitalWrite(LED_PIN, LOW);
        }
    }
    void wait_till_button(){
        reset_button();
        while(not is_button_pressed()){
            led_indicator(1);
            delay(250);
            led_indicator(0);
            delay(250);

        }

    }

    bool is_wall_present(){
        return true;

    }

    bool is_potato_present(){
        digitalRead(POTATO_IR_PIN);
        return true;
    }

    float calculate_steering_adjustment()
    {
        // always calculate the adjustment for testing. It may not get used.
        float pTerm = steeringKp * m_cross_track_error;
        float dTerm = steeringKd * (m_cross_track_error - last_steering_error);
        float adjustment = (pTerm + dTerm) * encoders.loopTime_s();

        //Serial.print("   adjustment  ");
        //Serial.print(adjustment);

        adjustment = constrain(adjustment, -STEERING_ADJUST_LIMIT, STEERING_ADJUST_LIMIT);
        last_steering_error = m_cross_track_error;
        m_steering_adjustment = adjustment;
        return adjustment;
    }
        void set_steering_mode(uint8_t mode)
    {
        last_steering_error = m_cross_track_error;
        m_steering_adjustment = 0;
        g_steering_mode = mode;
    }

    void map_sensors(){



        // Map the raw ADC readings using the min and max values from calibration
        //Serial.print(left_pin_state);

        for (int i = 0; i < NUM_SENSORS; i++)
        {
            adcValues[i] = map(adcValues[i], minValues[i], maxValues[i], 0, 100); // Mapping to a range of 0-100
            adcValues[i] = constrain(adcValues[i], 0, 100);
        }

        float alpha=0.5; //complementary filter for erroneous sensor(2nd one)
        adcValues[1]=(1-alpha)*((adcValues[0]+adcValues[2])/2)+(alpha)*(adcValues[1]);

        
        for (int i = 0; i < NUM_SENSORS; i++)
        {
            //adcValues[i] = map(adcValues[i], minValues[i], maxValues[i], 0, 100); // Mapping to a range of 0-100
            //Serial.print(i);
            //Serial.print(")  ");
            //Serial.print(adcValues[i]);
            //Serial.print("   ");
            if (!black_surface){ //detect white line in black background
                if (adcValues[i]>SENSOR_THRESHOLD){  //include a code to handle the inverting of colors here
                    sensor_on_line[i] = true; 
                }
                else{
                    sensor_on_line[i] = false;
                }
            }
            else {
                if (adcValues[i]<SENSOR_THRESHOLD){
                    sensor_on_line[i] = true;
                }
                else{
                    sensor_on_line[i] = false;
                }
            }
            //Serial.print(sensor_on_line[i]);
        }
        //Serial.print("     ");
        for (int i = 0; i < NUM_SENSORS; i++)
        {
            //Serial.print(adcValues[i]);
            //Serial.print(",");
        }
       //Serial.print(right_pin_state);

        //line state detection
        left_state = true;
        right_state = true;
        no_line = true; 
        int on_line_count =0;
        for (int i = 0; i < NUM_SENSORS; i++)         //detect for a line
        {
            if (sensor_on_line[i]==true){
                //no_line = false;
                on_line_count +=1;
            }
        }
        if (on_line_count>3){
            no_line = false;
        }
        


                                      //Change this accordingly to avoid detecting the adacent colour lines
        for (int i = 0; i < NUM_SENSORS/4; i++)         //detect for a line in left side of the line
        {
            if (sensor_on_line[i]==false){
                left_state = false;
            }
        }
        for (int i = 3*NUM_SENSORS/4; i < NUM_SENSORS; i++)         //detect for a line in right side of the line
        {
            if (sensor_on_line[i]==false){
                right_state = false;
            }
        }


        if (no_line == true){
            line_state = NO_LINE;
            //Serial.println("NO_LINE");
        }
        else if (left_state == true and right_state==true and on_line_count >= NUM_SENSORS/2 and left_pin_state==true and right_pin_state == true){
            line_state = CROSS_OR_T;
            //led_indicator(true);
            //Serial.println("CROSS_OR_T");
        }
        else if (left_state == true and on_line_count>=((NUM_SENSORS/2)) and left_pin_state==true){
            line_state = LEFT_LINE;
            //Serial.println("LEFT_LINE");
        }
        else if (right_state == true and on_line_count>=((NUM_SENSORS/2)) and right_pin_state == true){
            line_state = RIGHT_LINE;
            //Serial.println("RIGHT_LINE");
        }
        else //if (left_state == false and right_state==false)
        {
           line_state = LINE;
           //Serial.println("LINE");
        }

        
    }

    // Calibrate function to find minimum and maximum values for each sensor channel
    void calibrate()
    {   
        wait_till_button();
        led_indicator(1);
        Serial.println("Callibration Started");
        // Initialize min and max arrays
        for (int i = 0; i < NUM_SENSORS; i++)
        {
            minValues[i] = 32767;  // Start with a high value for minimum
            maxValues[i] = -32768; // Start with a low value for maximum
        }

        // Take multiple readings to find min and max
        int t = millis();
        while (millis() < t + 5000)
        {
            readSensors(); // Read current values

            // Check and update min/max values for each channel
            for (int i = 0; i < NUM_SENSORS; i++)
            {
                noInterrupts();
                if (adcValues[i] < minValues[i])
                    minValues[i] = adcValues[i]; // Update min value
                if (adcValues[i] > maxValues[i])
                    maxValues[i] = adcValues[i]; // Update max value
                interrupts();
            }
            delay(15);
        }
        calibrated = true;
        Serial.println("Callibration Finished");
        wait_till_button();
        led_indicator(0);
    }

    // Function to compute the line error (position of the line)
    float line_error(bool black_background)
    {
        unsigned long weightedSum = 0;
        unsigned int totalValue = 0;

        for (int i = 0; i < NUM_SENSORS; i++)
        {
        float sensorValue = adcValues[i];

        // Adjust sensor interpretation based on the mode
        if (black_background) {
            // Invert the sensor value for white-on-black mode
            sensorValue = 100 - sensorValue;
        }
            weightedSum += (long)sensorValue * i * 1000;
            totalValue += sensorValue;
        }

        // Handle case where no line is detected
        if (totalValue == 0)
        {
            return 0.0; // Return center (3500) if no line detected
        }
        if (black_background) {
            // Invert the error value for white-on-black mode
            return (ERROR_POLARITY)*((weightedSum / totalValue)-3500.0)/100.0;//  error -35<error<35
        }
        else {
        return (ERROR_POLARITY)*((weightedSum / totalValue)-3500.0)/100.0;//  error -35<error<35
        }
    }
    void begin_color_sensor() {
            // Initialize the TCS34725 sensor
            if (tcs.begin()) {
                Serial.println("TCS34725 found");
                color_sensor_available = true;
            } else {
                Serial.println("No TCS34725 found ... check your connections");
                wait_till_button(); // halt program
            }
            }

    int readProminentColor(){
        updateProminentColor();
        return prominent_color;
    }
    String getProminentColorinword(){
        if (prominent_color == RED){
            return "RED";
            }
        else if (prominent_color == BLUE){
            return "BLUE";
            }
        else if (prominent_color == BLACK){
            return "BLACK";
            }
        else if (prominent_color == WHITE){
            return "WHITE";
            }
        else {
            return "UNKNOWN";
            }
    }


    void updateProminentColor() {
        //color_sensor_token ++;
        //color_sensor_token = color_sensor_token % 5;
        if (color_sensor_available ){//&& color_sensor_token==1){
        // Variables to store color data
        uint16_t red, green, blue, clear;

        // Get raw data from the sensor
        tcs.getRawData(&red, &green, &blue, &clear);

        // Calculate the RGB values scaled to 0-255
        float r = red / (float)clear * 255.0;
        float g = green / (float)clear * 255.0;
        float b = blue / (float)clear * 255.0;

        // Serial.print(" r- ");
        // Serial.print(r);
        // Serial.print(" g- ");
        // Serial.print(g);
        // Serial.print(" b- ");
        // Serial.print(b);

        float avg = (r+b+g)/3;

        // Determine the prominent color
        if (clear < 400) {
            prominent_color = BLACK; // Low light indicates black
        }
        else if (abs(r-avg) < 10 && abs(g-avg) < 10 && abs(b-avg) < 10) {
            prominent_color = WHITE; // High values across RGB indicate white
        }
        else if (r > b && r > g) {
            prominent_color =RED;
        } else if (b > r && b > g) {
            prominent_color = BLUE;
        } else if (r > 200 && g > 200 && b > 200) {
            prominent_color = HIGH;; // High values across RGB indicate white
        } else {
            prominent_color = UNKNOWN; // Default case if no prominent color
        }
        }
    }
    void begin_ToF(){

        if (!ToF_bottom.begin()) {
                 Serial.println("Failed to find VL53L0X sensor! Check wiring.");
                    //while (1);
                 }

        Serial.println("VL53L0X sensor initialized.");
        //         // Setup XSHUT pin as output
        // pinMode(XSHUT_PIN, OUTPUT);

        // // Step 1: Initialize the default sensor
        // digitalWrite(XSHUT_PIN, LOW); // Disable the second sensor
        // delay(10);                    // Ensure it's off
        // if (!ToF_bottom.begin(0x29)) { // Default address is 0x29
        //     Serial.println("Failed to initialize sensor at default address (0x29)");
        //     wait_till_button();
        // }
        // Serial.println("VL53L0X at 0x29 initialized");

        // // Step 2: Readdress the second sensor
        // digitalWrite(XSHUT_PIN, HIGH); // Enable the second sensor
        // delay(10);                     // Allow time for initialization
        // if (!ToF_top.begin(0x30)) {  // Assign a new address (0x30)
        //     Serial.println("Failed to initialize second sensor at new address (0x30)");
        //     wait_till_button();
        // }
        // Serial.println("VL53L0X at 0x30 initialized");
    
    }
    void ToF_measure(){
          VL53L0X_RangingMeasurementData_t measure;

  // Perform a ranging measurement
            ToF_bottom.rangingTest(&measure, false); // Pass 'true' to get debug data printed to Serial

            // Check if the measurement is valid
            if (measure.RangeStatus != 4) {  // 4 means out of range
                Serial.print("Distance: ");
                Serial.print(measure.RangeMilliMeter);
                Serial.println(" mm");
            } else {
                Serial.println("Out of range");
            }
            bottom_ToF_reading = measure.RangeMilliMeter;
            if (bottom_ToF_reading<=OBJECT_DETECT_RANGE){
                object_infront_bottom_ToF = true;
            }
            else{
                object_infront_bottom_ToF = false;
                
            }
        // VL53L0X_RangingMeasurementData_t measure;

        // // Read from the default sensor
        // // ToF_bottom.rangingTest(&measure, false);
        // // if (measure.RangeStatus != 4) {
        // //     //Serial.print("Sensor at 0x29: ");
        // //     //Serial.print(measure.RangeMilliMeter);
        // //     //Serial.println(" mm");
        // //     bottom_ToF_reading = measure.RangeMilliMeter;
        // //     if (bottom_ToF_reading<=OBJECT_DETECT_RANGE){
        // //         object_infront_bottom_ToF = true;
        // //     }
        // //     else{
        // //         object_infront_bottom_ToF = false;
                
        // //     }
        // // } else {
        // //     object_infront_bottom_ToF = false;
        // //     //Serial.println("Sensor at 0x29: Out of range");
        // // }

        // // Read from the readdressed sensor
        // ToF_top.rangingTest(&measure, false);
        // if (measure.RangeStatus != 4) {
        //     // Serial.print("Sensor at 0x30: ");
        //     // Serial.print(measure.RangeMilliMeter);
        //     // Serial.println(" mm");
        //     top_ToF_reading = measure.RangeMilliMeter;
        //                 if (top_ToF_reading<=OBJECT_DETECT_RANGE){
        //         object_infront_top_ToF = true;
        //     }
        //     else{
        //         object_infront_top_ToF = false;
                
        //     }
        // } else {
        //     object_infront_top_ToF = false;
        //     //Serial.println("Sensor at 0x30: Out of range");
        // }

        // //delay(500); // Wait before next reading

    }
    void measure_height(){
        bool base = false;
        bool mid = false;
        bool top = false;
        mechanisms.lower();
        ToF_measure();
            if (object_infront_bottom_ToF){
                base = true;
            }
            if (object_infront_top_ToF){
                mid = true;
            }
        mechanisms.lift();
            if (object_infront_bottom_ToF){
                mid = true;
            }
            if (object_infront_top_ToF){
                top = true;
            }
        if (base && mid && (!top)){
            box_height = CM10;
        }
        else if (base && (!mid) && (!top)){
            box_height = CM5;
        }
        else if (base && mid && top){
            box_height = CM15;
        }
        else {
            box_height = INVALID;
        }

}
private:
    // variables for steering
    float last_steering_error = 0;
    volatile float m_cross_track_error;
    volatile float m_steering_adjustment;
    volatile bool button_pressed;
    bool left_state;
    bool right_state;
    bool no_line;
};
