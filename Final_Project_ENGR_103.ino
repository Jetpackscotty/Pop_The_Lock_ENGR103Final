// Call libraries used for this program (iostream and Adafruit Circuit Playground)
#include <iostream>
#include <Adafruit_CircuitPlayground.h>

// Defines int variables used to store sound sensor output and if mute is enabled or disabled
int micLevel = 0;
int mute = 1;

// Defines int variable that is used to select which loop segment the program is running in
int correct = 5;

// Defines int varibles used for defining the players and targets position
int currentpos = 0;
int target = random(1,9);

// Defines int variable for length between each time the player moves
int levelDelay = 1000;

// Defines volatile bool variables used for flags in interrupts
volatile bool flagSS = 0;
volatile bool flagLB = 0;
volatile bool flagRB = 0;

// Defines int variables used to define the player's color
int redP = 0;
int greenP = 0;
int blueP = 255;

// Defines variables used for the MIDI function and sound outputs
float midi[127];
int A_four = 440;

// Array that defines that notes and duration of each in the bag song, used when a player loses
int badSong[6][2] = {
    {60, 600},
    {60, 100},
    {59, 100},
    {59, 100},
    {58, 100},
    {57, 500}
};

// Array that defines that notes and duration of each in the win song, used when a player wins
int winSong[6][2] = {
    {56, 100},
    {72, 100},
    {59, 100},
    {59, 100},
    {62, 100},
    {72, 100}
};


void setup() {

// Begins output on the serial port and inputs/ outputs from the Circuit Playground board
Serial.begin(9600);
CircuitPlayground.begin();

// Attach interrupts to the left and right buttons when they are rising, and to the slide switch when the voltage changes and sends them to their respective function
attachInterrupt(5, irpLeftButton, RISING);
attachInterrupt(4, irpRightButton, RISING);
attachInterrupt(7, irpSwitch, CHANGE);

// Begins the MIDI function to be used for sounds and music
generateMIDI();

}


void loop() {

delay(100);
// Starting Point
// Checks sound sensor and defines the value as micLevel, and printing it to the Serial Port
micLevel = CircuitPlayground.mic.soundPressureLevel(10);
Serial.println(micLevel);
// Checks to see if the value of micLevel is above the starting threshold, if so it will change the section of the program is running in, clear the neopixels, and wait a short delay
if (micLevel > 80) {
    correct = 0;
    CircuitPlayground.clearPixels();
    delay(1000);
}

// Main Portion
if (correct == 0) { 

    // Runs a for loop in which a single neopixel in position i will clight up to the color defined by the player color variables and setting the color of the target neo pixel to white
    // The for loop will define the value of i as the current position, and will check if mute has been enabled, if not then a tone will play each time the player moves a pixel
    for (int i=0; i < 10; ++i) {
        currentpos = i;
        CircuitPlayground.setPixelColor(target, 255, 255, 255);
        CircuitPlayground.setPixelColor(i, redP, greenP, blueP);
        if (mute == 1) {
          CircuitPlayground.playTone(midi[65], 50);
        }
        // The program will delay the value of levelDelay in ms, after which clearing the neopixels for the next loop throguh the for loop
        delay(levelDelay);
        CircuitPlayground.clearPixels();

        // When the left button is pressed
        if (flagLB == 1) {
            // The program check if the value of currentpos is equal to that of target
            // If Yes, then levelDelay will be shortened by 100 ms and all neopixels will turn green indicating that the player completed the stage
            // The target will be randomly changed and i will be reset to 0
          if (currentpos == target) {
              levelDelay = levelDelay - 100;
              for (int i=0; i<10; ++i) {
                  CircuitPlayground.setPixelColor(i, 0, 255, 0);
                  delay(50);
              }
              target = random(1,9);
              i = 0;
              // Checks to see if the levelDelay is less than 500ms, if so the program will break from the main fucntion, print win to the Serial Monitor, and move to the win final
              if (levelDelay <= 500) {
                correct = 2;
                Serial.print("WIN");
                break;
              }
              // Resets the flag back to 0 for the next stage
              flagLB = 0;
              // If No, then the program will break out of the main function and move to the lose final
          } else {
              correct = 1;
              break;
        }
    }
    }
}

// Lose Final
// The program will change all of NeoPixels to red and play the notes defined by the array badSong to play a bad song
if (correct == 1) {
  for (int i=0; i<10; ++i) {
    CircuitPlayground.setPixelColor(i, 255, 0, 0);
    delay(50);
  }
  if (mute == 1) {
    for(int i = 0; i < sizeof(badSong) / sizeof(badSong[0]); i++) {
      CircuitPlayground.playTone(midi[badSong[i][0]], badSong[i][1]);
      delay(1);
  }
}
}

// Win Final
// If the win condition is met, the program will change the color of each neopixel to a random color will playing the notes defined by the array win song to play a win song
if (correct == 2) {
  for (int i=0; i<10; ++i) {
    redP=random(255);
    greenP=random(255);
    blueP=random(255);
    CircuitPlayground.setPixelColor(i, redP, greenP, blueP);
    delay(50);
  }
  if (mute == 1) {
    for(int i = 0; i < sizeof(winSong) / sizeof(winSong[0]); i++) {
      CircuitPlayground.playTone(midi[winSong[i][0]], winSong[i][1]);
      delay(1);
    }
  }
}

    // In order to restart the game, a one must press the reset button on the Circuit Playground Board
}

// Defines the intterupt function for the left button press
void irpLeftButton() {
    flagLB = 1;
    delay(50);
}
// Defines the intterupt function for the right button pressed to randomly change the color of the player's LED
void irpRightButton() {
    redP = random(255);
    greenP = random(255);
    blueP = random(255);
    delay(50);
}
// Defines the intterupt function for the switch changing in order to switch if mute in enabled or disabled
void irpSwitch() {
    mute = mute * -1;
    delay(50);
}


// MIDI function that is used to create the sound and music used throguhout the program
void generateMIDI() {
  for (int x = 0; x < 127; ++x)
  {
    midi[x] = (A_four / 32.0) * pow(2.0, ((x - 9.0) / 12.0));
    Serial.println(midi[x]);
  }
}
