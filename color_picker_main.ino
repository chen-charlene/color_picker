#include "color_picker.h"
#include "config.h"
#include <vector>

//FSM variables
static uint16_t currColor;
static int timeStep, savedClock, numButtons;
static float potVal, currOpacity;
static uint16_t savedColors[10];

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

struct colorCalibration {
  unsigned int blackValue;
  unsigned int whiteValue;
};

// initiate three stuctures to hold calibration of Red, Green and LED's in TCS3472
static colorCalibration redCal, greenCal, blueCal;
int gammatable[256];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial);
  
  //initialize components
  initializeSystem();

  //initialize variables
  currColor = 65535;
  currOpacity = 1;
  timeStep = 50;
  savedClock = 0;
  numButtons = 0;
  potVal = getPotentiometerData();
  // savedColors[10] = {0,0,0,0,0,0,0,0,0,0};
}


void loop() {
 // uncomment for running
  static state CURRENT_STATE = sSCAN;
  CURRENT_STATE = updateFSM(CURRENT_STATE,millis());

  // uncomment for testing
  // testAllHardware(); 
  // testAllSoftware();
  // testAllMain();
  // petWDT();


  delay(100);
}

// updateFSM method that gets called in the loop and is used to transition between different states.
state updateFSM(state currState, long mils) {
  state nextState;
  switch(currState) {
    case sSCAN:
      if (mils - savedClock >= timeStep && numButtons == 0) {   // 1-2 scan to update color
        // Serial.println("1-2");
        savedClock = mils;
        updateLCD(currColor);
        nextState = sUPDATE_COLOR;
      } else if (mils - savedClock >= timeStep && numButtons > 0 ) {  // 1-3 handling button press
        // Serial.println("1-3");
        savedClock = mils;
        updateLCD(currColor);
        numButtons = 0; // Reset numButtons back to 0
        nextState = sPRINT_COLOR;
      } 
      break;
    case sUPDATE_COLOR:
      Serial.print("");
      if (mils - savedClock >= timeStep) { // 2-1 return to scan
        // Serial.println("2-1");
        savedClock = mils;
        updateColor();
        updateLCD(currColor);
        petWDT();
        nextState = sSCAN;
      }
      break;
    case sPRINT_COLOR:
      if (mils - savedClock >= timeStep) { // 3-1 after printing, return to scan state
        // Serial.println("3-1");
        savedClock = mils;
        addColor(savedColors, currColor);
        printColorsList(savedColors);
        petWDT();
        nextState = sSCAN;
      }
      break;
  }
  return nextState;
}














