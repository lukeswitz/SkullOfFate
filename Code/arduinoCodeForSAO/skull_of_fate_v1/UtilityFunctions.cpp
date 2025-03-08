// UtilityFunctions.cpp

#include "UtilityFunctions.h"
#include "NFCWriter.h"
#include "SparkFun_ST25DV64KC_Arduino_Library.h"  // Include the NFC library
#include <Adafruit_SleepyDog.h>


#include "ConfigManager.h"  // Include ConfigManager to access configManager

// Declare the ConfigManager instance as extern
extern ConfigManager configManager;

extern Config currentConfig;

extern int animationIndex;

// Debounce variables for double tap detection
static unsigned long lastDoubleTapTime_global = 0;            // Last time a double tap was handled
static const unsigned long DOUBLE_TAP_COOLDOWN_GLOBAL = 500;  // Cooldown duration in milliseconds

// Timing variables for button sequence detection
unsigned long lastRightButtonTime = 0;
bool specialModeActive = false;
const unsigned long SPECIAL_MODE_TIMEOUT = 2000; // 2 seconds timeout

static DoubleTapCallback doubleTapCallback = NULL;

// Function to set the double tap callback
void setDoubleTapCallback(DoubleTapCallback callback) {
  doubleTapCallback = callback;
}

/**
 * @brief Updates an integer configuration parameter.
 * 
 * This function updates a configuration parameter with an integer value.
 * It logs the process to the Serial Monitor for debugging purposes.
 * 
 * @param key The key of the configuration parameter to update.
 * @param value The new integer value to assign to the parameter.
 */
void updateConfigParameterInt(const String &key, int value) {
  // Serial.print(F("Updating config parameter '"));
  // Serial.print(key);
  // Serial.print(F("' to value "));
  // Serial.println(value);

  // Create a temporary JSON document to hold the key-value pair
  DynamicJsonDocument tempDoc(2048);  // Adjust size if necessary
  tempDoc[key] = value;

  // Obtain a JsonVariant reference to pass to ConfigManager
  JsonVariant variant = tempDoc[key];

  // Attempt to update the configuration
  if (configManager.updateConfig(key, variant)) {
    // Serial.print(F("Successfully updated '"));
    // Serial.print(key);
    // Serial.print(F("' to "));
    // Serial.println(value);
  } else {
    Serial.print(F("Failed to update '"));
    Serial.print(key);
    Serial.println(F("'."));
  }
}

/**
 * @brief Updates a string configuration parameter.
 * 
 * This function updates a configuration parameter with a string value.
 * It logs the process to the Serial Monitor for debugging purposes.
 * 
 * @param key The key of the configuration parameter to update.
 * @param value The new string value to assign to the parameter.
 */
void updateConfigParameterString(const String &key, const String &value) {
  // Serial.print(F("Updating config parameter '"));
  // Serial.print(key);
  // Serial.print(F("' to value '"));
  // Serial.print(value);
  // Serial.println(F("'"));

  // Create a temporary JSON document to hold the key-value pair
  DynamicJsonDocument tempDoc(2048);  // Adjust size if necessary
  tempDoc[key] = value;

  // Obtain a JsonVariant reference to pass to ConfigManager
  JsonVariant variant = tempDoc[key];

  // Attempt to update the configuration
  if (configManager.updateConfig(key, variant)) {
    // Serial.print(F("Successfully updated '"));
    // Serial.print(key);
    // Serial.print(F("' to '"));
    // Serial.print(value);
    // Serial.println(F("'"));
  } else {
    Serial.print(F("Failed to update '"));
    Serial.print(key);
    Serial.println(F("'."));
  }
}

/**
 * @brief Updates a float configuration parameter.
 * 
 * This function updates a configuration parameter with a float value.
 * It logs the process to the Serial Monitor for debugging purposes.
 * 
 * @param key The key of the configuration parameter to update.
 * @param value The new float value to assign to the parameter.
 */
void updateConfigParameterFloat(const String &key, float value) {
  // Serial.print(F("Updating config parameter '"));
  // Serial.print(key);
  // Serial.print(F("' to value "));
  // Serial.println(value);

  // Create a temporary JSON document to hold the key-value pair
  DynamicJsonDocument tempDoc(2048);  // Adjust size if necessary
  tempDoc[key] = value;

  // Obtain a JsonVariant reference to pass to ConfigManager
  JsonVariant variant = tempDoc[key];

  // Attempt to update the configuration
  if (configManager.updateConfig(key, variant)) {
    // Serial.print(F("Successfully updated '"));
    // Serial.print(key);
    // Serial.print(F("' to "));
    // Serial.println(value);
  } else {
    Serial.print(F("Failed to update '"));
    Serial.print(key);
    Serial.println(F("'."));
  }
}

/**
 * @brief Updates a boolean configuration parameter.
 * 
 * This function updates a configuration parameter with a boolean value.
 * It logs the process to the Serial Monitor for debugging purposes.
 * 
 * @param key The key of the configuration parameter to update.
 * @param value The new boolean value to assign to the parameter.
 */
void updateConfigParameterBool(const String &key, bool value) {
  // Serial.print(F("Updating config parameter '"));
  // Serial.print(key);
  // Serial.print(F("' to value "));
  // Serial.println(value ? "true" : "false");

  // Create a temporary JSON document to hold the key-value pair
  DynamicJsonDocument tempDoc(2048);  // Adjust size if necessary
  tempDoc[key] = value;

  // Obtain a JsonVariant reference to pass to ConfigManager
  JsonVariant variant = tempDoc[key];

  // Attempt to update the configuration
  if (configManager.updateConfig(key, variant)) {
    // Serial.print(F("Successfully updated '"));
    // Serial.print(key);
    // Serial.print(F("' to "));
    // Serial.println(value ? "true" : "false");
  } else {
    Serial.print(F("Failed to update '"));
    Serial.print(key);
    Serial.println(F("'."));
  }
}

//SFE_ST25DV64KC tag;       // Create an instance of the ST25DV64KC NDEF class
//NFCWriter nfcWriter(tag); // Create an instance of NFCWriter

extern volatile bool animationInterrupted;


////////////////////////////////////////////////////////

// Simulation Configuration
const unsigned long GENERATION_INTERVAL_MS = 1000;  // Time between generations in milliseconds
const unsigned long FADE_STEP_DELAY_MS = 20;        // Delay between fade steps in milliseconds
const unsigned long RESET_DELAY_MS = 2000;          // Delay before resetting after conditions met
const uint32_t DEAD_CELL_COLOR = 0x000000;          // Black/off for dead cells

// Live Cell Colors (Cycle through these on each restart)
const uint32_t colorList[] = {
  0x00FF00,  // Green
  0xFF0000,  // Red
  0x0000FF,  // Blue
  0xFFFF00,  // Yellow
  0xFF00FF,  // Magenta
  0x00FFFF   // Cyan
};
const int numColors2 = sizeof(colorList) / sizeof(colorList[0]);
int currentColorIndex = 0;
uint32_t LIVE_CELL_COLOR;


// ---------------------------
// Simulation Variables
// ---------------------------

bool currentState[5][10];         // Current state of each cell (live or dead)
bool nextState[5][10];            // Next state of each cell after update
bool previousState[5][10];        // Previous state for comparison
bool secondPreviousState[5][10];  // Second previous state for oscillation detection

int stableCounter = 0;           // Counter for stable generations
const int STABLE_THRESHOLD = 3;  // Number of stable generations before reset

int oscillationCounter = 0;           // Counter for oscillations
const int OSCILLATION_THRESHOLD = 2;  // Number of oscillations before reset

unsigned long lastGenerationTime = 0;  // Last time the grid was updated
// Combined grid mapping for easy access
int grid[5][10];  // 5 rows, 10 columns

// ---------------------------
// Function Prototypes
// ---------------------------
void initializeGrid();
void setPredefinedPattern();
void randomizeGrid();
void copyGridsToCombinedGrid();
void updateGrid();
int countLiveNeighbors(int row, int col);
void displayGrid();
void printGridToSerial();
bool isEdgeCell(int row, int col);
bool areGridsEqual(bool grid1[5][10], bool grid2[5][10]);
bool checkOscillation();
void restartSimulation();
void clearGrid();
void fadeTransition(bool fadingOut, bool fadingIn);

////////////////////////////////////////////////////////

// Brightness Configuration
//const uint8_t MAX_BRIGHTNESS_PERCENT = 10; // Maximum brightness percentage (20%)
//const uint8_t MAX_BRIGHTNESS = (MAX_BRIGHTNESS_PERCENT * 255) / 100; // 20% of 255 = 51

// Animation Configuration
const int GRID_WIDTH = 5;
const int GRID_HEIGHT = 5;
const int FALL_SPEED_MS = 200;  // Time between piece moves in milliseconds

// Color Configuration for Tetrominoes
const uint32_t I_COLOR = 0x00FFFF;  // Cyan
const uint32_t O_COLOR = 0xFFFF00;  // Yellow
const uint32_t T_COLOR = 0xAA00FF;  // Purple
const uint32_t S_COLOR = 0x00FF00;  // Green
const uint32_t Z_COLOR = 0xFF0000;  // Red
const uint32_t J_COLOR = 0x0000FF;  // Blue
const uint32_t L_COLOR = 0xFF7F00;  // Orange


// ---------------------------
// Tetromino Definitions
// ---------------------------

struct Tetromino {
  int shape[4][4];        // 4x4 matrix representing the shape
  uint32_t color;         // Color of the tetromino
  int rotationState = 0;  // Rotation state (0, 1, 2, 3)
};
Tetromino currentPiece;  // Current tetromino piece

const int NUM_TETROMINOES = 7;

// Define the tetromino shapes
const int tetrominoShapes[NUM_TETROMINOES][4][4] = {
  // I Tetromino
  {
    { 0, 0, 0, 0 },
    { 1, 1, 1, 1 },
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 } },
  // O Tetromino
  {
    { 0, 0, 0, 0 },
    { 0, 1, 1, 0 },
    { 0, 1, 1, 0 },
    { 0, 0, 0, 0 } },
  // T Tetromino
  {
    { 0, 0, 0, 0 },
    { 1, 1, 1, 0 },
    { 0, 1, 0, 0 },
    { 0, 0, 0, 0 } },
  // S Tetromino
  {
    { 0, 0, 0, 0 },
    { 0, 1, 1, 0 },
    { 1, 1, 0, 0 },
    { 0, 0, 0, 0 } },
  // Z Tetromino
  {
    { 0, 0, 0, 0 },
    { 1, 1, 0, 0 },
    { 0, 1, 1, 0 },
    { 0, 0, 0, 0 } },
  // J Tetromino
  {
    { 0, 0, 0, 0 },
    { 1, 0, 0, 0 },
    { 1, 1, 1, 0 },
    { 0, 0, 0, 0 } },
  // L Tetromino
  {
    { 0, 0, 0, 0 },
    { 0, 0, 1, 0 },
    { 1, 1, 1, 0 },
    { 0, 0, 0, 0 } }
};

// Corresponding colors for the tetrominoes
const uint32_t tetrominoColors[NUM_TETROMINOES] = {
  I_COLOR, O_COLOR, T_COLOR, S_COLOR, Z_COLOR, J_COLOR, L_COLOR
};

// ---------------------------
// Animation Variables
// ---------------------------

int pieceX = 1;   // X position of the current piece
int pieceY = -4;  // Y position of the current piece (starts above the grid)

unsigned long lastFallTime = 0;  // Timestamp of the last fall update

// Button Variables
bool lastButtonState = HIGH;  // Assume button not pressed (pull-up resistor)
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;  // 50ms debounce

// ---------------------------
// Function Prototypes
// ---------------------------

void spawnNewPiece();
void rotatePiece();
void drawCurrentPiece();
void clearDisplay();
void copyShape(int dest[4][4], const int src[4][4]);
void rotateShape(int shape[4][4], int rotationState);


////////////////////////////////////////////////////////


// Droplet Configuration
const int MAX_DROPLETS_PER_GRID = 5;  // Maximum number of simultaneous droplets per grid
const int DROPLET_SPEED_MS = 100;     // Droplet falling speed in milliseconds per row


// ---------------------------
// Droplet Structure
// ---------------------------
struct Droplet {
  int row;                   // Current row (0-4)
  int column;                // Current column (0-4)
  int speed;                 // Falling speed (milliseconds per row)
  uint32_t color;            // Color of the droplet
  unsigned long lastUpdate;  // Timestamp of the last position update
};


// ---------------------------
// Color Selection Variables
// ---------------------------
int colorMode = 0;  // 0 = Random, 1-10 = colorArray[0-9]
// ---------------------------
// Function Prototypes
// ---------------------------
void createDroplet(Droplet droplets[], int &dropletCount, const int grid[5][5]);
void updateDroplets(Droplet droplets[], int &dropletCount, const int grid[5][5]);
void displayDroplets(Droplet droplets[], int dropletCount, const int grid[5][5]);
uint32_t getColor();
uint32_t ColorHSV(long hue, uint8_t sat, uint8_t val);


////////////////////////////////////////////////

// Extern variables from main sketch
extern bool bothButtonsPressed;
extern Adafruit_NeoPixel pixels;
extern void (*currentAnimation)(Adafruit_NeoPixel &);
extern void (*previousAnimation)(Adafruit_NeoPixel &);
extern Adafruit_LIS3DH lis;

// Define the array of colors globally
const int numColors = 10;  // Number of colors in the array

const uint8_t colorArray[numColors][3] = {
  { 255, 0, 0 },      // Red
  { 0, 255, 0 },      // Green
  { 0, 0, 255 },      // Blue
  { 255, 255, 0 },    // Yellow
  { 0, 255, 255 },    // Cyan
  { 255, 0, 255 },    // Magenta
  { 255, 165, 0 },    // Orange
  { 128, 0, 128 },    // Purple
  { 255, 255, 255 },  // White
  { 255, 192, 203 }   // Pink
};

Adafruit_ZeroPDM pdm(PIN_PDM_CLK, PIN_PDM_DATA);

const uint16_t sincfilter[DECIMATION] = {
  0, 2, 9, 21, 39, 63, 94, 132,
  179, 236, 302, 379, 467, 565, 674, 792,
  920, 1055, 1196, 1341, 1487, 1633, 1776, 1913,
  2042, 2159, 2263, 2352, 2422, 2474, 2506, 2516,
  2506, 2474, 2422, 2352, 2263, 2159, 2042, 1913,
  1776, 1633, 1487, 1341, 1196, 1055, 920, 792,
  674, 565, 467, 379, 302, 236, 179, 132,
  94, 63, 39, 21, 9, 2, 0
};

// GLOBAL VARIABLES
int16_t pcm_buffer[FFT_SIZE];
float spectrum[SPECTRUM_SIZE];
float max_all = 10.0;  // For dynamic scaling

// Define the grid mappings
// Left grid pixel indices (5x5 grid)
const int leftGrid[5][5] = {
  { -1, 0, 1, 2, -1 },     // y=0 (top row)
  { 3, 4, 5, 6, 7 },       // y=1
  { 8, 9, 10, 11, 12 },    // y=2
  { 13, 14, 15, 16, 17 },  // y=3
  { -1, 18, 19, 20, -1 }   // y=4 (bottom row)
};

