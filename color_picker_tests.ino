#include "lcd.h"

const char* monitorMsg;
static RGB mockRGBData;
static int mockPotentiometerData;


// HARDWARE UNIT TESTS

// tests to make sure that the potentiometer value is within range
bool testPotentiometer() {
  int currPotVal = (int) analogRead(POT_PIN);
  if (0 < currPotVal < 1023) {
    return true;
  }
  return false;
}


// method to call hardware tests
bool testAllHardware() {
  if (!testPotentiometer()) {
    Serial.println("potentiometer test - FAILED");
  } 
  
  else {
    Serial.println("All Hardware Tests Pass!");
  }
}



// SOFTWARE UNIT TESTS

// method stores all software unit tests
void testAllSoftware() {
  int counter = 9;

  if (!testConvertToHex()) {
    counter--;
    Serial.println("convertToHex Simple - FAILED");
  } 
  if (!testConvertToHexEdge()) {
    counter--;
    Serial.println("convertToHex Edge - FAILED");
  } 
  if (!testAddColor()) {
    counter--;
    Serial.println("addColor - FAILED");
  } 
  if (!testPrintColorsList()) {
      counter--;
     Serial.println("printColorsList - FAILED");
  } 
  if (!testHexToRGB()) {
    counter--;
    Serial.println("hexToRGB - FAILED");
  } 
  if (!testAdjustOpacity()) {
    counter--;
    Serial.println("adjustOpacity - FAILED");
  } 
  if (!testCalculateOpacity()) {
    counter--;
    Serial.println("calculateOpacity - FAILED");
  }
  if (!testRGBMap()) {
     counter--;
     Serial.println("RGBMap - FAILED");
  }
  if (!testCalibrateSensor()) {
     counter--;
     Serial.println("calibrateSensor - FAILED");
  }
  
  if (counter == 9) {
    Serial.println("All Software Tests Pass!");
  }
  
}

// method tests the converToHex method that takes in the red, green, blue and 
// converts it into the corresponding uint_16. The test tests for simple cases with
// white, red, black
bool testConvertToHex() {
  // simple test - white
  uint16_t white = 65535;
  if (white != convertToHex(255, 255, 255)) {
    return false;
  } 

  // simple test - red
  uint16_t red = 63488;
  if (red != convertToHex(255, 0, 0)) {
    return false;
  } 

  // simple test - black
  uint16_t black = 0;
  if (black != convertToHex(0, 0, 0)) {
    return false;
  } 

  return true;
}

// method tests for edge cases where the inputted value is out of range, the method
// should call handle error to print error
bool testConvertToHexEdge() {
  // edge case
  convertToHex(300,300,300);
  const char* errorMsg = "RGB values out of range";  
  if (strcmp(errorMsg, monitorMsg) == 0) {
    return true;
  } else {
    return false;
  }
}

// method tests ability to addcolor to a list by checking if the color is added. Size is not
// considered since the list is set to size 10
bool testAddColor() {
  uint16_t savedColors[10];
  uint16_t color = 65535;

  // simple case - adding one color to an empty list
  addColor(savedColors, color);
  if (!savedColors[0] == color) {
    return false;
  } 

  return true;
}

// tests that the printed list of colors is accurate by checking global variable.
bool testPrintColorsList() {
  uint16_t savedColors[10] = {0,0,0,0,0,0,0,0,0,0};
  printColorsList(savedColors);
  const char* expectedMsg = "0 0 0 0 0 0 0 0 0 0 ";
  if (strcmp(expectedMsg, monitorMsg) == 0) {
    return true;
  } else {
    return false;
  }
}

// method tests the hexToRGB method that takes in the currColor and pointers to the r, g, b variables, and 
// converts the expected RGB.
bool testHexToRGB() {
  uint16_t testValues[] = {0x0000, 0xFFFFF, 0xF800};
  uint8_t r, g, b;

  // Tests white 
  hexToRgb(testValues[0], r, g, b);
  if ((r != 0) or (g != 0) or (b != 0)) {
      return false;
  } 

  // Tests black 
  hexToRgb(testValues[1], r, g, b);
  if ((r != 255) or (g != 255) or (b != 255)) {
      return false;
  }

  // Tests red 
  hexToRgb(testValues[2], r, g, b);
  if ((r != 255) or (g != 0) or (b != 0)) {
      return false; 
  }
  return true;
}

