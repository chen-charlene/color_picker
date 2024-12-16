#include "lcd.h"
#include <SPI.h>
#include "config.h"
#include <stdint.h>
#include "wdt.h"
#include <vector>

//10MHz
#define SPI_SPEED 10000000




Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);

// method to initiliaze GPIO, SPI, Potentiometer, button, screen, gamma table, 
void initializeSystem() {
  //GPIO init
  pinMode(DEV_CS_PIN, OUTPUT);
  pinMode(DEV_RST_PIN, OUTPUT);
  pinMode(DEV_DC_PIN, OUTPUT);
  pinMode(DEV_BL_PIN, OUTPUT);
  analogWrite(DEV_BL_PIN,140);
  Serial.println("GPIO Initialized!");

  //SPI init
  SPI.begin();
  //Increases the SPI speed to 10MHz
  SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
  Serial.println("SPI Initialized!");

  // Potentiometer init
  pinMode(POT_PIN, INPUT);
  Serial.println("Potentiometer Initialized!");

  //Button init
  pinMode(BUT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUT_PIN), buttonPressISR, FALLING);
  Serial.println("Button Initialized!");

  //init screen
  LCDInit();
  Serial.println("Screen Initialized!");
  LCD_Clear(0xFFFF);

  // initialize gamma table
  gammaTableInit();

  if (tcs.begin()) {
    Serial.println("RGB Sensor Initialized!");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    // while (1); // halt!
  }

  //init WDT
  TIMER_INT = getNextCPUINT(1);
  WDT_INT = getNextCPUINT(TIMER_INT);
  initWDT();
  petWDT();
  Serial.println("Ready to detect color!");
}

// method called by ISR to increment numButtons by 1 to indicate that button has been pressed
void buttonPressISR() {
  numButtons++;
}

int RGBmap(unsigned int x, unsigned int inlow, unsigned int inhigh, int outlow, int outhigh){
  float flx = float(x);
  float fla = float(outlow);
  float flb = float(outhigh);
  float flc = float(inlow);
  float fld = float(inhigh);

  float res = ((flx-flc)/(fld-flc))*(flb-fla) + fla;

  if (res < 0) res = 0;
  if (res > 255) res = 255;

  return int(res);
}

// method to calibrate sensor based on black and white values upon initial calibration settings
std::vector<int> calibrateSensor(uint16_t &r, uint16_t &g, uint16_t &b) {
  uint16_t colorTemp, lux;
  int c = r + g + b;
  int redval, greenval, blueval;

  redval = RGBmap(r, redCal.blackValue, redCal.whiteValue, 0, 255);
  greenval = RGBmap(g, greenCal.blackValue, greenCal.whiteValue, 0, 255);
  blueval = RGBmap(b, blueCal.blackValue, blueCal.whiteValue, 0, 255);
  
  return {redval, greenval, blueval};
}

// method to take in potentiometer and rgb data, performs calculations to adjust the color, and returns the adjusted color in hex
void updateInputs() {
  // get the input values
  potVal = getPotentiometerData();
  RGB colors = getRGBSensorData();

  delay(50);

  // convert the colours based on sensor calibration
  uint16_t red = colors.r;
  uint16_t green = colors.g;
  uint16_t blue = colors.b;

  std::vector<int> calibratedColors = calibrateSensor(red, green, blue);

  int correctRed = calibratedColors[0];
  int correctGreen = calibratedColors[1];
  int correctBlue = calibratedColors[2];

  // convert potentiometer value to be within 0-1
  currOpacity = calculateOpacity(potVal);

  // adjsut the colour based on opacity value
  RGB rgbWithOpacity = adjustOpacity(correctRed, correctGreen, correctBlue, currOpacity);
  uint16_t colorWithOpacity = convertToHex(rgbWithOpacity.r, rgbWithOpacity.g, rgbWithOpacity.b);

  // update currColor variable
  currColor = colorWithOpacity;
}


// method to add color to a circular list. Takes in the list of colors and the color to add
static int head = 0;
static int tail = 0;
static bool isFull = false;

void addColor(uint16_t* savedColors, uint16_t color) {
  if (isFull) {
    head = (head + 1) % 10;
  }
  savedColors[tail] = color;
  tail = (tail + 1) % 10;
  if (tail == head) {
    isFull = true;
  }
}

// method to update the color displayed on screen, takes in a color. 
void updateLCD(uint16_t color) {
  // Clears the screen with the new color
  LCD_Clear(color);
}