// Right grid pixel indices (5x5 grid)
const int rightGrid[5][5] = {
  { -1, 21, 22, 23, -1 },  // y=0
  { 24, 25, 26, 27, 28 },
  { 29, 30, 31, 32, 33 },
  { 34, 35, 36, 37, 38 },
  { -1, 39, 40, 41, -1 }
};

// Brightness scaling variable
float brightnessFactor = 0.0;  // Start at 0%

// Volume threshold to determine when LEDs should light up
const float VOLUME_THRESHOLD = 3.0;  // Adjust based on testing

// Variables for rainbow animation
uint16_t globalHue = 0;  // Global hue offset for rainbow cycling

// Tempo (BPM) related variables
unsigned long lastBeatTime = 0;  // Timestamp of the last detected beat
float currentBPM = 120.0;        // Initial BPM estimate
const float MIN_BPM = 60.0;
const float MAX_BPM = 180.0;

// Hue increment range based on BPM
const uint16_t MIN_HUE_INCREMENT = 128;   // Slower cycle
const uint16_t MAX_HUE_INCREMENT = 1024;  // Faster cycle

// Smoothing factor for BPM updates
const float BPM_SMOOTHING = 0.8;


// Define thresholds for press duration (in milliseconds)
const unsigned long SHORT_PRESS_THRESHOLD = 500;  // 0.5 seconds
const unsigned long LONG_PRESS_THRESHOLD = 1000;  // 1 second


// Example function to handle long press action
void handleLongPress(int animationIndex) {
  // Implement your long press action here
  Serial.println("Long press action executed.");
  // For example, reset color to default
  //selectedColorIndex = 0;
  turnOnAllLEDs();
  updateConfigParameterInt("defaultAnimation", animationIndex);
  turnOffAllLEDs();
}


// Function declarations (prototypes) for functions used before their definitions
uint32_t HeatColor(uint8_t temperature, Adafruit_NeoPixel &pixels);
uint32_t Wheel(byte WheelPos, Adafruit_NeoPixel &pixels);

// Function definitions

void initializePeripherals() {
  Wire.begin();
  Serial.begin(9600);
  /*
  while (!Serial) {
    delay(10);  // Wait for serial port to connect
  }
  */
  Serial.println("Starting...");  // Added line

  // Initialize pins
  pinMode(HALL_EFFECT_PIN, INPUT_PULLUP);

  // Initialize buttons with pull-up resistors
  pinMode(LEFT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON_PIN, INPUT_PULLUP);

  // Initialize LEDs
  initializeLEDs();
}

void initializeNFC() {

  // Initialize microphone
  initializeMicrophone();

  // Initialize NFC tag
  if (!nfcWriter.initializeTag()) {
    Serial.println("Failed to initialize NFC tag.");
    while (1)
      ;  // Halt execution if NFC tag initialization fails
  }
}

void initializeLEDs() {
  // Set LED pins as outputs
  pinMode(RED_CUP_RIGHT_PIN, OUTPUT);
  pinMode(RED_CUP_LEFT_PIN, OUTPUT);

  pinMode(WHITE_SWORD_RIGHT_PIN, OUTPUT);
  pinMode(WHITE_SWORD_LEFT_PIN, OUTPUT);

  pinMode(YELLOW_WAND_UPPER_LEFT_PIN, OUTPUT);
  pinMode(YELLOW_WAND_UPPER_RIGHT_PIN, OUTPUT);
  pinMode(YELLOW_WAND_LOWER_LEFT_PIN, OUTPUT);
  pinMode(YELLOW_WAND_LOWER_RIGHT_PIN, OUTPUT);

  // Turn off all LEDs
  turnOffAllLEDs();
}

void initializeNeoPixels(Adafruit_NeoPixel &pixels) {
  pixels.begin();
  pixels.setBrightness(NEOPIXEL_BRIGHTNESS);
  pixels.show();  // Initialize all pixels to 'off'
}

void initializeAccelerometer(Adafruit_LIS3DH &lis) {
  if (!lis.begin(0x18)) {  // Default I2C address
    Serial.println("Could not start accelerometer.");
    while (1) yield();
  }
  lis.setRange(LIS3DH_RANGE_2_G);  // Set range to 2G
  lis.setDataRate(LIS3DH_DATARATE_50_HZ);
  lis.setClick(2, 80);  // Enable double-tap detection
}

void initializeMicrophone() {
  // Initialize the PDM microphone
  if (!pdm.begin()) {
    Serial.println("Failed to initialize PDM microphone!");
    while (1)
      ;
  }

  // Configure the PDM microphone
  if (!pdm.configure(SAMPLE_RATE * DECIMATION / 16, true)) {  // Mono mode
    Serial.println("Failed to configure PDM microphone!");
    while (1)
      ;
  }
}

// Magnetic sensor function
bool isMagneticFieldDetected() {
  return digitalRead(HALL_EFFECT_PIN) == LOW;
}


// Accelerometer functions
void getAccelerometerValues(Adafruit_LIS3DH &lis, float &x, float &y, float &z) {
  lis.read();  // Read accelerometer data
  x = lis.x_g;
  y = lis.y_g;
  z = lis.z_g;
}


/*
void getAccelerometerValues(Adafruit_LIS3DH &lis, float &x, float &y, float &z) {
  lis.read();      // get X Y and Z data at once
  x = lis.x;
  y = lis.y;
  z = lis.z;
}
*/
uint8_t getAccelerometerTap(Adafruit_LIS3DH &lis) {
  return lis.getClick();
}

// LED control functions
void setLED(uint8_t pin, bool state) {
  digitalWrite(pin, state ? HIGH : LOW);
}

void setLEDGroup(const char *groupName, bool state) {
  if (strcmp(groupName, "wands") == 0) {
    setLED(YELLOW_WAND_UPPER_LEFT_PIN, state);
    setLED(YELLOW_WAND_UPPER_RIGHT_PIN, state);
    setLED(YELLOW_WAND_LOWER_LEFT_PIN, state);
    setLED(YELLOW_WAND_LOWER_RIGHT_PIN, state);
  } else if (strcmp(groupName, "swords") == 0) {
    setLED(WHITE_SWORD_LEFT_PIN, state);
    setLED(WHITE_SWORD_RIGHT_PIN, state);
  } else if (strcmp(groupName, "cups") == 0) {
    setLED(RED_CUP_LEFT_PIN, state);
    setLED(RED_CUP_RIGHT_PIN, state);
  } else {
    Serial.println("Unknown LED group name.");
  }
}

void turnOffAllLEDs() {
  setLED(RED_CUP_LEFT_PIN, LOW);
  setLED(RED_CUP_RIGHT_PIN, LOW);

  setLED(WHITE_SWORD_LEFT_PIN, LOW);
  setLED(WHITE_SWORD_RIGHT_PIN, LOW);

  setLED(YELLOW_WAND_UPPER_LEFT_PIN, LOW);
  setLED(YELLOW_WAND_UPPER_RIGHT_PIN, LOW);
  setLED(YELLOW_WAND_LOWER_LEFT_PIN, LOW);
  setLED(YELLOW_WAND_LOWER_RIGHT_PIN, LOW);
}

void turnOnAllLEDs() {
  setLED(RED_CUP_LEFT_PIN, HIGH);
  setLED(RED_CUP_RIGHT_PIN, HIGH);

  setLED(WHITE_SWORD_LEFT_PIN, HIGH);
  setLED(WHITE_SWORD_RIGHT_PIN, HIGH);

  setLED(YELLOW_WAND_UPPER_LEFT_PIN, HIGH);
  setLED(YELLOW_WAND_UPPER_RIGHT_PIN, HIGH);
  setLED(YELLOW_WAND_LOWER_LEFT_PIN, HIGH);
  setLED(YELLOW_WAND_LOWER_RIGHT_PIN, HIGH);
}

// Function to test all LEDs
void testAllLEDs() {
  // Test red LEDs (Cups)
  Serial.println("Testing Red LEDs (Cups)");
  setLEDGroup("cups", HIGH);
  delay(1000);
  setLEDGroup("cups", LOW);

  // Test white LEDs (Swords)
  Serial.println("Testing White LEDs (Swords)");
  setLEDGroup("swords", HIGH);
  delay(1000);
  setLEDGroup("swords", LOW);

  // Test yellow LEDs (Wands)
  Serial.println("Testing Yellow LEDs (Wands)");
  setLEDGroup("wands", HIGH);
  delay(1000);
  setLEDGroup("wands", LOW);
}

// NeoPixel functions
void setAllNeoPixelsColor(Adafruit_NeoPixel &pixels, uint32_t color) {
  for (int i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, color);
  }
  pixels.show();
}

// Button functions
bool isLeftButtonPressed() {
  return digitalRead(LEFT_BUTTON_PIN) == LOW;
}

bool isSpecialModeActive() {
  // Check if special mode has timed out
  if (specialModeActive && (millis() - lastRightButtonTime > SPECIAL_MODE_TIMEOUT)) {
    specialModeActive = false;
  }
  return specialModeActive;
}


bool isRightButtonPressed() {
  bool currentState = (digitalRead(RIGHT_BUTTON_PIN) == LOW);
  
  // Only update lastRightButtonTime and activate special mode
  // when button is first pressed (not continuously while held)
  static bool lastRightState = HIGH;
  
  if (currentState && lastRightState == HIGH) {
    // Button was just pressed
    lastRightButtonTime = millis();
    specialModeActive = true;
    Serial.println("Special mode activated! Press both buttons within 2 seconds for full card range (1-78)");
  }
  
  lastRightState = currentState;
  return currentState;
}

bool isBothButtonsPressed() {
  return isLeftButtonPressed() && isRightButtonPressed();
}

// Utility function
void waitForButtonPress(bool (*buttonFunction)()) {
  while (!buttonFunction()) {
    // Do nothing, just wait
    delay(10);
  }
  // Debounce delay
  delay(50);
}

// Function to map heat values to colors
uint32_t HeatColor(uint8_t temperature, Adafruit_NeoPixel &pixels) {
  uint8_t t192 = (temperature * 191) / 255;  // Scale 'heat' down from 0-255 to 0-191

  uint8_t heatramp = t192 & 0x3F;  // 0..63
  heatramp <<= 2;                  // Scale up to 0..252

  if (t192 > 128) {
    // Hot
    return pixels.Color(255, 255, heatramp);
  } else if (t192 > 64) {
    // Medium
    return pixels.Color(255, heatramp, 0);
  } else {
    // Cool
    return pixels.Color(heatramp, 0, 0);
  }
}

// Flame effect with grid mapping
void flameEffect(Adafruit_NeoPixel &pixels) {
  Serial.println("Flame Effect. Press LEFT button to exit.");

  // Flame effect parameters
  const uint8_t cooling = 50;    // Less cooling to allow higher flames
  const uint8_t sparking = 120;  // Increase sparking for more activity
  const uint8_t gridWidth = 5;
  const uint8_t gridHeight = 5;

  uint8_t heatGrid[gridHeight][gridWidth] = { 0 };

  unsigned long previousMillis = 0;
  const unsigned long interval = 30;  // Interval in milliseconds

  // Variables for right button press detection
  bool rightButtonPressed = false;
  unsigned long pressStartTime = 0;
  bool longPressHandled = false;

  bool rightButtonLastState = HIGH;  // Assume button not pressed (pull-up resistor)

  while (!isLeftButtonPressed() && !animationInterrupted) {
    Watchdog.reset();
    unsigned long currentMillis = millis();

    // Check for both buttons pressed
    if (isBothButtonsPressed()) {
      handleBothButtonsPressed();
    }

    // Read the current state of the right button
    bool rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);

    if (rightButtonLastState == HIGH && rightButtonCurrentState == LOW) {
      // Button was just pressed
      delay(50);  // Debounce delay
      rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);
      if (rightButtonCurrentState == LOW) {
        rightButtonPressed = true;
        pressStartTime = millis();
        longPressHandled = false;
        Serial.println("Right button pressed.");
      }
    }

    if (rightButtonLastState == LOW && rightButtonCurrentState == HIGH) {
      // Button was just released
      if (rightButtonPressed) {
        unsigned long pressDuration = millis() - pressStartTime;
        if (pressDuration < SHORT_PRESS_THRESHOLD) {
          // Short press detected
          Serial.println("Short press detected. No change for this animation");
          // selectedColorIndex = (selectedColorIndex + 1) % numColors;
          // Serial.print("Color changed to index ");
          // Serial.println(selectedColorIndex);
        } else if (pressDuration >= LONG_PRESS_THRESHOLD && !longPressHandled) {
          // Long press detected upon release, only if not handled during holding
          Serial.println("Long press detected.");
          handleLongPress(animationIndex);  // Example function
        }
        rightButtonPressed = false;
      }
    }

    // Handle long press if button is still pressed and threshold exceeded
    if (rightButtonPressed && !longPressHandled) {
      unsigned long pressDuration = millis() - pressStartTime;
      if (pressDuration >= LONG_PRESS_THRESHOLD) {
        longPressHandled = true;
        Serial.println("Long press detected (while holding).");
        handleLongPress(animationIndex);  // Example function
      }
    }

    rightButtonLastState = rightButtonCurrentState;

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      // Step 1. Cool down every cell a little
      for (int y = 0; y < gridHeight; y++) {
        for (int x = 0; x < gridWidth; x++) {
          if (leftGrid[y][x] != -1 || rightGrid[y][x] != -1) {
            int cooldown = random(0, ((cooling * 10) / gridHeight) + 2);

            if (cooldown > heatGrid[y][x]) {
              heatGrid[y][x] = 0;
            } else {
              heatGrid[y][x] = heatGrid[y][x] - cooldown;
            }
          }
        }
      }

      // Step 2. Heat from each cell drifts 'up' and diffuses a little
      for (int y = gridHeight - 1; y >= 2; y--) {
        for (int x = 0; x < gridWidth; x++) {
          if (leftGrid[y][x] != -1 || rightGrid[y][x] != -1) {
            heatGrid[y][x] = (heatGrid[y - 1][x] + heatGrid[y - 2][x] + heatGrid[y - 2][x]) / 3;
          }
        }
      }

      // Step 3. Randomly ignite new 'sparks' near the bottom
      for (int x = 0; x < gridWidth; x++) {
        if (leftGrid[gridHeight - 1][x] != -1 || rightGrid[gridHeight - 1][x] != -1) {
          if (random(255) < sparking) {
            heatGrid[gridHeight - 1][x] = heatGrid[gridHeight - 1][x] + random(160, 255);
            if (heatGrid[gridHeight - 1][x] > 255) {
              heatGrid[gridHeight - 1][x] = 255;
            }
          }
        }
      }

      // Adjusted to sometimes spark at higher levels
      for (int y = gridHeight - 2; y >= 0; y--) {
        for (int x = 0; x < gridWidth; x++) {
          if (leftGrid[y][x] != -1 || rightGrid[y][x] != -1) {
            if (random(255) < (sparking / 15)) {  // Less frequent higher sparks
              heatGrid[y][x] = heatGrid[y][x] + random(160, 255);
              if (heatGrid[y][x] > 255) {
                heatGrid[y][x] = 255;
              }
            }
          }
        }
      }

      // Step 4. Convert heat to color and display
      // For left grid
      for (int y = 0; y < gridHeight; y++) {
        for (int x = 0; x < gridWidth; x++) {
          int pixelIndex = leftGrid[y][x];
          if (pixelIndex != -1) {
            uint8_t colorIndex = heatGrid[y][x];
            uint32_t color = HeatColor(colorIndex, pixels);
            pixels.setPixelColor(pixelIndex, color);
          }
        }
      }

      // For right grid
      for (int y = 0; y < gridHeight; y++) {
        for (int x = 0; x < gridWidth; x++) {
          int pixelIndex = rightGrid[y][x];
          if (pixelIndex != -1) {
            uint8_t colorIndex = heatGrid[y][x];
            uint32_t color = HeatColor(colorIndex, pixels);
            pixels.setPixelColor(pixelIndex, color);
          }
        }
      }

      pixels.show();
    }
    // Small delay to prevent CPU hogging
    delay(1);
  }

  // Turn off all pixels after exiting
  setAllNeoPixelsColor(pixels, pixels.Color(0, 0, 0));
  delay(50);  // Debounce delay
}