// method tests the adjustOpacity method that takes in the rgb and opacity and adjusts
// the values accordingly
bool testAdjustOpacity() {
  // full opacity -> expect same colour
  uint8_t r, g, b;
  r = 200;
  g = 200;
  b = 200;
  float opacity = 1.0;

  RGB actualColor = adjustOpacity(r, g, b, opacity);
  if (actualColor.r != r || actualColor.g != g || actualColor.b != b) {
    return false;
  }

  // no opacity -> expect white
  opacity = 0.0;
  actualColor = adjustOpacity(r, g, b, opacity);
  if (actualColor.r != 255 || actualColor.g != 255 || actualColor.b != 255) {
    return false;
  }

  // half opacity
  opacity = 0.5;
  actualColor = adjustOpacity(r, g, b, opacity);
  if (actualColor.r != 227 || actualColor.g != 227 || actualColor.b != 227) {
    return false;
  }
  return true;
}

// method tests that the opacity calulation from the potentiometer value is mapped
// correctly
bool testCalculateOpacity() {
  int potVal = 1023;
  float calculatedOpacity = calculateOpacity(potVal);
  if (calculatedOpacity != 1.0) {
    return false;
  }

  potVal = 0;
  calculatedOpacity = calculateOpacity(potVal);
  if (calculatedOpacity != 0) {
    return false;
  }

  potVal = -300;
  calculatedOpacity = calculateOpacity(potVal);
  const char* errorMsg = "opacity is mapped outside of the 0-1 range ";
  if (strcmp(errorMsg, monitorMsg) == 0) {
    return false;
  }
  
  return true;
}

// tests RGB map maps the colors correctly by considering extreme cases as well as edge case with negative values
bool testRGBMap() {
  // Testing if function correctly maps a middle range input value 
  if (RGBmap(50, 0, 100, 0, 255) != 127) {
    return false;
  } 
  // Testing below lower boundary 
  if (RGBmap(0, 0, 100, 0, 200) != 0 ) {
    return false;
  } 
  // Testing if ufnction properly handles negative numbers
  if (RGBmap(-10, 0, 100, 0, 255) != 255) {
    return false;
  } 

  return true;
}

// tests calibration of sensor to make sure that the colors are adjusted accordingly
bool testCalibrateSensor() {
  mockRGBData = {200,200,200};
  redCal = {50,1000};
  blueCal = {30, 800};
  greenCal = {20, 600};
  std::vector<int> calibratedColors = calibrateSensor(mockRGBData.r, mockRGBData.g, mockRGBData.g);
  std::vector<int> expectedColors = {40, 79, 56};
  if (calibratedColors != expectedColors) {
    return false;
  }
  return true;
}



// MAIN tests 

void testAllMain() {
  int counter = 6;
  if (!testUpdateColor()) {
    counter--;
    Serial.println("updateColor - FAILED");
  } 
  if (!testButtonISR()) {
    counter--;
    Serial.println("buttonISR - FAILED");
  } 
  if (!testStateTransitions(1)) { // testing 1-2
    counter--;
    Serial.println("Transition 1-2 - FAILED");
  }
  if (!testStateTransitions(2)) { // testing 1-3
    counter--;
    Serial.println("Transition 1-3 - FAILED");
  }
  if (!testStateTransitions(4)) { // testing 2-1
    counter--;
    Serial.println("Transition 2-1 - FAILED");
  }
  if (!testStateTransitions(5)) { // testing 3-1
    counter--;
    Serial.println("Transition 3-1 - FAILED");
  }
  
  if (counter == 6) {
    Serial.println("All Main Tests Pass!");
  }
}

// test fetching and updating inputs (potentiometer, rgb sensor), based on the inputted values, the 
// variables updated should match the expected values
bool testUpdateColor() {
  // tests to make sure that the output of updateColor correctly passes the rgb, combines it with the opacity, 
  // and updates the currColor variable.

  //calibration setting
  redCal = {0,255};
  blueCal = {0, 255};
  greenCal = {0, 255};

  // simple test with red and full opacity
  mockRGBData = {255,0,0};
  mockPotentiometerData = 1023;
  uint16_t expected_color = 63488;
  updateColor();
  if (currColor != expected_color) {
    return false;
  }

  // basic test with {200,200,200} and 0.5 opacity
  mockRGBData = {200,200,200};
  mockPotentiometerData = 512;
  expected_color = 59164;
  updateColor();
  if (currColor != expected_color) {
    return false;
  }

  // basic test with {87, 43, 20} and 0 opacity
  mockRGBData = {87,43,20};
  mockPotentiometerData = 0;
  expected_color = 65535;
  updateColor();
  if (currColor != expected_color) {
    return false;
  }
  return true;
}