// Convert the unit16_t format to a readable RGB value 
void hexToRgb(uint16_t rgb565, uint8_t &r, uint8_t &g, uint8_t &b) {
    // Extract the red, green, and blue components
    r = (rgb565 >> 11) & 0x1F; // Top 5 bits
    g = (rgb565 >> 5) & 0x3F;  // Middle 6 bits
    b = rgb565 & 0x1F;         // Bottom 5 bits

    // Scale the values back to 0-255
    r = (r * 255) / 31; // Scale red from 0–31 to 0–255
    g = (g * 255) / 63; // Scale green from 0–63 to 0–255
    b = (b * 255) / 31; // Scale blue from 0–31 to 0–255
}


// Changes the opacity of a color based on a 0-1 opacity value
RGB adjustOpacity(uint8_t r, uint8_t g, uint8_t b, float opacity) {
    r = (1-opacity) * 255 + (opacity * r);
    g = (1-opacity) * 255 + (opacity * g);
    b = (1-opacity) * 255 + (opacity * b);

    RGB color = {r,g,b};
    return color;
}

// method to convert from rgb to hex, returns the color in hex format
uint16_t convertToHex(int red, int green, int blue) {
    // Convert to RGB565
    uint16_t color = ((red & 0xF8) << 8) | // Red: top 5 bits
                      ((green & 0xFC) << 3) | // Green: top 6 bits
                      (blue >> 3);            // Blue: top 5 bits

    uint8_t r = (color >> 11) & 0x1F;
    uint8_t g = (color >> 5) & 0x3F;
    uint8_t b = color & 0x1F;
    r = (r * 255) / 31;
    g = (g * 255) / 63;
    b = (b * 255) / 31;

    // Find the original and converted dominant color
    int maxOriginal = max(red, max(green, blue));
    int maxConverted = max(r, max(g, b));
    
    if ((red > 255) or (green > 255) or (blue > 255) or (red < 0) or (green < 0) or (blue < 0)) {
      const char* errorMsg = "RGB values out of range";
      handleError(errorMsg);
    } else if (((red == maxOriginal) && (r != maxConverted)) || 
    ((green == maxOriginal) && (g != maxConverted)) || 
    ((blue == maxOriginal) && (b != maxConverted))) {
      const char* errorMsg = "Dominant color changed after conversion";
      handleError(errorMsg);
    }
    // } else if (!checkBitAloc(color)) {
    //   handleError("RGB bit allocation is incorrect");
    //   return 0;
    // }
    return color;
}

// helper method to initialize the gamma table for calibration
void gammaTableInit() {

  // TODO: these values are unchanged from the original numbers
  redCal.blackValue = 67;
  redCal.whiteValue = 744;
  greenCal.blackValue = 36;
  greenCal.whiteValue = 473;
  blueCal.blackValue = 26;
  blueCal.whiteValue = 322;

  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;

    gammatable[i] = int(x);
  }
}

// method for calculating the opacity and takes in the potentiometer value, returns the float value
float calculateOpacity(int potVal) {
  float mappedVal = potVal / 1023.0;
  if ((mappedVal < 0) || (mappedVal > 1)) {
    handleError("opacity is mapped outside of the 0-1 range");
  }
  return mappedVal;
}



#ifndef TESTING // unit testing turned off: normal behavior

  // prints error to serial monitor
  void handleError(const char* errorMsg) {
    Serial.println(errorMsg);
    // Serial.println("System restarting");
    // delay(5000);
  }

  // helper method to print the list of saved colours to the serial monitor
  void printColorsList(uint16_t* savedColorList) {
    Serial.println("Saved Colors:");
    for (int i = 0; i < 10; i++) {
        Serial.print("Color ");
        Serial.print(i+1);
        Serial.print(": 0x");
        Serial.println(savedColorList[i], HEX);  // Print in hexadecimal format
    }
  }

  // retrieves potentiometer data 
  int getPotentiometerData() {
    potVal = analogRead(POT_PIN);
    return potVal;
  }

  // retrieved rgb sensor data
  RGB getRGBSensorData() {
    uint16_t r, g, b, c;
    tcs.getRawData(&r, &g, &b, &c);
    return {r,g,b};
  }


#else

  // updates the monitor message variable to hold the current displayed message
  void handleError(const char* errorMessage) { 
    monitorMsg = errorMessage;
  }

  // helper method to print the list of saved colours to the serial monitor
  void printColorsList(uint16_t* savedColorList) {
    String resultColorString;
    for (int i = 0; i < 10; i++) {
        resultColorString += String(savedColorList[i], HEX) + " ";
    }
    // const char* convertString  = resultColorString.c_str();
    monitorMsg = strdup(resultColorString.c_str());
  }

  // mock potentiometer data
  int getPotentiometerData() {
    return mockPotentiometerData;
  }

  // returns mock sensor data
  RGB getRGBSensorData() {
    return mockRGBData;
  }

#endif