// Wheel function for rainbow colors
uint32_t Wheel(byte WheelPos, Adafruit_NeoPixel &pixels) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
    WheelPos -= 170;
    return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

/*
// Rainbow Cycle NeoPixel Demo
void rainbowCycleNeoPixelDemo(Adafruit_NeoPixel &pixels) {
  Serial.println("Rainbow Cycle NeoPixel Demo. Press LEFT button to exit.");

  uint16_t j = 0;
  unsigned long previousMillis = 0;
  const unsigned long interval = 20;

  while (!isLeftButtonPressed() && !animationInterrupted) {
    Watchdog.reset();
    unsigned long currentMillis = millis();

    // Check for both buttons pressed
    if (isBothButtonsPressed()) {
      handleBothButtonsPressed();
    }

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      for (int i = 0; i < pixels.numPixels(); i++) {
        pixels.setPixelColor(i, Wheel((i * 256 / pixels.numPixels() + j) & 255, pixels));
      }
      pixels.show();
      j++;
    }
    delay(1);
  }

  // Turn off all pixels after exiting
  setAllNeoPixelsColor(pixels, 0);
  delay(50);
}
*/
// Rainbow Cycle NeoPixel Demo with Short Press, Double Tap, and Long Press Detection
void rainbowCycleNeoPixelDemo(Adafruit_NeoPixel &pixels) {
  Serial.println("Rainbow Cycle NeoPixel Demo. Press LEFT button to exit.");

  uint16_t j = 0;
  unsigned long previousMillis = 0;
  const unsigned long interval = 20;

  // Variables for right button press detection
  bool rightButtonPressed = false;
  unsigned long pressStartTime = 0;
  bool longPressHandled = false;

  bool rightButtonLastState = HIGH;  // Assume button not pressed (pull-up resistor)

  while (!isLeftButtonPressed() && !animationInterrupted) {
    Watchdog.reset();
    unsigned long currentMillis = millis();

    // Check for both buttons pressed
    if (isBothButtonsPressed()) {
      handleBothButtonsPressed();
    }

    // Read the current state of the right button
    bool rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);

    if (rightButtonLastState == HIGH && rightButtonCurrentState == LOW) {
      // Button was just pressed
      delay(50);  // Debounce delay
      rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);
      if (rightButtonCurrentState == LOW) {
        rightButtonPressed = true;
        pressStartTime = millis();
        longPressHandled = false;
        Serial.println("Right button pressed.");
      }
    }

    if (rightButtonLastState == LOW && rightButtonCurrentState == HIGH) {
      // Button was just released
      if (rightButtonPressed) {
        unsigned long pressDuration = millis() - pressStartTime;
        if (pressDuration < SHORT_PRESS_THRESHOLD) {
          // Short press detected
          Serial.println("Short press detected. No change for this animation");
          // selectedColorIndex = (selectedColorIndex + 1) % numColors;
          // Serial.print("Color changed to index ");
          // Serial.println(selectedColorIndex);
        } else if (pressDuration >= LONG_PRESS_THRESHOLD && !longPressHandled) {
          // Long press detected upon release, only if not handled during holding
          Serial.println("Long press detected.");
          handleLongPress(animationIndex);  // Example function
        }
        rightButtonPressed = false;
      }
    }

    // Handle long press if button is still pressed and threshold exceeded
    if (rightButtonPressed && !longPressHandled) {
      unsigned long pressDuration = millis() - pressStartTime;
      if (pressDuration >= LONG_PRESS_THRESHOLD) {
        longPressHandled = true;
        Serial.println("Long press detected (while holding).");
        handleLongPress(animationIndex);  // Example function
      }
    }

    rightButtonLastState = rightButtonCurrentState;

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      for (int i = 0; i < pixels.numPixels(); i++) {
        pixels.setPixelColor(i, Wheel((i * 256 / pixels.numPixels() + j) & 255, pixels));
      }
      pixels.show();
      j++;
    }

    delay(1);
  }

  // Turn off all pixels after exiting
  setAllNeoPixelsColor(pixels, 0);
  delay(50);
}






// Bouncing Ball NeoPixel Demo with Short Press, Double Tap, and Long Press Detection
void bouncingBallNeoPixelDemo(Adafruit_NeoPixel &pixels) {
  Serial.println("Bouncing Ball NeoPixel Demo. Press LEFT button to exit.");

  int position = 0;
  int velocity = 1;

  int selectedColorIndex = 0;        // Start with the first color
  bool rightButtonLastState = HIGH;  // Assume button not pressed (pull-up resistor)

  unsigned long previousMillis = 0;
  const unsigned long interval = 20;

  // Variables for press duration detection
  bool rightButtonPressed = false;
  unsigned long pressStartTime = 0;
  bool longPressHandled = false;

  while (!isLeftButtonPressed() && !animationInterrupted) {
    Watchdog.reset();
    unsigned long currentMillis = millis();

    // Check for both buttons pressed
    if (isBothButtonsPressed()) {
      handleBothButtonsPressed();
    }

    // Read the current state of the right button
    bool rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);

    if (rightButtonLastState == HIGH && rightButtonCurrentState == LOW) {
      // Button was just pressed
      delay(50);  // Debounce delay
      rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);
      if (rightButtonCurrentState == LOW) {
        rightButtonPressed = true;
        pressStartTime = millis();
        longPressHandled = false;
        Serial.println("Right button pressed.");
      }
    }

    if (rightButtonLastState == LOW && rightButtonCurrentState == HIGH) {
      // Button was just released
      if (rightButtonPressed) {
        unsigned long pressDuration = millis() - pressStartTime;
        if (pressDuration < SHORT_PRESS_THRESHOLD) {
          // Short press detected
          Serial.println("Short press detected.");
          selectedColorIndex = (selectedColorIndex + 1) % numColors;
          Serial.print("Color changed to index ");
          Serial.println(selectedColorIndex);
        } else if (pressDuration >= LONG_PRESS_THRESHOLD && !longPressHandled) {
          // Long press detected upon release, only if not handled during holding
          Serial.println("Long press detected.");
          handleLongPress(animationIndex);  // Example function
        }
        rightButtonPressed = false;
      }
    }

    // Handle long press if button is still pressed and threshold exceeded
    if (rightButtonPressed && !longPressHandled) {
      unsigned long pressDuration = millis() - pressStartTime;
      if (pressDuration >= LONG_PRESS_THRESHOLD) {
        longPressHandled = true;
        Serial.println("Long press detected (while holding).");
        handleLongPress(animationIndex);  // Example function
      }
    }

    rightButtonLastState = rightButtonCurrentState;

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      setAllNeoPixelsColor(pixels, 0);
      uint8_t red = colorArray[selectedColorIndex][0];
      uint8_t green = colorArray[selectedColorIndex][1];
      uint8_t blue = colorArray[selectedColorIndex][2];
      pixels.setPixelColor(position, pixels.Color(red, green, blue));
      pixels.show();
      position += velocity;
      if (position == 0 || position == pixels.numPixels() - 1) {
        velocity = -velocity;
      }
    }
    delay(1);
  }

  // Turn off all pixels after exiting
  setAllNeoPixelsColor(pixels, 0);
  delay(50);
}








/*
// Bouncing Ball NeoPixel Demo with Double Tap Detection
void bouncingBallNeoPixelDemo(Adafruit_NeoPixel &pixels) {
  Serial.println("Bouncing Ball NeoPixel Demo. Press LEFT button to exit.");

  int position = 0;
  int velocity = 1;

  int selectedColorIndex = 0; // Start with the first color
  bool rightButtonLastState = HIGH; // Assume button not pressed (pull-up resistor)

  unsigned long previousMillis = 0;
  const unsigned long interval = 20;

  while (!isLeftButtonPressed() && !animationInterrupted) {
    Watchdog.reset();
    unsigned long currentMillis = millis();

    // Check for both buttons pressed
    if (isBothButtonsPressed()) {
      handleBothButtonsPressed();
    }

    // Check for right button press to change color
    bool rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);
    if (rightButtonLastState == HIGH && rightButtonCurrentState == LOW) {
      delay(50); // Debounce delay
      rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);
      if (rightButtonCurrentState == LOW) {
        // Button is pressed, change color
        selectedColorIndex = (selectedColorIndex + 1) % numColors;
        Serial.print("Color changed to index ");
        Serial.println(selectedColorIndex);
      }
    }
    rightButtonLastState = rightButtonCurrentState;

    // Tap Detection from Accelerometer
    //uint8_t tap = getAccelerometerTap(lis); // Ensure 'lis' is properly defined and initialized

    uint8_t click = lis.getClick();
  
    if (click & 0x30) { // Check if any tap is detected
      Serial.print("click detected: ");
      if (click & 0x10) {
        Serial.print("Single click");
        // If you want to handle single taps, you can add logic here
      }
      if (click & 0x20) {
        Serial.print("Double click");
        //saveanimationindex(); // Call your desired function on double tap
        Serial.println(" - saveanimationindex called.");
      }
      Serial.println();
    }

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      setAllNeoPixelsColor(pixels, 0);
      uint8_t red = colorArray[selectedColorIndex][0];
      uint8_t green = colorArray[selectedColorIndex][1];
      uint8_t blue = colorArray[selectedColorIndex][2];
      pixels.setPixelColor(position, pixels.Color(red, green, blue));
      pixels.show();
      position += velocity;
      if (position == 0 || position == pixels.numPixels() - 1) {
        velocity = -velocity;
      }
    }
    delay(1);
  }

  // Turn off all pixels after exiting
  setAllNeoPixelsColor(pixels, 0);
  delay(50);
}
*/

// Plasma Effect NeoPixel Demo
void plasmaEffectNeoPixelDemo(Adafruit_NeoPixel &pixels) {
  Serial.println("Plasma Effect NeoPixel Demo. Press LEFT button to exit.");

  int t = 0;
  unsigned long previousMillis = 0;
  const unsigned long interval = 50;

  // Variables for right button press detection
  bool rightButtonPressed = false;
  unsigned long pressStartTime = 0;
  bool longPressHandled = false;

  bool rightButtonLastState = HIGH;  // Assume button not pressed (pull-up resistor)

  while (!isLeftButtonPressed() && !animationInterrupted) {
    Watchdog.reset();
    unsigned long currentMillis = millis();

    // Check for both buttons pressed
    if (isBothButtonsPressed()) {
      handleBothButtonsPressed();
    }

    // Read the current state of the right button
    bool rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);

    if (rightButtonLastState == HIGH && rightButtonCurrentState == LOW) {
      // Button was just pressed
      delay(50);  // Debounce delay
      rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);
      if (rightButtonCurrentState == LOW) {
        rightButtonPressed = true;
        pressStartTime = millis();
        longPressHandled = false;
        Serial.println("Right button pressed.");
      }
    }

    if (rightButtonLastState == LOW && rightButtonCurrentState == HIGH) {
      // Button was just released
      if (rightButtonPressed) {
        unsigned long pressDuration = millis() - pressStartTime;
        if (pressDuration < SHORT_PRESS_THRESHOLD) {
          // Short press detected
          Serial.println("Short press detected. No change for this animation");
          // selectedColorIndex = (selectedColorIndex + 1) % numColors;
          // Serial.print("Color changed to index ");
          // Serial.println(selectedColorIndex);
        } else if (pressDuration >= LONG_PRESS_THRESHOLD && !longPressHandled) {
          // Long press detected upon release, only if not handled during holding
          Serial.println("Long press detected.");
          handleLongPress(animationIndex);  // Example function
        }
        rightButtonPressed = false;
      }
    }

    // Handle long press if button is still pressed and threshold exceeded
    if (rightButtonPressed && !longPressHandled) {
      unsigned long pressDuration = millis() - pressStartTime;
      if (pressDuration >= LONG_PRESS_THRESHOLD) {
        longPressHandled = true;
        Serial.println("Long press detected (while holding).");
        handleLongPress(animationIndex);  // Example function
      }
    }

    rightButtonLastState = rightButtonCurrentState;

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      for (int i = 0; i < pixels.numPixels(); i++) {
        int x = i % 5;
        int y = i / 5;
        uint8_t color = (uint8_t)(128.0 + (128.0 * sin(i + t / 7.0)));
        pixels.setPixelColor(i, pixels.Color(color, 0, 255 - color));
      }
      pixels.show();
      t++;
    }
    delay(1);
  }

  // Turn off all pixels after exiting
  setAllNeoPixelsColor(pixels, 0);
  delay(50);
}