// method tests that the button ISR interaction leads to the color being added to the list
bool testButtonISR() {
  // initialize savedColors list to be al white
  for (int i=0; i<10; i++) {
    savedColors[i] = 0;
  }
  numButtons = 0; // Hard reset the numButtons to be 0 (not pressed)
  
  // Mock the buttonPressISR which should update the numButtons 
  buttonPressISR();
  if (numButtons != 1) {
    return false;
  } 

  // After simulating button press, the color should be added to the list properly 
  addColor(savedColors, 18432);
  
  uint16_t expectedColors[10] = {18432,0,0,0,0,0,0,0,0,0};
  bool containsColor = false;
  for (int i=0; i<10; i++) {
    if (savedColors[i] == 18432) {
      containsColor = true;
    }
  }
  return containsColor;
}


// test FSM helper method by taking in the expected inputs and outputs, computes the actual values, and checks to see if the values are the same
bool testTransition(state startState, state endState, state_inputs testStateInputs, state_vars startStateVars, state_vars endStateVars, bool verbos) { 
  currColor = startStateVars.currColor;
  currOpacity = startStateVars.currOpacity;
  potVal = startStateVars.potVal;
  timeStep = startStateVars.timeStep;
  savedClock = startStateVars.savedClock;
  numButtons = startStateVars.numButtons;
  
  state resultState = updateFSM(startState, testStateInputs.mils);
  bool passedTest = (endState == resultState and
                      currColor == endStateVars.currColor 
                      and
                      currOpacity == endStateVars.currOpacity 
                      and
                      potVal == endStateVars.potVal 
                      and
                      timeStep == endStateVars.timeStep and
                      savedClock == endStateVars.savedClock
                      );
  if (! verbos) {
    return passedTest;
  } else if (passedTest) {
    char sToPrint[200];
    sprintf(sToPrint, "Test from %s to %s PASSED", s2str(startState), s2str(endState));
    Serial.println(sToPrint);
    return true;
  } else {
    char sToPrint[200];
    Serial.println(s2str(startState));
    sprintf(sToPrint, "Test from %s to %s FAILED", s2str(startState), s2str(endState));
    Serial.println(sToPrint);
    sprintf(sToPrint, "End state expected: %s | actual: %s", s2str(endState), s2str(resultState));
    Serial.println(sToPrint);
    sprintf(sToPrint, "Inputs: mils %ld", testStateInputs.mils);
    Serial.println(sToPrint);
    sprintf(sToPrint, "          %2s | %2s | %5s |%9s | %11s", "currColor", "currOpacity", "potVal", "timeStep", "savedClock");
    Serial.println(sToPrint);
    sprintf(sToPrint, "expected: %2s | %2s | %5s |%9s | %11s", endStateVars.currColor, endStateVars.currOpacity, endStateVars.potVal, endStateVars.timeStep, endStateVars.savedClock);
    Serial.println(sToPrint);
    sprintf(sToPrint, "actual:   %2s | %2s | %5s |%9s | %11s", currColor, currOpacity, potVal, timeStep, savedClock);
    Serial.println(sToPrint);
    return false;
  }
}

