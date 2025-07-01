/*last updated on 4/4/2025 4:55PM by Nilakna*/

#ifndef RASPI_H
#define RASPI_H

class Raspi;
extern Raspi raspi; // Declare the raspi object

class Raspi
{
public:
    void takePotato(); // Ask Raspberry Pi to take the potato
    bool isRedGood(); // read the april tag and return true if red is good, return false is blue is good
    bool isBoxRed(); // find the box colout=r. return true if red, false if blue
    void openGate(float GOODORBAD); // Open the gate
    
    bool detectDryPot(); // Detect dry potato. return true if dry, false if wet
    void takeWater(); // Ask Raspberry Pi to take water
    void waterPot(); // Ask Raspberry Pi to water the potato
    void playStarman(); // Play Starman song

    void ledOn(); // Turn on the LED
    void ledOff(); // Turn off the LED

    int boxColumnColors(bool goodRed); // Get the box colors of the column
    int rightBoxColumnColors(bool goodRed); // turn camera to right and Get the box colors of the column
    void takeBox(); // Ask Raspberry Pi to take the box
};

#endif // RASPI_H