// Cyberpunk Glitch NeoPixel Demo
void cyberpunkGlitchNeoPixelDemo(Adafruit_NeoPixel &pixels) {
  Serial.println("Cyberpunk Glitch NeoPixel Demo. Press LEFT button to exit.");

  int selectedColorIndex = 0;  // Start with the first color
  // Variables for right button press detection
  bool rightButtonPressed = false;
  unsigned long pressStartTime = 0;
  bool longPressHandled = false;

  bool rightButtonLastState = HIGH;  // Assume button not pressed (pull-up resistor)

  while (!isLeftButtonPressed() && !animationInterrupted) {
    Watchdog.reset();
    unsigned long currentMillis = millis();

    // Check for both buttons pressed
    if (isBothButtonsPressed()) {
      handleBothButtonsPressed();
    }

    // Read the current state of the right button
    bool rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);

    if (rightButtonLastState == HIGH && rightButtonCurrentState == LOW) {
      // Button was just pressed
      delay(50);  // Debounce delay
      rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);
      if (rightButtonCurrentState == LOW) {
        rightButtonPressed = true;
        pressStartTime = millis();
        longPressHandled = false;
        Serial.println("Right button pressed.");
      }
    }

    if (rightButtonLastState == LOW && rightButtonCurrentState == HIGH) {
      // Button was just released
      if (rightButtonPressed) {
        unsigned long pressDuration = millis() - pressStartTime;
        if (pressDuration < SHORT_PRESS_THRESHOLD) {
          // Short press detected
          Serial.println("Short press detected.");
          selectedColorIndex = (selectedColorIndex + 1) % numColors;
          Serial.print("Color changed to index ");
          Serial.println(selectedColorIndex);
        } else if (pressDuration >= LONG_PRESS_THRESHOLD && !longPressHandled) {
          // Long press detected upon release, only if not handled during holding
          Serial.println("Long press detected.");
          handleLongPress(animationIndex);  // Example function
        }
        rightButtonPressed = false;
      }
    }

    // Handle long press if button is still pressed and threshold exceeded
    if (rightButtonPressed && !longPressHandled) {
      unsigned long pressDuration = millis() - pressStartTime;
      if (pressDuration >= LONG_PRESS_THRESHOLD) {
        longPressHandled = true;
        Serial.println("Long press detected (while holding).");
        handleLongPress(animationIndex);  // Example function
      }
    }

    rightButtonLastState = rightButtonCurrentState;

    int glitchPixel = random(0, pixels.numPixels());
    uint8_t red = colorArray[selectedColorIndex][0];
    uint8_t green = colorArray[selectedColorIndex][1];
    uint8_t blue = colorArray[selectedColorIndex][2];
    uint32_t glitchColor = pixels.Color(red, green, blue);
    pixels.setPixelColor(glitchPixel, glitchColor);
    pixels.show();
    delay(random(50, 200));
    pixels.setPixelColor(glitchPixel, 0);

    // Small delay to allow for button checking
    delay(1);
  }

  // Turn off all pixels after exiting
  setAllNeoPixelsColor(pixels, 0);
  delay(50);
}

// Cyberpunk Circuit NeoPixel Demo
void cyberpunkCircuitNeoPixelDemo(Adafruit_NeoPixel &pixels) {
  Serial.println("Cyberpunk Circuit NeoPixel Demo. Press LEFT button to exit.");

  int index = 0;

  int selectedColorIndex1 = 0;  // Start with the first color
  int selectedColorIndex2 = 1;  // Start with the second color
  //bool rightButtonLastState = HIGH; // Assume button not pressed (pull-up resistor)

  unsigned long previousMillis = 0;
  const unsigned long interval = 100;
  // Variables for right button press detection

  bool rightButtonPressed = false;
  unsigned long pressStartTime = 0;
  bool longPressHandled = false;

  bool rightButtonLastState = HIGH;  // Assume button not pressed (pull-up resistor)

  while (!isLeftButtonPressed() && !animationInterrupted) {
    Watchdog.reset();
    unsigned long currentMillis = millis();

    // Check for both buttons pressed
    if (isBothButtonsPressed()) {
      handleBothButtonsPressed();
    }

    // Read the current state of the right button
    bool rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);

    if (rightButtonLastState == HIGH && rightButtonCurrentState == LOW) {
      // Button was just pressed
      delay(50);  // Debounce delay
      rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);
      if (rightButtonCurrentState == LOW) {
        rightButtonPressed = true;
        pressStartTime = millis();
        longPressHandled = false;
        Serial.println("Right button pressed.");
      }
    }

    if (rightButtonLastState == LOW && rightButtonCurrentState == HIGH) {
      // Button was just released
      if (rightButtonPressed) {
        unsigned long pressDuration = millis() - pressStartTime;
        if (pressDuration < SHORT_PRESS_THRESHOLD) {
          // Short press detected
          Serial.println("Short press detected.");
          // Button is pressed, change colors
          selectedColorIndex1 = (selectedColorIndex1 + 1) % numColors;
          selectedColorIndex2 = (selectedColorIndex2 + 1) % numColors;
          Serial.print("Colors changed to indexes ");
          Serial.print(selectedColorIndex1);
          Serial.print(" and ");
          Serial.println(selectedColorIndex2);
        } else if (pressDuration >= LONG_PRESS_THRESHOLD && !longPressHandled) {
          // Long press detected upon release, only if not handled during holding
          Serial.println("Long press detected.");
          handleLongPress(animationIndex);  // Example function
        }
        rightButtonPressed = false;
      }
    }

    // Handle long press if button is still pressed and threshold exceeded
    if (rightButtonPressed && !longPressHandled) {
      unsigned long pressDuration = millis() - pressStartTime;
      if (pressDuration >= LONG_PRESS_THRESHOLD) {
        longPressHandled = true;
        Serial.println("Long press detected (while holding).");
        handleLongPress(animationIndex);  // Example function
      }
    }

    rightButtonLastState = rightButtonCurrentState;

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      uint8_t bgRed = colorArray[selectedColorIndex1][0];
      uint8_t bgGreen = colorArray[selectedColorIndex1][1];
      uint8_t bgBlue = colorArray[selectedColorIndex1][2];

      uint8_t lineRed = colorArray[selectedColorIndex2][0];
      uint8_t lineGreen = colorArray[selectedColorIndex2][1];
      uint8_t lineBlue = colorArray[selectedColorIndex2][2];

      setAllNeoPixelsColor(pixels, pixels.Color(bgRed / 5, bgGreen / 5, bgBlue / 5));  // Dim background

      for (int i = 0; i < 5; i++) {
        int pixel = (index + i * 10) % pixels.numPixels();
        pixels.setPixelColor(pixel, pixels.Color(lineRed, lineGreen, lineBlue));  // Bright line color
      }
      pixels.show();
      index = (index + 1) % pixels.numPixels();
    }
    delay(1);
  }

  // Turn off all pixels after exiting
  setAllNeoPixelsColor(pixels, 0);
  delay(50);
}

// Accelerometer NeoPixel Demo
void accelerometerNeoPixelDemo(Adafruit_NeoPixel &pixels) {
  // Define the grid mappings (same as before)
  const uint8_t gridWidth = 5;
  const uint8_t gridHeight = 5;

  Serial.println("Accelerometer NeoPixel Demo. Press LEFT button to exit.");

  int selectedColorIndex = 0;  // Start with the first color
  // Variables for right button press detection
  bool rightButtonPressed = false;
  unsigned long pressStartTime = 0;
  bool longPressHandled = false;

  bool rightButtonLastState = HIGH;  // Assume button not pressed (pull-up resistor)

  while (!isLeftButtonPressed() && !animationInterrupted) {
    Watchdog.reset();
    unsigned long currentMillis = millis();

    // Check for both buttons pressed
    if (isBothButtonsPressed()) {
      handleBothButtonsPressed();
    }

    // Read the current state of the right button
    bool rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);

    if (rightButtonLastState == HIGH && rightButtonCurrentState == LOW) {
      // Button was just pressed
      delay(50);  // Debounce delay
      rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);
      if (rightButtonCurrentState == LOW) {
        rightButtonPressed = true;
        pressStartTime = millis();
        longPressHandled = false;
        Serial.println("Right button pressed.");
      }
    }

    if (rightButtonLastState == LOW && rightButtonCurrentState == HIGH) {
      // Button was just released
      if (rightButtonPressed) {
        unsigned long pressDuration = millis() - pressStartTime;
        if (pressDuration < SHORT_PRESS_THRESHOLD) {
          // Short press detected
          Serial.println("Short press detected. No change for this animation");
          selectedColorIndex = (selectedColorIndex + 1) % numColors;
          Serial.print("Color changed to index ");
          Serial.println(selectedColorIndex);
        } else if (pressDuration >= LONG_PRESS_THRESHOLD && !longPressHandled) {
          // Long press detected upon release, only if not handled during holding
          Serial.println("Long press detected.");
          handleLongPress(animationIndex);  // Example function
        }
        rightButtonPressed = false;
      }
    }

    // Handle long press if button is still pressed and threshold exceeded
    if (rightButtonPressed && !longPressHandled) {
      unsigned long pressDuration = millis() - pressStartTime;
      if (pressDuration >= LONG_PRESS_THRESHOLD) {
        longPressHandled = true;
        Serial.println("Long press detected (while holding).");
        handleLongPress(animationIndex);  // Example function
      }
    }

    rightButtonLastState = rightButtonCurrentState;

    float x, y, z;
    getAccelerometerValues(lis, x, y, z);

    // Map x and y values to grid positions (0 to 4)
    int gridX = map(x * 100, -100, 100, 0, 4);
    int gridY = map(y * 100, -100, 100, 0, 4);  // Inverted Y mapping

    // Constrain to grid
    gridX = constrain(gridX, 0, 4);
    gridY = constrain(gridY, 0, 4);

    // Get pixel indices
    int leftPixel = leftGrid[gridY][gridX];
    int rightPixel = rightGrid[gridY][gridX];

    // Clear all pixels
    setAllNeoPixelsColor(pixels, pixels.Color(0, 0, 0));

    // Set the pixels
    if (leftPixel != -1) {
      uint8_t red = colorArray[selectedColorIndex][0];
      uint8_t green = colorArray[selectedColorIndex][1];
      uint8_t blue = colorArray[selectedColorIndex][2];
      pixels.setPixelColor(leftPixel, pixels.Color(red, green, blue));
    }
    if (rightPixel != -1) {
      uint8_t red = colorArray[selectedColorIndex][0];
      uint8_t green = colorArray[selectedColorIndex][1];
      uint8_t blue = colorArray[selectedColorIndex][2];
      pixels.setPixelColor(rightPixel, pixels.Color(red, green, blue));
    }

    pixels.show();

    // Small delay to allow for button checking
    delay(10);
  }

  // Turn off all pixels after exiting
  setAllNeoPixelsColor(pixels, pixels.Color(0, 0, 0));
  delay(50);  // Debounce delay
}


// Eyeball NeoPixel Demo
void eyeballNeoPixelDemo(Adafruit_NeoPixel &pixels) {
  Serial.println("Eyeball NeoPixel Demo. Press LEFT button to exit.");

  const uint8_t gridWidth = 5;
  const uint8_t gridHeight = 5;

  // Eyeball positions for movement
  int positions[][2] = {
    { 2, 2 }, { 1, 2 }, { 3, 2 }, { 2, 1 }, { 2, 3 }, { 1, 1 }, { 3, 1 }, { 1, 3 }, { 3, 3 }
  };
  int numPositions = sizeof(positions) / sizeof(positions[0]);
  int index = 0;

  // Shape index for changing shapes
  int shapeIndex = 0;  // Start with the original shape
  const int numShapes = 3;

  // bool rightButtonLastState = HIGH; // Assume button not pressed (pull-up resistor)

  unsigned long previousMillis = 0;
  const unsigned long interval = 500;
  // Variables for right button press detection
  bool rightButtonPressed = false;
  unsigned long pressStartTime = 0;
  bool longPressHandled = false;

  bool rightButtonLastState = HIGH;  // Assume button not pressed (pull-up resistor)

  while (!isLeftButtonPressed() && !animationInterrupted) {
    Watchdog.reset();
    unsigned long currentMillis = millis();

    // Check for both buttons pressed
    if (isBothButtonsPressed()) {
      handleBothButtonsPressed();
    }

    // Read the current state of the right button
    bool rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);

    if (rightButtonLastState == HIGH && rightButtonCurrentState == LOW) {
      // Button was just pressed
      delay(50);  // Debounce delay
      rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);
      if (rightButtonCurrentState == LOW) {
        rightButtonPressed = true;
        pressStartTime = millis();
        longPressHandled = false;
        Serial.println("Right button pressed.");
      }
    }

    if (rightButtonLastState == LOW && rightButtonCurrentState == HIGH) {
      // Button was just released
      if (rightButtonPressed) {
        unsigned long pressDuration = millis() - pressStartTime;
        if (pressDuration < SHORT_PRESS_THRESHOLD) {
          // Short press detected
          Serial.println("Short press detected.");
          // Button is pressed, change shape
          shapeIndex = (shapeIndex + 1) % numShapes;
          Serial.print("Shape changed to index ");
          Serial.println(shapeIndex);
        } else if (pressDuration >= LONG_PRESS_THRESHOLD && !longPressHandled) {
          // Long press detected upon release, only if not handled during holding
          Serial.println("Long press detected.");
          handleLongPress(animationIndex);  // Example function
        }
        rightButtonPressed = false;
      }
    }

    // Handle long press if button is still pressed and threshold exceeded
    if (rightButtonPressed && !longPressHandled) {
      unsigned long pressDuration = millis() - pressStartTime;
      if (pressDuration >= LONG_PRESS_THRESHOLD) {
        longPressHandled = true;
        Serial.println("Long press detected (while holding).");
        handleLongPress(animationIndex);  // Example function
      }
    }

    rightButtonLastState = rightButtonCurrentState;

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      int x = positions[index][0];
      int y = positions[index][1];

      // Clear all pixels
      setAllNeoPixelsColor(pixels, pixels.Color(0, 0, 0));

      // Draw the selected shape based on shapeIndex
      switch (shapeIndex) {
        case 0:
          // Original Eyeball Shape
          drawEyeballShape(pixels, x, y);
          break;
        case 1:
          // Hole in the center with LEDs on top, left, right, and bottom
          drawCrossShape(pixels, x, y);
          break;
        case 2:
          // Single Dot
          drawSingleDot(pixels, x, y);
          break;
      }

      pixels.show();

      index = (index + 1) % numPositions;
    }
    delay(1);
  }

  // Turn off all pixels after exiting
  setAllNeoPixelsColor(pixels, pixels.Color(0, 0, 0));
  delay(50);
}


// Draw the original eyeball shape
void drawEyeballShape(Adafruit_NeoPixel &pixels, int x, int y) {
  const uint8_t gridWidth = 5;
  const uint8_t gridHeight = 5;

  // Draw eyeball on both grids
  // White background
  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      int nx = x + dx;
      int ny = y + dy;
      if (nx >= 0 && nx < gridWidth && ny >= 0 && ny < gridHeight) {
        int lp = leftGrid[ny][nx];
        int rp = rightGrid[ny][nx];
        if (lp != -1) pixels.setPixelColor(lp, pixels.Color(255, 255, 255));
        if (rp != -1) pixels.setPixelColor(rp, pixels.Color(255, 255, 255));
      }
    }
  }

  // Black pupil
  int leftPixel = leftGrid[y][x];
  int rightPixel = rightGrid[y][x];
  if (leftPixel != -1) pixels.setPixelColor(leftPixel, pixels.Color(0, 0, 0));
  if (rightPixel != -1) pixels.setPixelColor(rightPixel, pixels.Color(0, 0, 0));
}

// Draw cross shape (hole in center, LEDs on top, left, right, bottom)
void drawCrossShape(Adafruit_NeoPixel &pixels, int x, int y) {
  const uint8_t gridWidth = 5;
  const uint8_t gridHeight = 5;

  // Positions relative to center
  int positions[][2] = {
    { 0, -1 },  // Top
    { -1, 0 },  // Left
    { 1, 0 },   // Right
    { 0, 1 }    // Bottom
  };

  for (int i = 0; i < 4; i++) {
    int nx = x + positions[i][0];
    int ny = y + positions[i][1];
    if (nx >= 0 && nx < gridWidth && ny >= 0 && ny < gridHeight) {
      int lp = leftGrid[ny][nx];
      int rp = rightGrid[ny][nx];
      if (lp != -1) pixels.setPixelColor(lp, pixels.Color(255, 255, 255));
      if (rp != -1) pixels.setPixelColor(rp, pixels.Color(255, 255, 255));
    }
  }
}