// method that stores the variables in each state transition and checks the expected values against the returned values.
// holds tests for transiiton 1-2, 1-3, 3-1, 2-1
bool testStateTransitions(int i) {
  state startState;
  state endState;
  state_inputs testStateInputs;
  state_vars startStateVars;
  state_vars endStateVars;
  switch (i) {
    case 0: // transition 1-1
      mockPotentiometerData = 1023;
      mockRGBData = {255,255,255};
      startState = (state) 1;
      endState = (state) 1;
      testStateInputs.mils = 500;

      startStateVars.numButtons = 0;
      startStateVars.currColor = 65535;
      startStateVars.currOpacity = 1.0;
      startStateVars.savedClock = 500;
      startStateVars.timeStep = 50;
      startStateVars.potVal = 1023;

      endStateVars.currColor = 65535;
      endStateVars.currOpacity = 1.0;
      endStateVars.savedClock = 500;
      endStateVars.timeStep = 50;
      endStateVars.potVal = 1023;
      return testTransition(startState, endState, testStateInputs, startStateVars, endStateVars, true);
      break;
    case 1: // transition 1-2
      startState = (state) 1;
      endState = (state) 2;
      testStateInputs.mils = 600;
      
      startStateVars.numButtons = 0;
      startStateVars.currColor = 65535;
      startStateVars.currOpacity = 1.0;
      startStateVars.savedClock = 500;
      startStateVars.timeStep = 50;
      startStateVars.potVal = 1023;

      endStateVars.currColor = 65535;
      endStateVars.currOpacity = 1.0;
      endStateVars.savedClock = 600;
      endStateVars.timeStep = 50;
      endStateVars.potVal = 1023;

      return testTransition(startState, endState, testStateInputs, startStateVars, endStateVars, true);
      break;
    case 2: // transition 1-3
      startState = (state) 1;
      endState = (state) 3;
      testStateInputs.mils = 600;
      
      startStateVars.numButtons = 1;
      startStateVars.currColor = 65535;
      startStateVars.currOpacity = 1.0;
      startStateVars.savedClock = 500;
      startStateVars.timeStep = 50;
      startStateVars.potVal = 1023;

      endStateVars.currColor = 65535;
      endStateVars.currOpacity = 1.0;
      endStateVars.savedClock = 600;
      endStateVars.timeStep = 50;
      endStateVars.potVal = 1023;

      return testTransition(startState, endState, testStateInputs, startStateVars, endStateVars, true);
      break;
    case 3: // transition 2-2
      startState = (state) 2;
      endState = (state) 2;
      testStateInputs.mils = 500;
      
      startStateVars.numButtons = 1;
      startStateVars.currColor = 65535;
      startStateVars.currOpacity = 1.0;
      startStateVars.savedClock = 500;
      startStateVars.timeStep = 50;
      startStateVars.potVal = 1023;

      endStateVars.currColor = 65535;
      endStateVars.currOpacity = 1.0;
      endStateVars.savedClock = 600;
      endStateVars.timeStep = 50;
      endStateVars.potVal = 1023;

      return testTransition(startState, endState, testStateInputs, startStateVars, endStateVars, true);
      break;
    case 4: // transition 2-1
      startState = (state) 2;
      endState = (state) 1;
      mockRGBData = {255, 255, 255};
      // calibrate sensor
      redCal = {0,255};
      blueCal = {0, 255};
      greenCal = {0, 255};
      mockPotentiometerData = 1023;
      testStateInputs.mils = 600;
      
      startStateVars.numButtons = 1;
      startStateVars.currColor = 65535;
      startStateVars.currOpacity = 1.0;
      startStateVars.savedClock = 500;
      startStateVars.timeStep = 50;
      startStateVars.potVal = 1023;

      endStateVars.currColor = 65535;
      endStateVars.currOpacity = 1.0;
      endStateVars.savedClock = 600;
      endStateVars.timeStep = 50;
      endStateVars.potVal = 1023;

      return testTransition(startState, endState, testStateInputs, startStateVars, endStateVars, true);
      break;
    case 5: // transition 3-1
      mockPotentiometerData = 1023;
      mockRGBData = {0,0,0};
      // initialize savedColors list to be al white
      for (int i=0; i<10; i++) {
        savedColors[i] = 65535;
      }
      startState = (state) 3;
      endState = (state) 1;
      testStateInputs.mils = 600;
      startStateVars.numButtons = 1;

      startStateVars.currColor = 0;
      startStateVars.currOpacity = 1.0;
      startStateVars.potVal = 1023;
      startStateVars.savedClock = 500;
      startStateVars.timeStep = 50;

      endStateVars.currColor = 0;
      endStateVars.currOpacity = 1.0;
      endStateVars.potVal = 1023;
      endStateVars.savedClock = 600;
      endStateVars.timeStep = 50;

      testTransition(startState, endState, testStateInputs, startStateVars, endStateVars, true);

      // check the saved colors list to contain black
      for (int i=0; i<10; i++) {
        if (savedColors[i] == 0) {
          return true;
        }
      }
      
      break;
    default: // transiiton 3-3
      mockPotentiometerData = 1023;
      mockRGBData = {0,0,0};
      startState = (state) 3;
      endState = (state) 3;
      testStateInputs.mils = 500;

      startStateVars.numButtons = 1;
      startStateVars.currColor = 0;
      startStateVars.currOpacity = 1.0;
      startStateVars.potVal = 1023;
      startStateVars.savedClock = 500;
      startStateVars.timeStep = 50;

      endStateVars.currColor = 0;
      endStateVars.currOpacity = 1.0;
      endStateVars.potVal = 1023;
      endStateVars.savedClock = 600;
      endStateVars.timeStep = 50;
      return testTransition(startState, endState, testStateInputs, startStateVars, endStateVars, true);
      break;
  }
}


/*        
 * Helper function for printing states
 */
char* s2str(state s) {
  switch(s) {
    case sSCAN:
    return "(1) SCAN";
    case sUPDATE_COLOR:
    return "(2) UPDATE_COLOR";
    case sPRINT_COLOR:
    return "(3) PRINT_COLOR";
    default:
    return "???";
  }
}




