#include <iostream>
#include <Adafruit_CircuitPlayground.h>

int micLevel = 0;
int mute = 0;

int correct = 5;
int currentpos = 0;
int target = random(1,9);

int levelDelay = 1000;

volatile bool flagSSL = 0;
volatile bool flagSSH = 0;
volatile bool flagLB = 0;
volatile bool flagRB = 0;

int redP = 0;
int greenP = 0;
int blueP = 255;

float midi[127];
int A_four = 440;

int badSong[6][2] = {
    {60, 600},
    {60, 100},
    {59, 100},
    {59, 100},
    {58, 100},
    {57, 500}
};

int winSong[6][2] = {
    {56, 100},
    {72, 100},
    {59, 100},
    {59, 100},
    {62, 100},
    {72, 100}
};


void setup() {

mute = CircuitPlayground.slideSwitch();
Serial.begin(9600);
CircuitPlayground.begin();
attachInterrupt(5, irpLeftButton, RISING);
attachInterrupt(4, irpRightButton, RISING);
attachInterrupt(7, irpSwitchH, RISING);
attachInterrupt(7, irpSwitchL, FALLING);
generateMIDI();

}


void loop() {

delay(100);
// Starting Point
micLevel = CircuitPlayground.mic.soundPressureLevel(10);
Serial.println(micLevel);
// Say Speak to Start
if (micLevel > 80) {
    correct = 0;
    CircuitPlayground.clearPixels();
    delay(1000);
}

// Main Portion
if (correct == 0) {
    
    if (flagSSH == 1) {
      mute = 1;
      flagSSH = 0;
    }    
    
    if (flagSSL == 1) {
      mute = 0;
      flagSSL = 0;
    }



    for (int i=0; i < 10; ++i) {
        currentpos = i;
        CircuitPlayground.setPixelColor(target, 255, 255, 255);
        CircuitPlayground.setPixelColor(i, redP, greenP, blueP);
        if (mute == 0) {
          CircuitPlayground.playTone(midi[65], 50);
        }
        delay(levelDelay);
        CircuitPlayground.clearPixels();

        if (flagLB == 1) {
          if (currentpos == target) {
              levelDelay = levelDelay - 100;
              for (int i=0; i<10; ++i) {
                  CircuitPlayground.setPixelColor(i, 0, 255, 0);
                  delay(50);
              }
              target = random(1,9);
              i = 0;
              if (levelDelay <= 500) {
                correct = 2;
                Serial.print("WIN");
                break;
              }
              flagLB = 0;
          } else {
              correct = 1;
              break;
        }
    }
    }
}

// Lose Final
if (correct == 1) {
  for (int i=0; i<10; ++i) {
    CircuitPlayground.setPixelColor(i, 255, 0, 0);
    delay(50);
  }
  if (mute == 0) {
    for(int i = 0; i < sizeof(badSong) / sizeof(badSong[0]); i++) {
      CircuitPlayground.playTone(midi[badSong[i][0]], badSong[i][1]);
      delay(1);
  }
}
}

// Win Final
if (correct == 2) {
  for (int i=0; i<10; ++i) {
    redP=random(255);
    greenP=random(255);
    blueP=random(255);
    CircuitPlayground.setPixelColor(i, redP, greenP, blueP);
    delay(50);
  }
  if (mute == 0) {
    for(int i = 0; i < sizeof(winSong) / sizeof(winSong[0]); i++) {
      CircuitPlayground.playTone(midi[winSong[i][0]], winSong[i][1]);
      delay(1);
    }
  }
}

}

void irpLeftButton() {
    flagLB = 1;
    delay(50);
}
void irpRightButton() {
    redP = random(255);
    greenP = random(255);
    blueP = random(255);
    delay(50);
}
void irpSwitchH() {
    flagSSH = 1;
    delay(50);
}
void irpSwitchL() {
    flagSSL = 1;
    delay(50);
}

// DO NOT EDIT!!!!!
void generateMIDI() {
  for (int x = 0; x < 127; ++x)
  {
    midi[x] = (A_four / 32.0) * pow(2.0, ((x - 9.0) / 12.0));
    Serial.println(midi[x]);
  }
}