// Draw single dot at the center
void drawSingleDot(Adafruit_NeoPixel &pixels, int x, int y) {
  int leftPixel = leftGrid[y][x];
  int rightPixel = rightGrid[y][x];
  if (leftPixel != -1) pixels.setPixelColor(leftPixel, pixels.Color(255, 255, 255));
  if (rightPixel != -1) pixels.setPixelColor(rightPixel, pixels.Color(255, 255, 255));
}


// Color Swirl NeoPixel Demo
void colorSwirlNeoPixelDemo(Adafruit_NeoPixel &pixels) {
  Serial.println("Color Swirl NeoPixel Demo. Press LEFT button to exit.");

  int hue = 0;
  unsigned long previousMillis = 0;
  const unsigned long interval = 20;

  // Variables for right button press detection
  bool rightButtonPressed = false;
  unsigned long pressStartTime = 0;
  bool longPressHandled = false;

  bool rightButtonLastState = HIGH;  // Assume button not pressed (pull-up resistor)

  while (!isLeftButtonPressed() && !animationInterrupted) {
    Watchdog.reset();
    unsigned long currentMillis = millis();

    // Check for both buttons pressed
    if (isBothButtonsPressed()) {
      handleBothButtonsPressed();
    }

    // Read the current state of the right button
    bool rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);

    if (rightButtonLastState == HIGH && rightButtonCurrentState == LOW) {
      // Button was just pressed
      delay(50);  // Debounce delay
      rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);
      if (rightButtonCurrentState == LOW) {
        rightButtonPressed = true;
        pressStartTime = millis();
        longPressHandled = false;
        Serial.println("Right button pressed.");
      }
    }

    if (rightButtonLastState == LOW && rightButtonCurrentState == HIGH) {
      // Button was just released
      if (rightButtonPressed) {
        unsigned long pressDuration = millis() - pressStartTime;
        if (pressDuration < SHORT_PRESS_THRESHOLD) {
          // Short press detected
          Serial.println("Short press detected. No change for this animation");
          // selectedColorIndex = (selectedColorIndex + 1) % numColors;
          // Serial.print("Color changed to index ");
          // Serial.println(selectedColorIndex);
        } else if (pressDuration >= LONG_PRESS_THRESHOLD && !longPressHandled) {
          // Long press detected upon release, only if not handled during holding
          Serial.println("Long press detected.");
          handleLongPress(animationIndex);  // Example function
        }
        rightButtonPressed = false;
      }
    }

    // Handle long press if button is still pressed and threshold exceeded
    if (rightButtonPressed && !longPressHandled) {
      unsigned long pressDuration = millis() - pressStartTime;
      if (pressDuration >= LONG_PRESS_THRESHOLD) {
        longPressHandled = true;
        Serial.println("Long press detected (while holding).");
        handleLongPress(animationIndex);  // Example function
      }
    }

    rightButtonLastState = rightButtonCurrentState;

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      for (int i = 0; i < pixels.numPixels(); i++) {
        pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV((hue + i * 65536 / pixels.numPixels()) % 65536)));
      }
      pixels.show();
      hue += 256;  // Adjust for speed
    }
    delay(1);
  }

  // Turn off all pixels after exiting
  setAllNeoPixelsColor(pixels, 0);
  delay(50);
}

// Function to test sensors (functionality kept but not used in main sketch)
void testSensors(Adafruit_LIS3DH &lis) {
  Serial.println("Testing sensors for 5 seconds.");
  unsigned long startTime = millis();
  while (millis() - startTime < 5000) {
    Watchdog.reset();
    // Magnetic sensor reading
    if (isMagneticFieldDetected()) {
      Serial.println("Magnetic field detected!");
    } else {
      Serial.println("No magnetic field detected.");
    }

    // Microphone reading
    int micValue = analogRead(A6);  // Read the microphone analog value
    Serial.print("Microphone reading: ");
    Serial.println(micValue);

    // Accelerometer readings
    float x, y, z;
    getAccelerometerValues(lis, x, y, z);
    Serial.print("Accelerometer X: ");
    Serial.print(x);
    Serial.print(" Y: ");
    Serial.print(y);
    Serial.print(" Z: ");
    Serial.println(z);

    // Tap detection
    uint8_t tap = getAccelerometerTap(lis);
    if (tap & 0x30) {
      Serial.print("Tap detected: ");
      if (tap & 0x10) Serial.print("Single tap");
      if (tap & 0x20) Serial.print("Double tap");
      Serial.println();
    }

    delay(500);
  }
}

// Function to test NeoPixels (functionality kept but not used in main sketch)
void testNeoPixels(Adafruit_NeoPixel &pixels) {
  Watchdog.reset();
  Serial.println("Testing NeoPixels.");

  setAllNeoPixelsColor(pixels, pixels.Color(255, 0, 0));  // Set to red
  Serial.println("NeoPixels set to red. Press LEFT button to proceed.");
  waitForButtonPress(isLeftButtonPressed);

  setAllNeoPixelsColor(pixels, pixels.Color(0, 255, 0));  // Set to green
  Serial.println("NeoPixels set to green. Press LEFT button to proceed.");
  waitForButtonPress(isLeftButtonPressed);

  setAllNeoPixelsColor(pixels, pixels.Color(0, 0, 255));  // Set to blue
  Serial.println("NeoPixels set to blue. Press LEFT button to proceed.");
  waitForButtonPress(isLeftButtonPressed);

  setAllNeoPixelsColor(pixels, pixels.Color(255, 255, 255));  // Set to white
  Serial.println("NeoPixels set to white. Press LEFT button to proceed.");
  waitForButtonPress(isLeftButtonPressed);

  setAllNeoPixelsColor(pixels, pixels.Color(0, 0, 0));  // Turn off
  Serial.println("NeoPixels turned off.");
}

// Function to record audio from the PDM microphone
void recordAudio() {
  for (int i = 0; i < FFT_SIZE; i++) {
    uint32_t runningsum = 0;
    const uint16_t *sinc_ptr = sincfilter;

    // Read DECIMATION bits and perform convolution
    for (uint8_t samplenum = 0; samplenum < (DECIMATION / 16); samplenum++) {
      uint16_t sample = pdm.read() & 0xFFFF;  // Read 16 bits

      // Process each bit
      for (int b = 0; b < 16; b++) {
        if (sample & 0x1) {
          runningsum += *sinc_ptr;
        }
        sinc_ptr++;
        sample >>= 1;
      }
    }

    // Normalize the running sum
    runningsum /= DECIMATION;

    // Store the PCM sample
    pcm_buffer[i] = (int16_t)runningsum - 32768;  // Center around zero
  }
}

bool recordAudioWithButtonCheck() {
  for (int i = 0; i < FFT_SIZE; i++) {
    // Check for left button press
    if (isLeftButtonPressed()) {
      return false;  // Exit recording early
    }
    uint32_t runningsum = 0;
    const uint16_t *sinc_ptr = sincfilter;

    // Read DECIMATION bits and perform convolution
    for (uint8_t samplenum = 0; samplenum < (DECIMATION / 16); samplenum++) {
      uint16_t sample = pdm.read() & 0xFFFF;  // Read 16 bits

      // Process each bit
      for (int b = 0; b < 16; b++) {
        if (sample & 0x1) {
          runningsum += *sinc_ptr;
        }
        sinc_ptr++;
        sample >>= 1;
      }
    }

    // Normalize the running sum
    runningsum /= DECIMATION;

    // Store the PCM sample
    pcm_buffer[i] = (int16_t)runningsum - 32768;  // Center around zero
  }
  return true;
}


// Function to perform FFT on the recorded audio
void processFFT() {
  // Perform FFT on the PCM data
  ZeroFFT(pcm_buffer, FFT_SIZE);

  // Compute magnitude spectrum (log scale)
  for (int i = 1; i < SPECTRUM_SIZE - 1; i++) {  // Start from 1 to exclude DC component
    float real = pcm_buffer[i * 2];
    float imag = pcm_buffer[i * 2 + 1];
    float mag = sqrt(real * real + imag * imag);
    spectrum[i] = log(mag + 1e-7);  // Add a small value to prevent log(0)
  }

  // Find min and max values in the spectrum
  float min_curr = spectrum[1];
  float max_curr = spectrum[1];
  for (int i = 1; i < SPECTRUM_SIZE - 1; i++) {
    if (spectrum[i] < min_curr) min_curr = spectrum[i];
    if (spectrum[i] > max_curr) max_curr = spectrum[i];
  }

  // Update max_all for dynamic scaling
  if (max_curr > max_all) {
    max_all = max_curr;
  } else {
    max_all = max_all * 0.95 + max_curr * 0.05;  // Smoother decay
  }

  // Optionally, lower the min_curr threshold to allow for more sensitivity
  min_curr = 0.0;  // Removed the previous threshold of 3.0

  // Normalize and scale the spectrum data
  for (int i = 1; i < SPECTRUM_SIZE - 1; i++) {
    spectrum[i] = (spectrum[i] - min_curr) * (15.0 / (max_all - min_curr));
    if (spectrum[i] < 0) spectrum[i] = 0;
    if (spectrum[i] > 15.0) spectrum[i] = 15.0;  // Cap at 15
  }
}

// Function to calculate overall volume from the spectrum
float calculateVolume() {
  float volume = 0.0;
  for (int i = 1; i < SPECTRUM_SIZE - 1; i++) {
    volume += spectrum[i];
  }
  volume /= (SPECTRUM_SIZE - 2);  // Average magnitude
  return volume;
}

// Function to display the solid color animation with brightness scaling
void displaySolidColor(Adafruit_NeoPixel &pixels, uint32_t selectedColor) {
  // Determine RGB components from selectedColor
  uint8_t red = (selectedColor >> 16) & 0xFF;
  uint8_t green = (selectedColor >> 8) & 0xFF;
  uint8_t blue = selectedColor & 0xFF;

  // Adjust brightness based on brightnessFactor
  uint8_t adjustedRed = (uint8_t)(red * brightnessFactor);
  uint8_t adjustedGreen = (uint8_t)(green * brightnessFactor);
  uint8_t adjustedBlue = (uint8_t)(blue * brightnessFactor);

  // Create the adjusted color
  uint32_t adjustedColor = pixels.Color(adjustedRed, adjustedGreen, adjustedBlue);

  // Set each pixel to the adjusted color
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, adjustedColor);
  }

  // Update the NeoPixel strip
  pixels.show();
}

// Function to display the rainbow animation with brightness and speed scaling
void displayRainbow(Adafruit_NeoPixel &pixels) {
  // Determine hue increment based on current BPM
  // Map BPM to hue increment: slower BPM -> lower increment, faster BPM -> higher increment
  // Assuming BPM ranges from 60 to 180
  float bpmClamped = constrain(currentBPM, MIN_BPM, MAX_BPM);
  float bpmRatio = (bpmClamped - MIN_BPM) / (MAX_BPM - MIN_BPM);  // 0.0 to 1.0
  uint16_t hueIncrement = MIN_HUE_INCREMENT + bpmRatio * (MAX_HUE_INCREMENT - MIN_HUE_INCREMENT);

  // Increment the global hue based on the calculated hue increment
  globalHue += hueIncrement;
  if (globalHue > 65535) {
    globalHue = 0;
  }

  // Loop through each pixel and set its color based on the rainbow
  for (int i = 0; i < NUMPIXELS; i++) {
    // Calculate hue for this pixel
    uint16_t hue = (globalHue + (i * 65535UL / NUMPIXELS)) % 65536;

    // Define saturation and value based on brightnessFactor
    uint8_t saturation = 255;
    uint8_t value = brightnessFactor * 255;  // Scale brightness

    // Convert HSV to RGB
    uint32_t color = pixels.ColorHSV(hue, saturation, value);

    // Set the pixel color
    pixels.setPixelColor(i, color);
  }

  // Update the NeoPixel strip
  pixels.show();
}


void solidColorMusic(Adafruit_NeoPixel &pixels) {
  Serial.println("Sound Effect NeoPixel Demo. Press LEFT button to exit.");

  int selectedColorIndex = 0;
  int fadeColorIndex = random(numColors);
  bool fadingUp = true;
  uint8_t fadeBrightness = 0;
  bool fadeActive = true;

  bool rightButtonPressed = false;
  unsigned long pressStartTime = 0;
  bool longPressHandled = false;
  bool rightButtonLastState = HIGH;

  while (!isLeftButtonPressed() && !animationInterrupted) {
    Watchdog.reset();

    // Check for buttons
    if (isBothButtonsPressed()) {
      handleBothButtonsPressed();
    }

    // Read audio & process FFT
    if (!recordAudioWithButtonCheck()) break;
    processFFT();
    float volume = calculateVolume();

    if (volume < VOLUME_THRESHOLD) {
      fadeActive = true;
      if (fadingUp) {
        fadeBrightness = min(fadeBrightness + 5, 255);
        if (fadeBrightness >= 255) fadingUp = false;
      } else {
        fadeBrightness = max(fadeBrightness - 5, 0);
        if (fadeBrightness == 0) {
          fadingUp = true;
          fadeColorIndex = random(numColors);
        }
      }

      uint8_t red = (colorArray[fadeColorIndex][0] * fadeBrightness) / 255;
      uint8_t green = (colorArray[fadeColorIndex][1] * fadeBrightness) / 255;
      uint8_t blue = (colorArray[fadeColorIndex][2] * fadeBrightness) / 255;
      setAllNeoPixelsColor(pixels, pixels.Color(red, green, blue));
    } else {
      fadeActive = false;
      float brightnessFactor = constrain((volume - VOLUME_THRESHOLD) / (15.0 - VOLUME_THRESHOLD), 0.0, 1.0);
      uint8_t red = colorArray[selectedColorIndex][0] * brightnessFactor;
      uint8_t green = colorArray[selectedColorIndex][1] * brightnessFactor;
      uint8_t blue = colorArray[selectedColorIndex][2] * brightnessFactor;
      displaySolidColor(pixels, pixels.Color(red, green, blue));
    }
    delay(20);
  }

  setAllNeoPixelsColor(pixels, 0);
  fadeActive = true;  // Ensure fade resets for next use
  delay(50);
}

