 
#include "Adafruit_TCS34725.h"
#include <vector>

// unomment if testing
// #define TESTING


/*
 * An enum to keep all FSM states in one place
 */
typedef enum {
  sSCAN = 1,
  sUPDATE_COLOR = 2,
  sPRINT_COLOR = 3,
} state;


/*
 * A struct to keep all r g b values in one place
 */
typedef struct {
  uint16_t r;
  uint16_t g; 
  uint16_t b; 
} RGB;


/*
 * A struct to keep all state inputs in one place
 */
typedef struct {
  int numButtons;
  long mils;
} state_inputs;

/*
 * A struct to keep all state variables in one place
 */
typedef struct {
  uint16_t currColor;
  float currOpacity;
  int potVal;
  uint16_t savedColors;
  int timeStep;
  int savedClock;
} state_vars;


// Setup Functions
void initializeSystem();

// Read in inputs and udpate variables
void updateColor();

// Controller
state updateFSM(state curState, long mils);

// Helper Functions
void addColor(uint16_t* savedColors, uint16_t color);
RGB adjustOpacity(uint8_t r, uint8_t g, uint8_t b, float opacity);
RGB getRGBSensorData();
std::vector<int> calibrateSensor(uint16_t &r, uint16_t &g, uint16_t &b);
void buttonPressISR();

// Error handling
void handleError(const char* errorMsg);