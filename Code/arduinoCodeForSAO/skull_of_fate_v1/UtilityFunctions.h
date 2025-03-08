// UtilityFunctions.h

#ifndef UTILITYFUNCTIONS_H
#define UTILITYFUNCTIONS_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_ZeroFFT.h>
#include <Adafruit_ZeroPDM.h>

#include "NFCWriter.h"
#include "SparkFun_ST25DV64KC_Arduino_Library.h"

// Extern variables
extern NFCWriter nfcWriter;

// I2C client Address
//int CLIENT_ADDRESS = 0x12;  // Ensure this matches the host's client_ADDRESS


// PDM MICROPHONE CONFIGURATION
#define PIN_PDM_CLK 6
#define PIN_PDM_DATA 12

// FFT/SPECTRUM CONFIGURATION
#define FFT_SIZE 64                 // Size of the FFT; must be a power of two
#define SPECTRUM_SIZE (FFT_SIZE / 2) // FFT output bins
#define SAMPLE_RATE 16000           // 16 kHz sample rate

// PDM DECIMATION PARAMETERS
#define DECIMATION 64

// NeoPixel configuration
#define NEOPIXEL_PIN 7
#define NUMPIXELS 42  // Total number of pixels

// Global brightness settings
#define MAX_BRIGHTNESS_PERCENT 10  // Changed from 20 to 10
#define NEOPIXEL_BRIGHTNESS ((MAX_BRIGHTNESS_PERCENT * 255) / 100)

// Pin definitions for LEDs
#define RED_CUP_RIGHT_PIN A5
#define RED_CUP_LEFT_PIN 28
#define WHITE_SWORD_RIGHT_PIN A0
#define WHITE_SWORD_LEFT_PIN 5

#define YELLOW_WAND_UPPER_LEFT_PIN 30
#define YELLOW_WAND_UPPER_RIGHT_PIN A1
#define YELLOW_WAND_LOWER_LEFT_PIN 29
#define YELLOW_WAND_LOWER_RIGHT_PIN A2

// Pin definitions for buttons
#define LEFT_BUTTON_PIN 9
#define RIGHT_BUTTON_PIN A4

// Other pin definitions
#define HALL_EFFECT_PIN A3

// Function prototypes

// Initialization functions
void initializePeripherals();
void initializeLEDs();
void initializeNeoPixels(Adafruit_NeoPixel &pixels);
void initializeAccelerometer(Adafruit_LIS3DH &lis);
void initializeMicrophone();
void initializeNFC();

// Magnetic sensor function
bool isMagneticFieldDetected();

// Accelerometer functions
void getAccelerometerValues(Adafruit_LIS3DH &lis, float &x, float &y, float &z);
uint8_t getAccelerometerTap(Adafruit_LIS3DH &lis);

// LED control functions
void setLED(uint8_t pin, bool state);
void setLEDGroup(const char* groupName, bool state);
void turnOffAllLEDs();
void turnOnAllLEDs();
void testAllLEDs();
bool setIndividualLED(uint8_t ledId, bool state);  // Added for individual LED control

// NeoPixel functions
void setAllNeoPixelsColor(Adafruit_NeoPixel &pixels, uint32_t color);
void flameEffect(Adafruit_NeoPixel &pixels);
void accelerometerNeoPixelDemo(Adafruit_NeoPixel &pixels);
void eyeballNeoPixelDemo(Adafruit_NeoPixel &pixels);
void colorSwirlNeoPixelDemo(Adafruit_NeoPixel &pixels);
void rainbowCycleNeoPixelDemo(Adafruit_NeoPixel &pixels);
void bouncingBallNeoPixelDemo(Adafruit_NeoPixel &pixels);
void plasmaEffectNeoPixelDemo(Adafruit_NeoPixel &pixels);
void cyberpunkGlitchNeoPixelDemo(Adafruit_NeoPixel &pixels);
void cyberpunkCircuitNeoPixelDemo(Adafruit_NeoPixel &pixels);
void solidColorMusic(Adafruit_NeoPixel &pixels);
void rainbowBeatMusic(Adafruit_NeoPixel &pixels);
void accelerometerNeoPixelDemoSmoother(Adafruit_NeoPixel &pixels);
void gameOfLifeNeoPixelDemo(Adafruit_NeoPixel &pixels);
void tetrisNeoPixelDemo(Adafruit_NeoPixel &pixels);
void fallingDropsNeoPixelDemo(Adafruit_NeoPixel &pixels);
void spiralingVortexNeoPixelDemo(Adafruit_NeoPixel &pixels);
void theaterMarqueeNeoPixelDemo(Adafruit_NeoPixel &pixels);
void neopixelsOff(Adafruit_NeoPixel &pixels);

// **Startup Sequence and Shapes**
void runStartupSequence();
void drawEyeballShape(Adafruit_NeoPixel &pixels, int x, int y);
void drawCrossShape(Adafruit_NeoPixel &pixels, int x, int y);
void drawSingleDot(Adafruit_NeoPixel &pixels, int x, int y);


// Button functions
bool isLeftButtonPressed();
bool isRightButtonPressed();
bool isBothButtonsPressed();

//void handleBothButtonsPressed();


// Utility functions
void waitForButtonPress(bool (*buttonFunction)());
void handleBothButtonsPressed();

// Test functions (functionality kept but not used in main sketch)
void testSensors(Adafruit_LIS3DH &lis);
void testNeoPixels(Adafruit_NeoPixel &pixels);

// Additional function prototypes
void recordAudio();
void processFFT();
float calculateVolume();
void displaySolidColor(Adafruit_NeoPixel &pixels, uint32_t selectedColor);
void displayRainbow(Adafruit_NeoPixel &pixels);

// Button timing and state variables
extern unsigned long lastRightButtonTime;
extern bool specialModeActive;
extern const unsigned long SPECIAL_MODE_TIMEOUT;

// Dedicated function to check special mode
bool isSpecialModeActive();

// Function to perform the chase pattern
void runChasePattern(int speed, int repeats);

// Define a type for the callback function
typedef void (*DoubleTapCallback)();

// Function to set the double tap callback
void setDoubleTapCallback(DoubleTapCallback callback);

// Function to initialize utility functions (if needed)
void initUtilityFunctions();


#endif  // UTILITYFUNCTIONS_H