void rainbowBeatMusic(Adafruit_NeoPixel &pixels) {
  Serial.println("Rainbow Beat NeoPixel Demo. Press LEFT button to exit.");

  int fadeColorIndex = random(numColors);
  bool fadingUp = true;
  uint8_t fadeBrightness = 0;
  bool fadeActive = true;

  while (!isLeftButtonPressed() && !animationInterrupted) {
    Watchdog.reset();

    // Check for button presses
    if (isBothButtonsPressed()) {
      handleBothButtonsPressed();
    }

    // Read audio & process FFT
    if (!recordAudioWithButtonCheck()) break;
    processFFT();
    float volume = calculateVolume();

    if (volume < VOLUME_THRESHOLD) {
      fadeActive = true;
      if (fadingUp) {
        fadeBrightness = min(fadeBrightness + 5, 255);
        if (fadeBrightness >= 255) fadingUp = false;
      } else {
        fadeBrightness = max(fadeBrightness - 5, 0);
        if (fadeBrightness == 0) {
          fadingUp = true;
          fadeColorIndex = random(numColors);
        }
      }

      uint8_t red = (colorArray[fadeColorIndex][0] * fadeBrightness) / 255;
      uint8_t green = (colorArray[fadeColorIndex][1] * fadeBrightness) / 255;
      uint8_t blue = (colorArray[fadeColorIndex][2] * fadeBrightness) / 255;
      setAllNeoPixelsColor(pixels, pixels.Color(red, green, blue));
    } else {
      fadeActive = false;
      float brightnessFactor = constrain((volume - VOLUME_THRESHOLD) / (15.0 - VOLUME_THRESHOLD), 0.0, 1.0);

      if (volume > VOLUME_THRESHOLD * 1.5) {
        unsigned long currentTime = millis();
        if (lastBeatTime != 0) {
          unsigned long timeSinceLastBeat = currentTime - lastBeatTime;
          float bpm = 60000.0 / timeSinceLastBeat;
          if (bpm >= MIN_BPM && bpm <= MAX_BPM) {
            currentBPM = BPM_SMOOTHING * currentBPM + (1.0 - BPM_SMOOTHING) * bpm;
          }
        }
        lastBeatTime = millis();
      }

      displayRainbow(pixels);
    }
    delay(20);
  }

  setAllNeoPixelsColor(pixels, 0);
  fadeActive = true;
  delay(50);
}


void handleBothButtonsPressed() {
  static unsigned long lastBothButtonsTime = 0;
  static bool bothButtonsLock = false;
  
  // Check if this is a fresh press (debounce and prevent continuous triggering)
  unsigned long currentTime = millis();
  if (currentTime - lastBothButtonsTime < 300) {
    return; // Debounce
  }
  
  if (!bothButtonsLock && isBothButtonsPressed()) {
    Watchdog.reset();
    bothButtonsLock = true;
    lastBothButtonsTime = currentTime;
    
    // Store the current animation to resume later
    previousAnimation = currentAnimation;
    currentAnimation = NULL;  // Stop current animation
    
    // Check if special mode is active
    bool useFullRange = isSpecialModeActive();
    
    if (useFullRange) {
      Serial.println("BOTH BUTTONS PRESSED IN SPECIAL MODE!");
      Serial.println("Using full card range (1-78)");
    } else {
      Serial.println("BOTH BUTTONS PRESSED!");
      Serial.println("Using limited card range (1-38)");
    }

    // Initialize random seed
    randomSeed(analogRead(A3));

    // Visual feedback
    turnOnAllLEDs();
    setAllNeoPixelsColor(pixels, pixels.Color(255, 255, 255));
    pixels.show();

    // NFC operations
    nfcWriter.wipeEEPROM();
    nfcWriter.writeCCFile();
    nfcWriter.writeRandomURI(useFullRange);

    Serial.println("NFC tag written.");
    Watchdog.reset();

    // Wait for visual feedback
    delay(2000);

    // Clean up
    turnOffAllLEDs();
    setAllNeoPixelsColor(pixels, 0);
    pixels.show();

    // Resume previous animation
    currentAnimation = previousAnimation;
    
    // Reset special mode after use
    specialModeActive = false;
  } 
  else if (bothButtonsLock && !isBothButtonsPressed()) {
    // Buttons were released
    bothButtonsLock = false;
  }
}

void accelerometerNeoPixelDemoSmoother(Adafruit_NeoPixel &pixels) {
  Serial.println("Accelerometer NeoPixel Demo Smoother. Press LEFT button to exit.");

  // Define the grid dimensions
  const uint8_t gridWidth = 5;
  const uint8_t gridHeight = 5;

  // Smoothing factor for low-pass filter
  const float alpha = 0.2;  // Adjust between 0 (no new data) and 1 (no filtering)

  // Variables to hold smoothed accelerometer values
  float filteredX = 0;
  float filteredY = 0;
  float filteredZ = 0;

  int selectedColorIndex = 0;  // Start with the first color
                               // bool rightButtonLastState = HIGH; // Assume button not pressed (pull-up resistor)

  // Variables for right button press detection
  bool rightButtonPressed = false;
  unsigned long pressStartTime = 0;
  bool longPressHandled = false;

  bool rightButtonLastState = HIGH;  // Assume button not pressed (pull-up resistor)

  while (!isLeftButtonPressed() && !animationInterrupted) {
    Watchdog.reset();
    unsigned long currentMillis = millis();

    // Check for both buttons pressed
    if (isBothButtonsPressed()) {
      handleBothButtonsPressed();
    }

    // Read the current state of the right button
    bool rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);

    if (rightButtonLastState == HIGH && rightButtonCurrentState == LOW) {
      // Button was just pressed
      delay(50);  // Debounce delay
      rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);
      if (rightButtonCurrentState == LOW) {
        rightButtonPressed = true;
        pressStartTime = millis();
        longPressHandled = false;
        Serial.println("Right button pressed.");
      }
    }

    if (rightButtonLastState == LOW && rightButtonCurrentState == HIGH) {
      // Button was just released
      if (rightButtonPressed) {
        unsigned long pressDuration = millis() - pressStartTime;
        if (pressDuration < SHORT_PRESS_THRESHOLD) {
          // Short press detected
          Serial.println("Short press detected. No change for this animation");
          selectedColorIndex = (selectedColorIndex + 1) % numColors;
          Serial.print("Color changed to index ");
          Serial.println(selectedColorIndex);
        } else if (pressDuration >= LONG_PRESS_THRESHOLD && !longPressHandled) {
          // Long press detected upon release, only if not handled during holding
          Serial.println("Long press detected.");
          handleLongPress(animationIndex);  // Example function
        }
        rightButtonPressed = false;
      }
    }

    // Handle long press if button is still pressed and threshold exceeded
    if (rightButtonPressed && !longPressHandled) {
      unsigned long pressDuration = millis() - pressStartTime;
      if (pressDuration >= LONG_PRESS_THRESHOLD) {
        longPressHandled = true;
        Serial.println("Long press detected (while holding).");
        handleLongPress(animationIndex);  // Example function
      }
    }

    rightButtonLastState = rightButtonCurrentState;

    float x, y, z;
    getAccelerometerValues(lis, x, y, z);

    // Apply low-pass filter
    filteredX = alpha * x + (1 - alpha) * filteredX;
    filteredY = alpha * y + (1 - alpha) * filteredY;
    filteredZ = alpha * z + (1 - alpha) * filteredZ;

    // Map filtered x and y values to grid positions (0 to 4)
    int gridX = map(filteredX * 100, -100, 100, 0, 4);
    int gridY = map(filteredY * 100, -100, 100, 0, 4);  // Inverted Y mapping

    // Constrain to grid
    gridX = constrain(gridX, 0, 4);
    gridY = constrain(gridY, 0, 4);

    // Get pixel indices
    int leftPixel = leftGrid[gridY][gridX];
    int rightPixel = rightGrid[gridY][gridX];

    // Clear all pixels
    setAllNeoPixelsColor(pixels, pixels.Color(0, 0, 0));

    // Set the pixels
    if (leftPixel != -1) {
      uint8_t red = colorArray[selectedColorIndex][0];
      uint8_t green = colorArray[selectedColorIndex][1];
      uint8_t blue = colorArray[selectedColorIndex][2];
      pixels.setPixelColor(leftPixel, pixels.Color(red, green, blue));
    }
    if (rightPixel != -1) {
      uint8_t red = colorArray[selectedColorIndex][0];
      uint8_t green = colorArray[selectedColorIndex][1];
      uint8_t blue = colorArray[selectedColorIndex][2];
      pixels.setPixelColor(rightPixel, pixels.Color(red, green, blue));
    }

    pixels.show();

    // Small delay to allow for button checking
    // delay(10);
  }

  // Turn off all pixels after exiting
  setAllNeoPixelsColor(pixels, pixels.Color(0, 0, 0));
  delay(50);  // Debounce delay
}


void gameOfLifeNeoPixelDemo(Adafruit_NeoPixel &pixels) {
  Serial.println("Game of Life Animation. Press LEFT button to exit.");

  // Initialize grid mappings
  copyGridsToCombinedGrid();

  // Initialize live cell color
  LIVE_CELL_COLOR = colorList[currentColorIndex];

  // Initialize random seed from A3
  randomSeed(analogRead(A3));  // Use A3 for random seed

  // Initialize grid state
  randomizeGrid();
  displayGrid();
  // printGridToSerial();

  // Initialize previous states
  for (int row = 0; row < 5; row++) {
    for (int col = 0; col < 10; col++) {
      previousState[row][col] = currentState[row][col];
      secondPreviousState[row][col] = currentState[row][col];
    }
  }

  // Variables for button debouncing
  // bool rightButtonReading;
  // bool rightButtonState = HIGH;       // Debounced button state
  // bool lastRightButtonState = HIGH;   // Previous debounced button state


  unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 50;

  unsigned long previousMillis = 0;
  const unsigned long interval = 20;  // Update interval for animation
  // Variables for right button press detection
  bool rightButtonPressed = false;
  unsigned long pressStartTime = 0;
  bool longPressHandled = false;

  bool rightButtonLastState = HIGH;  // Assume button not pressed (pull-up resistor)

  while (!isLeftButtonPressed() && !animationInterrupted) {
    Watchdog.reset();
    unsigned long currentMillis = millis();

    // Check for both buttons pressed
    if (isBothButtonsPressed()) {
      handleBothButtonsPressed();
    }

    // Read the current state of the right button
    bool rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);

    if (rightButtonLastState == HIGH && rightButtonCurrentState == LOW) {
      // Button was just pressed
      delay(50);  // Debounce delay
      rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);
      if (rightButtonCurrentState == LOW) {
        rightButtonPressed = true;
        pressStartTime = millis();
        longPressHandled = false;
        Serial.println("Right button pressed.");
      }
    }

    if (rightButtonLastState == LOW && rightButtonCurrentState == HIGH) {
      // Button was just released
      if (rightButtonPressed) {
        unsigned long pressDuration = millis() - pressStartTime;
        if (pressDuration < SHORT_PRESS_THRESHOLD) {
          // Short press detected
          Serial.println("Short press detected.");
          randomizeGrid();
          displayGrid();
        } else if (pressDuration >= LONG_PRESS_THRESHOLD && !longPressHandled) {
          // Long press detected upon release, only if not handled during holding
          Serial.println("Long press detected.");
          handleLongPress(animationIndex);  // Example function
        }
        rightButtonPressed = false;
      }
    }

    // Handle long press if button is still pressed and threshold exceeded
    if (rightButtonPressed && !longPressHandled) {
      unsigned long pressDuration = millis() - pressStartTime;
      if (pressDuration >= LONG_PRESS_THRESHOLD) {
        longPressHandled = true;
        Serial.println("Long press detected (while holding).");
        handleLongPress(animationIndex);  // Example function
      }
    }

    rightButtonLastState = rightButtonCurrentState;

    if (currentMillis - lastGenerationTime >= GENERATION_INTERVAL_MS) {
      lastGenerationTime = currentMillis;

      // Perform fade out of current state
      fadeTransition(true, false);

      // Update grid to next generation
      updateGrid();

      // Check for restart conditions
      bool allOff = true;
      for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 10; col++) {
          if (grid[row][col] != -1 && currentState[row][col]) {
            allOff = false;
            break;
          }
        }
        if (!allOff) break;
      }

      if (allOff) {
        restartSimulation();
        continue;  // Skip further processing after restart
      }

      // Check if current state is same as previous state
      if (areGridsEqual(currentState, previousState)) {
        stableCounter++;
        if (stableCounter >= STABLE_THRESHOLD) {
          restartSimulation();
          continue;  // Skip further processing after restart
        }
      } else {
        stableCounter = 0;  // Reset counter if state changes
      }

      // Check for oscillation between two states
      if (checkOscillation()) {
        restartSimulation();
        continue;  // Skip further processing after restart
      }

      // Save current state to previousState and secondPreviousState for next comparison
      for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 10; col++) {
          secondPreviousState[row][col] = previousState[row][col];
          previousState[row][col] = currentState[row][col];
        }
      }

      // Perform fade in of new state
      fadeTransition(false, true);

      // Display and print the updated grid
      displayGrid();
      // printGridToSerial();
    }

    // Small delay to prevent CPU hogging
    delay(1);
  }

  // Turn off all pixels after exiting
  setAllNeoPixelsColor(pixels, 0);
  pixels.show();
  delay(50);  // Debounce delay
}


// ---------------------------
// Function Definitions
// ---------------------------

// Copy left and right grids into combined grid
void copyGridsToCombinedGrid() {
  // Copy left grid
  for (int row = 0; row < 5; row++) {
    for (int col = 0; col < 5; col++) {
      grid[row][col] = leftGrid[row][col];
    }
  }
  // Copy right grid
  for (int row = 0; row < 5; row++) {
    for (int col = 0; col < 5; col++) {
      grid[row][col + 5] = rightGrid[row][col];
    }
  }
}

// Initialize the grid with a random pattern
void randomizeGrid() {
  clearGrid();  // Start with all cells dead

  for (int row = 0; row < 5; row++) {
    for (int col = 0; col < 10; col++) {
      if (grid[row][col] != -1 && !isEdgeCell(row, col)) {
        currentState[row][col] = random(0, 2);  // Randomly set to true (live) or false (dead)
      } else {
        currentState[row][col] = false;  // Edges are dead
      }
    }
  }

  // Serial.println("Grid randomized.");
}

// Initialize the grid with a predefined pattern (e.g., Glider)
void setPredefinedPattern() {
  clearGrid();  // Start with all cells dead

  // Example: Glider pattern
  /*
    . O .
    . . O
    O O O
  */

  // Coordinates for the glider in the grid
  // Assuming grid[5][10], placing glider somewhere not on the edge
  int baseRow = 1;  // Starting at row 1
  int baseCol = 3;  // Starting at column 3 (left grid)

  currentState[baseRow][baseCol + 0] = false;
  currentState[baseRow][baseCol + 1] = true;
  currentState[baseRow][baseCol + 2] = false;

  currentState[baseRow + 1][baseCol + 0] = false;
  currentState[baseRow + 1][baseCol + 1] = false;
  currentState[baseRow + 1][baseCol + 2] = true;

  currentState[baseRow + 2][baseCol + 0] = true;
  currentState[baseRow + 2][baseCol + 1] = true;
  currentState[baseRow + 2][baseCol + 2] = true;

  // Serial.println("Predefined glider pattern set.");
}

// Clear the grid by setting all cells to dead
void clearGrid() {
  for (int row = 0; row < 5; row++) {
    for (int col = 0; col < 10; col++) {
      currentState[row][col] = false;
      nextState[row][col] = false;
      previousState[row][col] = false;
      secondPreviousState[row][col] = false;
    }
  }
  stableCounter = 0;
  oscillationCounter = 0;
  // Serial.println("Grid cleared.");
}

// Update the grid to the next generation
void updateGrid() {
  for (int row = 0; row < 5; row++) {
    for (int col = 0; col < 10; col++) {
      if (grid[row][col] != -1) {
        int liveNeighbors = countLiveNeighbors(row, col);
        if (currentState[row][col]) {
          // Cell is alive
          if (liveNeighbors < 2 || liveNeighbors > 3) {
            nextState[row][col] = false;  // Cell dies
          } else {
            nextState[row][col] = true;  // Cell lives
          }
        } else {
          // Cell is dead
          if (liveNeighbors == 3) {
            nextState[row][col] = true;  // Cell becomes alive
          } else {
            nextState[row][col] = false;  // Cell remains dead
          }
        }
      } else {
        nextState[row][col] = false;  // No cell at this position
      }
    }
  }

  // Copy nextState to currentState
  for (int row = 0; row < 5; row++) {
    for (int col = 0; col < 10; col++) {
      currentState[row][col] = nextState[row][col];
    }
  }

  // Serial.println("Grid updated to next generation.");
}

// Count the number of live neighbors for a cell
int countLiveNeighbors(int row, int col) {
  int liveNeighbors = 0;
  for (int i = -1; i <= 1; i++) {
    int neighborRow = row + i;
    if (neighborRow < 0 || neighborRow >= 5) continue;
    for (int j = -1; j <= 1; j++) {
      int neighborCol = col + j;
      if (neighborCol < 0 || neighborCol >= 10) continue;
      if (i == 0 && j == 0) continue;  // Skip the cell itself
      if (currentState[neighborRow][neighborCol]) {
        liveNeighbors++;
      }
    }
  }
  return liveNeighbors;
}

// Display the current grid state on the NeoPixel grids
void displayGrid() {
  // Directly set the pixel colors based on currentState
  for (int row = 0; row < 5; row++) {
    for (int col = 0; col < 10; col++) {
      int pixelIndex = grid[row][col];
      if (pixelIndex != -1) {
        if (currentState[row][col]) {
          pixels.setPixelColor(pixelIndex, LIVE_CELL_COLOR);
        } else {
          pixels.setPixelColor(pixelIndex, DEAD_CELL_COLOR);
        }
      }
    }
  }
  pixels.show();
}

// Print the current grid state to the Serial Monitor
void printGridToSerial() {
  Serial.println("Current Grid State:");
  for (int row = 0; row < 5; row++) {
    for (int col = 0; col < 10; col++) {
      if (grid[row][col] != -1) {
        Serial.print(currentState[row][col] ? "O" : ".");
      } else {
        Serial.print(" ");  // Represent invalid positions as space
      }
    }
    Serial.println();
  }
  Serial.println();
}

// Check if a cell is an edge cell (should remain dead)
bool isEdgeCell(int row, int col) {
  // Define edges where there are no physical LEDs
  // Left grid edges
  if (col < 5) {
    if ((row == 0 && (col == 0 || col == 4)) || (row == 4 && (col == 0 || col == 4))) {
      return true;
    }
  }
  // Right grid edges
  if (col >= 5) {
    int c = col - 5;
    if ((row == 0 && (c == 0 || c == 4)) || (row == 4 && (c == 0 || c == 4))) {
      return true;
    }
  }
  return false;
}

// Compare two grids for equality
bool areGridsEqual(bool grid1[5][10], bool grid2[5][10]) {
  for (int row = 0; row < 5; row++) {
    for (int col = 0; col < 10; col++) {
      if (grid1[row][col] != grid2[row][col]) {
        return false;
      }
    }
  }
  return true;
}

// Check for oscillation between two states
bool checkOscillation() {
  if (areGridsEqual(currentState, secondPreviousState)) {
    oscillationCounter++;
    //  Serial.print("Oscillation count: ");
    //  Serial.println(oscillationCounter);
    if (oscillationCounter > OSCILLATION_THRESHOLD) {
      return true;
    }
  } else {
    oscillationCounter = 0;
  }
  return false;
}

// Restart the simulation by randomizing the grid and changing the live cell color
void restartSimulation() {
  // Change live cell color
  currentColorIndex = (currentColorIndex + 1) % numColors2;
  LIVE_CELL_COLOR = colorList[currentColorIndex];

  // Serial.print("Changing live cell color to: ");
  //  Serial.println(LIVE_CELL_COLOR, HEX);

  // Perform fade out of current state
  fadeTransition(true, false);

  // Reinitialize grid
  randomizeGrid();

  // Perform fade in of new state
  fadeTransition(false, true);

  // Display and print the updated grid
  displayGrid();
  // printGridToSerial();

  // Update previous states to match current state
  for (int row = 0; row < 5; row++) {
    for (int col = 0; col < 10; col++) {
      previousState[row][col] = currentState[row][col];
      secondPreviousState[row][col] = currentState[row][col];
    }
  }

  // Reset stable and oscillation counters
  stableCounter = 0;
  oscillationCounter = 0;
}


// Fade transition between states
void fadeTransition(bool fadingOut, bool fadingIn) {
  const int FADE_STEPS = 20;
  for (int step = 1; step <= FADE_STEPS; step++) {
    // Check for left button press to exit early
    if (isLeftButtonPressed()) {
      break;
    }

    for (int row = 0; row < 5; row++) {
      for (int col = 0; col < 10; col++) {
        int pixelIndex = grid[row][col];
        if (pixelIndex != -1) {
          bool wasLive = previousState[row][col];
          bool isLive = currentState[row][col];

          uint32_t fromColor = wasLive ? LIVE_CELL_COLOR : DEAD_CELL_COLOR;
          uint32_t toColor = isLive ? LIVE_CELL_COLOR : DEAD_CELL_COLOR;

          uint8_t r1 = (fromColor >> 16) & 0xFF;
          uint8_t g1 = (fromColor >> 8) & 0xFF;
          uint8_t b1 = fromColor & 0xFF;

          uint8_t r2 = (toColor >> 16) & 0xFF;
          uint8_t g2 = (toColor >> 8) & 0xFF;
          uint8_t b2 = toColor & 0xFF;

          uint8_t r, g, b;

          if (fadingOut && !fadingIn) {
            // Only fade out
            r = r1 - (r1 * step) / FADE_STEPS;
            g = g1 - (g1 * step) / FADE_STEPS;
            b = b1 - (b1 * step) / FADE_STEPS;
          } else if (!fadingOut && fadingIn) {
            // Only fade in
            r = r1 + ((r2 - r1) * step) / FADE_STEPS;
            g = g1 + ((g2 - g1) * step) / FADE_STEPS;
            b = b1 + ((b2 - b1) * step) / FADE_STEPS;
          } else {
            // No fading
            r = r2;
            g = g2;
            b = b2;
          }

          // Ensure RGB values are within 0-255
          r = constrain(r, 0, 255);
          g = constrain(g, 0, 255);
          b = constrain(b, 0, 255);

          pixels.setPixelColor(pixelIndex, pixels.Color(r, g, b));
        }
      }
    }
    pixels.show();
    delay(FADE_STEP_DELAY_MS);
  }
}


///////////////////////////////////////////

// tetris animation function

///*
// Tetris animation function
void tetrisNeoPixelDemo(Adafruit_NeoPixel &pixels) {
  // Seed random number generator using analogRead(A3)
  randomSeed(analogRead(A3));
  Serial.println("Tetris Animation. Press LEFT button to exit.");

  // Spawn the first piece
  spawnNewPiece();

  // Variables for button debouncing
  // bool rightButtonReading;
  //  bool rightButtonState = HIGH;       // Debounced button state
  // bool lastRightButtonState = HIGH;   // Previous debounced button state
  unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 50;

  unsigned long previousMillis = 0;
  const unsigned long interval = 20;  // Update interval for animation

  // Variables for right button press detection
  bool rightButtonPressed = false;
  unsigned long pressStartTime = 0;
  bool longPressHandled = false;

  bool rightButtonLastState = HIGH;  // Assume button not pressed (pull-up resistor)

  while (!isLeftButtonPressed() && !animationInterrupted) {
    Watchdog.reset();
    unsigned long currentMillis = millis();

    // Check for both buttons pressed
    if (isBothButtonsPressed()) {
      handleBothButtonsPressed();
    }

    // Read the current state of the right button
    bool rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);

    if (rightButtonLastState == HIGH && rightButtonCurrentState == LOW) {
      // Button was just pressed
      delay(50);  // Debounce delay
      rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);
      if (rightButtonCurrentState == LOW) {
        rightButtonPressed = true;
        pressStartTime = millis();
        longPressHandled = false;
        Serial.println("Right button pressed.");
      }
    }

    if (rightButtonLastState == LOW && rightButtonCurrentState == HIGH) {
      // Button was just released
      if (rightButtonPressed) {
        unsigned long pressDuration = millis() - pressStartTime;
        if (pressDuration < SHORT_PRESS_THRESHOLD) {
          // Short press detected
          Serial.println("Short press detected.");
          rotatePiece();
        } else if (pressDuration >= LONG_PRESS_THRESHOLD && !longPressHandled) {
          // Long press detected upon release, only if not handled during holding
          Serial.println("Long press detected.");
          handleLongPress(animationIndex);  // Example function
        }
        rightButtonPressed = false;
      }
    }

    // Handle long press if button is still pressed and threshold exceeded
    if (rightButtonPressed && !longPressHandled) {
      unsigned long pressDuration = millis() - pressStartTime;
      if (pressDuration >= LONG_PRESS_THRESHOLD) {
        longPressHandled = true;
        Serial.println("Long press detected (while holding).");
        handleLongPress(animationIndex);  // Example function
      }
    }

    rightButtonLastState = rightButtonCurrentState;

    // Move the piece down at regular intervals
    if (currentMillis - lastFallTime >= FALL_SPEED_MS) {
      lastFallTime = currentMillis;

      // Move the piece down
      pieceY += 1;

      // If the piece has moved beyond the grid, spawn a new piece
      if (pieceY > GRID_HEIGHT) {
        spawnNewPiece();
      }
    }

    // Draw the current piece
    drawCurrentPiece();

    // Small delay to prevent CPU hogging
    delay(1);
  }

  // Turn off all pixels after exiting
  clearDisplay();
  pixels.show();
  delay(50);  // Debounce delay
}

//*/

// ---------------------------
// Function Definitions
// ---------------------------

// Spawn a new random tetromino
void spawnNewPiece() {
  int index = random(0, NUM_TETROMINOES);

  // Copy the shape
  copyShape(currentPiece.shape, tetrominoShapes[index]);

  // Set the color
  currentPiece.color = tetrominoColors[index];

  // Reset position
  pieceX = 1;   // Start near the middle
  pieceY = -4;  // Start above the grid

  // Reset rotation state
  currentPiece.rotationState = 0;

  // Serial.print("Spawned New Piece Index: ");
  // Serial.println(index);
}

// Rotate the current piece
void rotatePiece() {
  // Update rotation state
  currentPiece.rotationState = (currentPiece.rotationState + 1) % 4;
  // Serial.print("Rotation State: ");
  // Serial.println(currentPiece.rotationState);
}

// Draw the current piece on the NeoPixel grids
void drawCurrentPiece() {
  // Clear the display
  clearDisplay();

  // Create a temporary shape to hold the rotated shape
  int rotatedShape[4][4];
  copyShape(rotatedShape, currentPiece.shape);
  rotateShape(rotatedShape, currentPiece.rotationState);

  // Draw current piece
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      if (rotatedShape[y][x]) {
        int gridX = pieceX + x;
        int gridY = pieceY + y;
        if (gridY >= 0 && gridY < GRID_HEIGHT && gridX >= 0 && gridX < GRID_WIDTH) {
          int leftPixelIndex = leftGrid[gridY][gridX];
          int rightPixelIndex = rightGrid[gridY][gridX];
          if (leftPixelIndex != -1) {
            pixels.setPixelColor(leftPixelIndex, currentPiece.color);
          }
          if (rightPixelIndex != -1) {
            pixels.setPixelColor(rightPixelIndex, currentPiece.color);
          }
        }
      }
    }
  }

  // Update the NeoPixel strip to show changes
  pixels.show();
}

// Clear the display
void clearDisplay() {
  pixels.clear();
}

// Copy a shape from src to dest
void copyShape(int dest[4][4], const int src[4][4]) {
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      dest[y][x] = src[y][x];
    }
  }
}

// Rotate the shape based on rotation state
void rotateShape(int shape[4][4], int rotationState) {
  int temp[4][4];

  // Apply rotation the number of times specified by rotationState
  for (int i = 0; i < rotationState; i++) {
    // Rotate the shape 90 degrees clockwise
    for (int y = 0; y < 4; y++) {
      for (int x = 0; x < 4; x++) {
        temp[y][x] = shape[3 - x][y];
      }
    }
    // Copy temp back to shape
    for (int y = 0; y < 4; y++) {
      for (int x = 0; x < 4; x++) {
        shape[y][x] = temp[y][x];
      }
    }
  }
}


// Falling Drops animation function
void fallingDropsNeoPixelDemo(Adafruit_NeoPixel &pixels) {
  Serial.println("Falling Drops Animation. Press LEFT button to exit.");

  // Droplet Arrays for Grids
  Droplet dropletsLeft[MAX_DROPLETS_PER_GRID];
  int dropletCountLeft = 0;

  Droplet dropletsRight[MAX_DROPLETS_PER_GRID];
  int dropletCountRight = 0;

  // Color Selection Variables
  int colorMode = 0;  // 0 = Random, 1-10 = colorArray[0-9]

  // Variables for Button Debouncing
  // bool rightButtonLastState = HIGH;
  unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 50;

  // Initialize Droplet arrays for Left and Right grids
  for (int i = 0; i < MAX_DROPLETS_PER_GRID; i++) {
    // Left Grid Droplets
    dropletsLeft[i].row = -1;  // Inactive

    // Right Grid Droplets
    dropletsRight[i].row = -1;  // Inactive
  }

  // Seed random number generator for randomized colors and positions
  randomSeed(analogRead(A3));

  unsigned long previousMillis = 0;
  const unsigned long interval = 20;  // Update interval for animation

  // Variables for right button press detection
  bool rightButtonPressed = false;
  unsigned long pressStartTime = 0;
  bool longPressHandled = false;

  bool rightButtonLastState = HIGH;  // Assume button not pressed (pull-up resistor)

  while (!isLeftButtonPressed() && !animationInterrupted) {
    Watchdog.reset();
    unsigned long currentMillis = millis();

    // Check for both buttons pressed
    if (isBothButtonsPressed()) {
      handleBothButtonsPressed();
    }

    // Read the current state of the right button
    bool rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);

    if (rightButtonLastState == HIGH && rightButtonCurrentState == LOW) {
      // Button was just pressed
      delay(50);  // Debounce delay
      rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);
      if (rightButtonCurrentState == LOW) {
        rightButtonPressed = true;
        pressStartTime = millis();
        longPressHandled = false;
        Serial.println("Right button pressed.");
      }
    }

    if (rightButtonLastState == LOW && rightButtonCurrentState == HIGH) {
      // Button was just released
      if (rightButtonPressed) {
        unsigned long pressDuration = millis() - pressStartTime;
        if (pressDuration < SHORT_PRESS_THRESHOLD) {
          // Short press detected
          Serial.println("Short press detected.");
          // Cycle colorMode
          colorMode = (colorMode + 1) % (numColors + 1);  // 0 to numColors
        } else if (pressDuration >= LONG_PRESS_THRESHOLD && !longPressHandled) {
          // Long press detected upon release, only if not handled during holding
          Serial.println("Long press detected.");
          handleLongPress(animationIndex);  // Example function
        }
        rightButtonPressed = false;
      }
    }

    // Handle long press if button is still pressed and threshold exceeded
    if (rightButtonPressed && !longPressHandled) {
      unsigned long pressDuration = millis() - pressStartTime;
      if (pressDuration >= LONG_PRESS_THRESHOLD) {
        longPressHandled = true;
        Serial.println("Long press detected (while holding).");
        handleLongPress(animationIndex);  // Example function
      }
    }

    rightButtonLastState = rightButtonCurrentState;

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      // Create new droplets for Left Grid
      if (dropletCountLeft < MAX_DROPLETS_PER_GRID) {
        createDroplet(dropletsLeft, dropletCountLeft, leftGrid);
      }

      // Create new droplets for Right Grid
      if (dropletCountRight < MAX_DROPLETS_PER_GRID) {
        createDroplet(dropletsRight, dropletCountRight, rightGrid);
      }

      // Update droplet positions for Left Grid
      updateDroplets(dropletsLeft, dropletCountLeft, leftGrid);

      // Update droplet positions for Right Grid
      updateDroplets(dropletsRight, dropletCountRight, rightGrid);

      // Display droplets on Left Grid
      displayDroplets(dropletsLeft, dropletCountLeft, leftGrid);

      // Display droplets on Right Grid
      displayDroplets(dropletsRight, dropletCountRight, rightGrid);
    }

    // Small delay to prevent CPU hogging
    delay(1);
  }

  // Turn off all pixels after exiting
  setAllNeoPixelsColor(pixels, 0);
  pixels.show();
  delay(50);  // Debounce delay
}

// ---------------------------
// Function Definitions
// ---------------------------

// Function to create a new droplet in the specified grid
void createDroplet(Droplet droplets[], int &dropletCount, const int grid[5][5]) {
  // Find an inactive droplet slot
  for (int i = 0; i < MAX_DROPLETS_PER_GRID; i++) {
    if (droplets[i].row == -1) {
      droplets[i].row = 0;  // Start at top row (y=0)

      // Assign a random column within the grid based on row
      // y=0: columns 1-3
      // y=1-3: columns 0-4
      if (droplets[i].row == 0) {
        droplets[i].column = random(1, 4);  // Columns 1, 2, 3
      } else {
        droplets[i].column = random(0, 5);  // Columns 0-4
      }

      // Ensure the assigned column has a valid pixel in the current row
      if (grid[droplets[i].row][droplets[i].column] == -1) {
        // If invalid, assign to a valid column within the row
        if (droplets[i].row == 0) {
          droplets[i].column = random(1, 4);  // y=0: Columns 1-3
        } else {
          droplets[i].column = random(0, 5);  // y=1-3: Columns 0-4
        }

        // Recheck and assign again if necessary
        while (grid[droplets[i].row][droplets[i].column] == -1) {
          if (droplets[i].row == 0) {
            droplets[i].column = random(1, 4);
          } else {
            droplets[i].column = random(0, 5);
          }
        }
      }

      // Assign color based on current colorMode
      droplets[i].color = getColor();

      // Set fixed speed
      droplets[i].speed = DROPLET_SPEED_MS;

      // Update the lastUpdate timestamp
      droplets[i].lastUpdate = millis();

      // Increment droplet count
      dropletCount++;

      break;  // Exit after creating one droplet
    }
  }
}

// Function to update droplet positions in the specified grid
void updateDroplets(Droplet droplets[], int &dropletCount, const int grid[5][5]) {
  unsigned long currentTime = millis();

  for (int i = 0; i < MAX_DROPLETS_PER_GRID; i++) {
    if (droplets[i].row != -1) {  // Active droplet
      if (currentTime - droplets[i].lastUpdate >= droplets[i].speed) {
        droplets[i].row += 1;                  // Move droplet down by one row
        droplets[i].lastUpdate = currentTime;  // Reset the lastUpdate timestamp

        // Assign a new column if the droplet is now in y=1,2,3
        if (droplets[i].row >= 1 && droplets[i].row <= 3) {
          droplets[i].column = random(0, 5);  // Columns 0-4

          // Ensure the new column has a valid pixel in the current row
          if (grid[droplets[i].row][droplets[i].column] == -1) {
            // If invalid, assign to a valid column within the row
            droplets[i].column = random(0, 5);
            while (grid[droplets[i].row][droplets[i].column] == -1) {
              droplets[i].column = random(0, 5);
            }
          }
        }

        // Check if droplet has reached the bottom row
        if (droplets[i].row >= 5) {  // Assuming grid rows are 0-4
          droplets[i].row = -1;      // Deactivate droplet
          droplets[i].color = 0;     // Reset color
          dropletCount--;            // Decrement droplet count
        }
      }
    }
  }
}

// Function to display droplets on the specified grid
void displayDroplets(Droplet droplets[], int dropletCount, const int grid[5][5]) {
  // Clear all pixels in the grid before updating
  for (int row = 0; row < 5; row++) {
    for (int col = 0; col < 5; col++) {
      int pixelIndex = grid[row][col];
      if (pixelIndex != -1) {
        pixels.setPixelColor(pixelIndex, 0);  // Turn off pixel
      }
    }
  }

  // Light up active droplets
  for (int i = 0; i < MAX_DROPLETS_PER_GRID; i++) {
    if (droplets[i].row != -1) {
      int row = droplets[i].row;
      int col = droplets[i].column;
      int pixelIndex = grid[row][col];
      if (pixelIndex != -1) {
        pixels.setPixelColor(pixelIndex, droplets[i].color);  // Set droplet color
      }
    }
  }

  // Update the NeoPixel strip to show changes
  pixels.show();
}

// Function to get the current color based on colorMode
uint32_t getColor() {
  if (colorMode == 0) {
    // Random color
    uint16_t hue = random(0, 65535);  // Random hue
    return ColorHSV(hue, 255, 255);   // Full saturation and brightness
  } else {
    // Predefined color from colorArray
    uint8_t red = colorArray[colorMode - 1][0];
    uint8_t green = colorArray[colorMode - 1][1];
    uint8_t blue = colorArray[colorMode - 1][2];
    return pixels.Color(red, green, blue);
  }
}

// Helper function to convert HSV to RGB
uint32_t ColorHSV(long hue, uint8_t sat, uint8_t val) {
  // Hue: 0-65535, Saturation: 0-255, Value: 0-255
  uint16_t region = hue / (65535 / 6);
  uint8_t remainder = (hue % (65535 / 6)) * 255 / (65535 / 6);
  uint8_t p = (val * (255 - sat)) / 255;
  uint8_t q = (val * (255 - ((sat * remainder) / 255))) / 255;
  uint8_t t = (val * (255 - ((sat * (255 - remainder)) / 255))) / 255;
  uint8_t r, g, b;

  switch (region) {
    case 0:
      r = val;
      g = t;
      b = p;
      break;
    case 1:
      r = q;
      g = val;
      b = p;
      break;
    case 2:
      r = p;
      g = val;
      b = t;
      break;
    case 3:
      r = p;
      g = q;
      b = val;
      break;
    case 4:
      r = t;
      g = p;
      b = val;
      break;
    case 5:
    default:
      r = val;
      g = p;
      b = q;
      break;
  }

  return pixels.Color(r, g, b);
}

void spiralingVortexNeoPixelDemo(Adafruit_NeoPixel &pixels) {
  Serial.println("Spiraling Vortex NeoPixel Demo. Press LEFT button to exit.");

  int selectedColorIndex = 0;
  //  bool rightButtonLastState = HIGH;
  int radius = 0;

  // Variables for right button press detection
  bool rightButtonPressed = false;
  unsigned long pressStartTime = 0;
  bool longPressHandled = false;

  bool rightButtonLastState = HIGH;  // Assume button not pressed (pull-up resistor)

  while (!isLeftButtonPressed() && !animationInterrupted) {
    Watchdog.reset();
    unsigned long currentMillis = millis();

    // Check for both buttons pressed
    if (isBothButtonsPressed()) {
      handleBothButtonsPressed();
    }

    // Read the current state of the right button
    bool rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);

    if (rightButtonLastState == HIGH && rightButtonCurrentState == LOW) {
      // Button was just pressed
      delay(50);  // Debounce delay
      rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);
      if (rightButtonCurrentState == LOW) {
        rightButtonPressed = true;
        pressStartTime = millis();
        longPressHandled = false;
        Serial.println("Right button pressed.");
      }
    }

    if (rightButtonLastState == LOW && rightButtonCurrentState == HIGH) {
      // Button was just released
      if (rightButtonPressed) {
        unsigned long pressDuration = millis() - pressStartTime;
        if (pressDuration < SHORT_PRESS_THRESHOLD) {
          // Short press detected
          Serial.println("Short press detected. No change for this animation");
          selectedColorIndex = (selectedColorIndex + 1) % numColors;
          Serial.print("Color changed to index ");
          Serial.println(selectedColorIndex);
        } else if (pressDuration >= LONG_PRESS_THRESHOLD && !longPressHandled) {
          // Long press detected upon release, only if not handled during holding
          Serial.println("Long press detected.");
          handleLongPress(animationIndex);  // Example function
        }
        rightButtonPressed = false;
      }
    }

    // Handle long press if button is still pressed and threshold exceeded
    if (rightButtonPressed && !longPressHandled) {
      unsigned long pressDuration = millis() - pressStartTime;
      if (pressDuration >= LONG_PRESS_THRESHOLD) {
        longPressHandled = true;
        Serial.println("Long press detected (while holding).");
        handleLongPress(animationIndex);  // Example function
      }
    }

    rightButtonLastState = rightButtonCurrentState;

    uint8_t red = colorArray[selectedColorIndex][0];
    uint8_t green = colorArray[selectedColorIndex][1];
    uint8_t blue = colorArray[selectedColorIndex][2];

    setAllNeoPixelsColor(pixels, 0);

    for (int i = 0; i < radius; i++) {
      int pixelIndex = (i * 7 + radius) % pixels.numPixels();  // Spiral pattern
      pixels.setPixelColor(pixelIndex, pixels.Color(red, green, blue));
    }

    pixels.show();
    radius = (radius + 1) % pixels.numPixels();
    delay(100);
  }

  setAllNeoPixelsColor(pixels, 0);
  delay(50);
}

void neopixelsOff(Adafruit_NeoPixel &pixels) {
  setAllNeoPixelsColor(pixels, 0);  // Turn off all NeoPixels
}

void theaterMarqueeNeoPixelDemo(Adafruit_NeoPixel &pixels) {
  Serial.println("Theater Marquee NeoPixel Demo. Press LEFT button to exit.");
  Watchdog.reset();
  int selectedColorIndex = 0;
  // Variables for right button press detection
  bool rightButtonPressed = false;
  unsigned long pressStartTime = 0;
  bool longPressHandled = false;

  bool rightButtonLastState = HIGH;  // Assume button not pressed (pull-up resistor)

  while (!isLeftButtonPressed() && !animationInterrupted) {
    Watchdog.reset();
    unsigned long currentMillis = millis();

    // Check for both buttons pressed
    if (isBothButtonsPressed()) {
      handleBothButtonsPressed();
    }

    // Read the current state of the right button
    bool rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);

    if (rightButtonLastState == HIGH && rightButtonCurrentState == LOW) {
      // Button was just pressed
      delay(50);  // Debounce delay
      rightButtonCurrentState = digitalRead(RIGHT_BUTTON_PIN);
      if (rightButtonCurrentState == LOW) {
        rightButtonPressed = true;
        pressStartTime = millis();
        longPressHandled = false;
        Serial.println("Right button pressed.");
      }
    }

    if (rightButtonLastState == LOW && rightButtonCurrentState == HIGH) {
      // Button was just released
      if (rightButtonPressed) {
        unsigned long pressDuration = millis() - pressStartTime;
        if (pressDuration < SHORT_PRESS_THRESHOLD) {
          // Short press detected
          Serial.println("Short press detected. No change for this animation");
          selectedColorIndex = (selectedColorIndex + 1) % numColors;
          Serial.print("Color changed to index ");
          Serial.println(selectedColorIndex);
        } else if (pressDuration >= LONG_PRESS_THRESHOLD && !longPressHandled) {
          // Long press detected upon release, only if not handled during holding
          Serial.println("Long press detected.");
          handleLongPress(animationIndex);  // Example function
        }
        rightButtonPressed = false;
      }
    }

    // Handle long press if button is still pressed and threshold exceeded
    if (rightButtonPressed && !longPressHandled) {
      unsigned long pressDuration = millis() - pressStartTime;
      if (pressDuration >= LONG_PRESS_THRESHOLD) {
        longPressHandled = true;
        Serial.println("Long press detected (while holding).");
        handleLongPress(animationIndex);  // Example function
      }
    }

    rightButtonLastState = rightButtonCurrentState;

    uint8_t red = colorArray[selectedColorIndex][0];
    uint8_t green = colorArray[selectedColorIndex][1];
    uint8_t blue = colorArray[selectedColorIndex][2];

    for (int i = 0; i < pixels.numPixels(); i++) {
      if (i % 3 == 0) {
        pixels.setPixelColor(i, pixels.Color(red, green, blue));
      } else {
        pixels.setPixelColor(i, 0);
      }
    }
    pixels.show();
    delay(100);

    for (int i = 0; i < pixels.numPixels(); i++) {
      if ((i + 1) % 3 == 0) {
        pixels.setPixelColor(i, pixels.Color(red, green, blue));
      } else {
        pixels.setPixelColor(i, 0);
      }
    }
    pixels.show();
    delay(100);
  }

  setAllNeoPixelsColor(pixels, 0);
  delay(50);
}



// Function to control individual LEDs based on LED ID
bool setIndividualLED(uint8_t ledId, bool state) {
  switch (ledId) {
    case 1:
      setLED(RED_CUP_RIGHT_PIN, state);
      break;
    case 2:
      setLED(RED_CUP_LEFT_PIN, state);
      break;
    case 3:
      setLED(WHITE_SWORD_RIGHT_PIN, state);
      break;
    case 4:
      setLED(WHITE_SWORD_LEFT_PIN, state);
      break;
    case 5:
      setLED(YELLOW_WAND_UPPER_LEFT_PIN, state);
      break;
    case 6:
      setLED(YELLOW_WAND_UPPER_RIGHT_PIN, state);
      break;
    case 7:
      setLED(YELLOW_WAND_LOWER_LEFT_PIN, state);
      break;
    case 8:
      setLED(YELLOW_WAND_LOWER_RIGHT_PIN, state);
      break;
    default:
      Serial.println("Invalid LED ID.");
      return false;
  }
  return true;
}
