// skull_of_fate_v1.ino

#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_LIS3DH.h>
#include "UtilityFunctions.h"
#include "NFCWriter.h"
#include "SparkFun_ST25DV64KC_Arduino_Library.h" // Include the NFC library
#include "wiring_private.h"  // Include this header for pinPeripheral
#include <Adafruit_SleepyDog.h>
#include "ConfigManager.h"   // Include ConfigManager

#include <SPI.h>
#include <SdFat.h>
#include <ArduinoJson.h>
#include <Adafruit_SPIFlash.h>
#include "flash_config.h"    // Include your flash configuration

// Initialize the flash transport via flash_config.h
// flash_config.h defines 'flashTransport'
Adafruit_SPIFlash flash(&flashTransport);  // This object is defined in flash_config.h

// Instantiate ConfigManager with the flash object
ConfigManager configManager(flash);

// Declare the ConfigManager instance as extern
// Already handled via 'extern ConfigManager configManager;' in ConfigManager.h

// Configuration parameters
Config currentConfig;



// Watchdog timer
int watchdogTimerInSeconds = 10;

Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Accelerometer
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

// Variables for animations
int chaseSpeed;
int chaseRepeats;
int watchdogTimeout;

// Function prototypes
void updateConfigParameterInt(const String& key, int value);

volatile bool animationInterrupted = false;

// NFC Tag and Writer instances
SFE_ST25DV64KC tag;       // Create an instance of the ST25DV64KC NDEF class
NFCWriter nfcWriter(tag); // Create an instance of NFCWriter

// Variables for the chase pattern
int animationIndex = 0; // current animation index

// Function pointer array for animations
void (*animations[])(Adafruit_NeoPixel&) = {
  eyeballNeoPixelDemo,
  accelerometerNeoPixelDemoSmoother,
  solidColorMusic,
  rainbowBeatMusic,
  flameEffect,
  colorSwirlNeoPixelDemo,
  bouncingBallNeoPixelDemo,
  rainbowCycleNeoPixelDemo,
  plasmaEffectNeoPixelDemo,
  cyberpunkGlitchNeoPixelDemo,
  cyberpunkCircuitNeoPixelDemo,
  gameOfLifeNeoPixelDemo,
  tetrisNeoPixelDemo,
  fallingDropsNeoPixelDemo,
  spiralingVortexNeoPixelDemo,
  theaterMarqueeNeoPixelDemo,
  neopixelsOff
};

const int numAnimations = sizeof(animations) / sizeof(animations[0]); // Update numAnimations

// Variable to keep track of the current animation function
void (*currentAnimation)(Adafruit_NeoPixel&) = eyeballNeoPixelDemo;

// Variables for button states
bool lastLeftButtonState = HIGH;
bool lastRightButtonState = HIGH;

// Variable to store if both buttons were pressed
bool bothButtonsPressed = false;

// Store the previous animation to resume after both buttons are pressed
void (*previousAnimation)(Adafruit_NeoPixel&) = NULL;

// I2C client Address
#define DEFAULT_CLIENT_ADDRESS 0x13
#define ALT1_CLIENT_ADDRESS 0x13
#define ALT2_CLIENT_ADDRESS 0x14
#define ALT3_CLIENT_ADDRESS 0x15

// I2C client Address
//#define CLIENT_ADDRESS 0x12  // Ensure this matches the host's client_ADDRESS
int CLIENT_ADDRESS = DEFAULT_CLIENT_ADDRESS;

// Create a new TwoWire instance on SERCOM1, with SDA on pin 11 and SCL on pin 13
TwoWire myWire(&sercom1, 11, 13);

void SERCOM1_Handler() {
  myWire.onService();
}

// Buffer to store dynamic messages
const int MAX_MESSAGE_LENGTH = 512;  // Adjust based on maximum expected data
char receivedMessage[MAX_MESSAGE_LENGTH];
int messageLength = 0;

// Variable to store the last requested sensor data
char lastRequestedSensor = 0;


// Function to switch animations
void switchAnimation(int index) {
    if(index >= 0 && index < numAnimations){
        currentAnimation = animations[index];
        animationInterrupted = true; // Signal the current animation to exit
        Serial.print("Switched to animation index ");
        Serial.println(index);
        // Update the 'defaultAnimation' in the configuration if needed
        // updateConfigParameterInt("defaultAnimation", index);
    } else {
        Serial.println("Invalid animation index.");
    }
}


// Callback function to handle double tap
void onDoubleTap() {
    // animationIndex = (animationIndex + 1) % numAnimations;
    // Get the current configuration
    currentConfig = configManager.getConfig();
    animationIndex = currentConfig.defaultAnimation;
    switchAnimation(animationIndex);
}

/*
// Function to handle configuration updates
void handleConfigUpdate(const String& key, int value) {
  if (configManager.updateConfig(key, value)) {
    Serial.println(F("Configuration updated successfully."));
    currentConfig = configManager.getConfig();
    // Apply updated configurations as needed
    // For example:
    // Update chaseSpeed based on new config if applicable
    // chaseSpeed = currentConfig.watchdogmaxtimeout; // Example usage
    // Add more parameter applications here as needed
  } else {
    Serial.println(F("Failed to update configuration."));
  }
}
*/
/*
// Function to handle configuration updates
void handleConfigUpdate(const String& key, const JsonVariant& value) {
  if (configManager.updateConfig(key, value)) {
    Serial.println(F("Configuration updated successfully."));
    currentConfig = configManager.getConfig();
    // Apply updated configurations as needed
    // For example:
    //chaseSpeed = currentConfig.watchdogmaxtimeout; // Example usage
    // Add more parameter applications here
  } else {
    Serial.println(F("Failed to update configuration."));
  }
}
*/
void setup() {

    // Initialize peripherals
  initializePeripherals();

     // Initialize LEDs and NeoPixels
 // initializeLEDs();
  initializeNeoPixels(pixels);

    // Perform the startup sequence
  runStartupSequence();

  initializeNFC();
  //Serial.println("System initialized.");

  // Initialize ConfigManager
  if (!configManager.initialize()) {
    Serial.println(F("Configuration Manager initialization failed. Halting."));
    while (1) { yield(); }
  }

  // Get the current configuration
  currentConfig = configManager.getConfig();

  // Apply configuration parameters
  chaseSpeed = 100; // You can set this based on config if available
  chaseRepeats = 2; // Similarly, set based on config if available
  watchdogTimeout = currentConfig.watchdogmaxtimeout / 1000; // Convert ms to seconds



  setAllNeoPixelsColor(pixels, 0);  // Turn off all NeoPixels

  // Initialize accelerometer
  initializeAccelerometer(lis);

  // Setup I2C
  // myWire.begin(currentConfig.defaulti2cAddress);
  // myWire.onReceive(receiveEvent);
  // myWire.onRequest(requestEvent);

  int address0 = currentConfig.defaulti2cAddress;
  if (address0 >= 0x08 && address0 <= 0x77) {
      
      Serial.print("0x");
      Serial.print(currentConfig.defaulti2cAddress, HEX);
      Serial.println(" - current config defaulti2cAddress is in a valid I2C range and assigning as I2C client address.");
      CLIENT_ADDRESS = address0;
  } else {
      Serial.println("current config defaulti2cAddress is out of range or invalid.");
  }

  
    
    if (isBothButtonsPressed()) {
      setIndividualLED(1, true);
      // CLIENT_ADDRESS = ALT3_CLIENT_ADDRESS;
      // Serial.print("Updating config defaulti2cAddress to: ");
      // Serial.println(CLIENT_ADDRESS, HEX);
      // updateConfigParameterInt("defaulti2cAddress", CLIENT_ADDRESS);

      int address3 = currentConfig.alternatei2cAddress3;
      if (address3 >= 0x08 && address3 <= 0x77) {
          Serial.print("0x");
          Serial.print(address3, HEX);
          Serial.println(" - current config alternatei2cAddress3 is in a valid I2C range and assigning/saving as I2C client address.");
          updateConfigParameterInt("defaulti2cAddress", address3);
          CLIENT_ADDRESS = address3;
      } else {
          Serial.print("current config alternatei2cAddress1 is out of range or invalid. Assigning the following address: 0x");
          Serial.print(ALT3_CLIENT_ADDRESS, HEX);
          CLIENT_ADDRESS = ALT3_CLIENT_ADDRESS;
      }
  
      delay(1000);
  } else {
  
      if (isLeftButtonPressed()) {
        setIndividualLED(2, true);
          // Left button was just pressed
          int address1 = currentConfig.alternatei2cAddress1;
          if (address1 >= 0x08 && address1 <= 0x77) {
              Serial.print("0x");
              Serial.print(address1, HEX);
              Serial.println(" - current config alternatei2cAddress1 is in a valid I2C range and assigning as I2C client address.");
              CLIENT_ADDRESS = address1;
              updateConfigParameterInt("defaulti2cAddress", address1);
          } else {
              Serial.print("current config alternatei2cAddress1 is out of range or invalid. Assigning the following address: 0x");
              Serial.print(ALT1_CLIENT_ADDRESS, HEX);
              CLIENT_ADDRESS = ALT1_CLIENT_ADDRESS;
          }
      
          delay(1000);
      }
      else if (isRightButtonPressed()) {
        setIndividualLED(3, true);
          // Right button was just pressed
          int address2 = currentConfig.alternatei2cAddress2;
          if (address2 >= 0x08 && address2 <= 0x77) {
              Serial.print("0x");
              Serial.print(address2, HEX);
              Serial.println(" - current config alternatei2cAddress2 is in a valid I2C range and assigning as I2C client address.");
              CLIENT_ADDRESS = address2;
              updateConfigParameterInt("defaulti2cAddress", address2);
          } else {
              Serial.print("current config alternatei2cAddress2 is out of range or invalid. Assigning the following address: 0x");
              Serial.print(ALT2_CLIENT_ADDRESS, HEX);
              CLIENT_ADDRESS = ALT2_CLIENT_ADDRESS;
          }
      
          delay(1000);
          
      }
  }




  Serial.print("I2C Client address set to: 0x");
  Serial.println(CLIENT_ADDRESS, HEX);
  
  

  //delay(1000);

     // Initialize I2C as client
  myWire.begin(CLIENT_ADDRESS);
  myWire.onReceive(receiveEvent);
  myWire.onRequest(requestEvent);

    // Assign SDA and SCL functions to specific pins (SERCOM setup)
  pinPeripheral(11, PIO_SERCOM); // SDA
  pinPeripheral(13, PIO_SERCOM); // SCL



  // Start with the Eyeball NeoPixel Demo
  //currentAnimation = eyeballNeoPixelDemo;

 // currentAnimation = 
 Serial.print("Loading Default Animation Index: ");
  Serial.println(currentConfig.defaultAnimation);
 // Serial.println(" milliseconds!");
  currentAnimation = animations[currentConfig.defaultAnimation];
  animationIndex = currentConfig.defaultAnimation;


   // Initialize watchdog timer
  Watchdog.enable(watchdogTimeout * 1000);
  Serial.print("Enabled the watchdog with max countdown of ");
  Serial.print(watchdogTimeout * 1000);
  Serial.println(" milliseconds!");

  setDoubleTapCallback(onDoubleTap);

  configManager.printConfig();

    // Turn off all LEDs at start
  turnOffAllLEDs();
}

void loop() {
  // Check for both buttons pressed
  if (isBothButtonsPressed()) {
    handleBothButtonsPressed();
    Watchdog.reset();
  } else {
    if (isLeftButtonPressed()) {
      // Left button was just pressed
      //configManager.printConfig();
      advanceAnimation();
      Watchdog.reset();
      delay(200); // Reduced debounce delay
    }
    // Run the current animation
    if (currentAnimation != NULL) {
      currentAnimation(pixels);
      animationInterrupted = false; // Reset the flag after animation exits
      Watchdog.reset();
    }
  }
  // Reset watchdog with every loop to make sure the sketch keeps running.
  Watchdog.reset();
}

// Function to handle received I2C commands
void receiveEvent(int howMany) {
  if (howMany <= 0) return;

  char command = myWire.read();
  Serial.print("Command received: ");
  Serial.println(command);

  // Initialize variables for data reception
  uint8_t dataBuffer[howMany - 1];
  int dataLength = 0;

  // Read the remaining bytes as data
  while (myWire.available()) {
    if (dataLength < (howMany - 1)) {
      dataBuffer[dataLength++] = myWire.read();
    } else {
      myWire.read();  // Discard any extra bytes
    }
  }

  handleCommand(command, dataBuffer, dataLength);
}

// Callback function executed when the host requests data
void requestEvent() {
  // Send sensor data based on the last requested sensor
  switch (lastRequestedSensor) {
    case '3': {
      // Magnetic sensor data
      bool magneticField = isMagneticFieldDetected();
      Serial.println(magneticField);
      if (magneticField) {
        myWire.write(1); // Magnetic field detected
      } else {
        myWire.write(0); // No magnetic field
      }
      Serial.println("Magnetic sensor value sent.");
      Watchdog.reset();
      break;
    }
    case '4': {
      // Accelerometer data
      float x, y, z;
      getAccelerometerValues(lis, x, y, z);
      myWire.write((byte*)&x, sizeof(x));
      myWire.write((byte*)&y, sizeof(y));
      myWire.write((byte*)&z, sizeof(z));
      Serial.println("Accelerometer data sent.");
      Watchdog.reset();
      break;
    }
    case '5': {
      // Button states
      uint8_t buttons = 0;
      if (isLeftButtonPressed()) buttons |= 0x01;
      if (isRightButtonPressed()) buttons |= 0x02;
      myWire.write(buttons);
      Serial.println("Button states sent.");
      Watchdog.reset();
      break;
    }
    case '6': {
      // PDM microphone data (e.g., volume level)
      float volume = getMicrophoneVolume();
      myWire.write((byte*)&volume, sizeof(volume));
      Serial.println("PDM microphone data sent.");
      Watchdog.reset();
      break;
    }
    default:
      // Send a dummy byte if no valid sensor was requested
      myWire.write('A');
      Serial.println("Default data sent.");
      Watchdog.reset();
      break;
  }
}

// Function to handle different commands
void handleCommand(char command, uint8_t* data, int length) {
  // Store the last requested sensor command
  lastRequestedSensor = command;

  switch (command) {
    case '0':
      // Stop all effects and turn off LEDs
      turnOffAllLEDs();   // Turn off all individual LEDs
      setAllNeoPixelsColor(pixels, 0);  // Turn off all NeoPixels
      currentAnimation = animations[numAnimations - 1];
      animationInterrupted = true;
      Serial.println("All effects stopped, LEDs turned OFF");
      Watchdog.reset();
      break;
    case '1':
      // Change current animation index
      if (length >= 1) {
          uint8_t newIndex = data[0];
          if (newIndex == 255) { // Random animation
              animationIndex = random(0, numAnimations-1);
          } else if (newIndex < numAnimations) {
              animationIndex = newIndex;
          } else {
              Serial.println("Error: Invalid animation index.");
              break;
          }
          currentAnimation = animations[animationIndex];
          animationInterrupted = true; // Signal the current animation to exit
          Serial.print("Switched to animation index ");
          Serial.println(animationIndex);
          // Update the 'defaultAnimation' in the configuration
          // updateConfigParameterInt("defaultAnimation", animationIndex);
          
      } else {
          Serial.println("Error: Animation index not provided.");
      }
      Watchdog.reset();
      break;

    case '2':
      // Trigger handleBothButtonsPressed function
      handleBothButtonsPressed();
      Serial.println("handleBothButtonsPressed function triggered via I2C.");
      Watchdog.reset();
      break;
    case '3':
      // Request to send magnetic sensor data
      Serial.println("Magnetic sensor data requested.");
      // Data will be sent in requestEvent
      Watchdog.reset();
      break;
    case '4':
      // Request to send accelerometer data
      Serial.println("Accelerometer data requested.");
      // Data will be sent in requestEvent
      Watchdog.reset();
      break;
    case '5':
      // Request to send button states
      Serial.println("Button states requested.");
      // Data will be sent in requestEvent
      Watchdog.reset();
      break;
    case '6':
      // Request to send PDM microphone data
      Serial.println("PDM microphone data requested.");
      // Data will be sent in requestEvent
      Watchdog.reset();
      break;
    case '7':
      // Placeholder command 1
      Serial.println("Placeholder command 1 received.");
      // Add your code here
      Watchdog.reset();
      break;
    case '8':
      // Placeholder command 2
      Serial.println("Placeholder command 2 received.");
      // Add your code here
      Watchdog.reset();
      break;
    case '9':
      // Placeholder command 3
      Serial.println("Placeholder command 3 received.");
      // Add your code here
      Watchdog.reset();
      break;
    case 'A':
    case 'a':
      // Placeholder command 4
      Serial.println("Placeholder command 4 received.");
      // Add your code here
      Watchdog.reset();
      break;
    case 'B':
    case 'b':
      // Placeholder command 5
      Serial.println("Placeholder command 5 received.");
      // Add your code here
      Watchdog.reset();
      break;
    case 'C':
    case 'c':
      // Control Individual LED (Format: C <LED_ID> <STATE>)
      if (length >= 2) {  // Expecting two parameters: LED ID and State
        uint8_t ledId = data[0];
        uint8_t stateByte = data[1];
        bool state = false;
        if (stateByte == 1) {
          state = true;
        } else if (stateByte == 0) {
          state = false;
        } else {
          Serial.print("Invalid state parameter: ");
          Serial.println(stateByte);
          Watchdog.reset();
          break;
        }

        Serial.print("Controlling LED ID ");
        Serial.print(ledId);
        Serial.print(" to ");
        Serial.println(state ? "ON" : "OFF");

        if (setIndividualLED(ledId, state)) {
          Serial.println("LED control successful.");
        } else {
          Serial.println("LED control failed.");
        }
      } else {
        Serial.println("Error: Insufficient parameters for LED control.");
      }
      Watchdog.reset();
    case 'U':  // Example command to update config parameter
      // Format: U <key_length> <key> <value>
      if (length >= 3) { // At least key_length, key, and value
        uint8_t keyLength = data[0];
        if (length < 1 + keyLength + 1) {
          Serial.println("Error: Insufficient data for update command.");
          break;
        }
        String key = "";
        for (int i = 1; i <= keyLength; i++) {
          key += (char)data[i];
        }
        // Assuming the value is an integer for simplicity
        int value = 0;
        for (int i = 1 + keyLength; i < length; i++) {
          value = (value << 8) | data[i];
        }
        Serial.print("Updating config key '");
        Serial.print(key);
        Serial.print("' to value ");
        Serial.println(value);
//        handleConfigUpdate(key, value);
      } else {
        Serial.println("Error: Invalid update command format.");
      }
      Watchdog.reset();
      break;
    default:
      Serial.println("Unknown command received");
      Watchdog.reset();
      break;
  }
}

// Function to advance to the next animation
void advanceAnimation() {
  // Advance the animation index
  animationIndex = (animationIndex + 1) % numAnimations;
  currentAnimation = animations[animationIndex];
  Serial.print("Switched to animation index ");
  Serial.println(animationIndex);
  // Update the 'defaultAnimation' in the configuration
 // updateConfigParameterInt("defaultAnimation", animationIndex);
  Watchdog.reset();
}

// Function to run the startup sequence
void runStartupSequence() {
  const int delayTime = 150; // Delay time in milliseconds (can be adjusted)
  const int finalDelayTime = 500; // Final delay time before proceeding

  // Turn on Cups
  setLEDGroup("cups", HIGH);
  delay(delayTime);
  setLEDGroup("cups", LOW);

  // Turn on Swords
  setLEDGroup("swords", HIGH);
  delay(delayTime);
  setLEDGroup("swords", LOW);

  // Turn on Wands
  setLEDGroup("wands", HIGH);
  delay(delayTime);
  setLEDGroup("wands", LOW);

  // Turn on Cups and keep them on
  setLEDGroup("cups", HIGH);
  // Turn on Swords
  setLEDGroup("swords", HIGH);
  // Turn on Wands
  setLEDGroup("wands", HIGH);

  // Turn on all NeoPixels white
  setAllNeoPixelsColor(pixels, pixels.Color(255, 255, 255)); // White color
  pixels.show();

  // Wait for final delay time
  delay(finalDelayTime);

  // Turn off all LEDs and NeoPixels
  turnOffAllLEDs();
  setAllNeoPixelsColor(pixels, 0); // Turn off NeoPixels
  pixels.show();
  Watchdog.reset();
}

// Function to perform the chase pattern
void runChasePattern(int speed, int repeats) {
  // Define the sequence of LEDs
  const int sequenceLength = 8;
  const int ledSequence[sequenceLength] = {
    YELLOW_WAND_LOWER_LEFT_PIN,  // bottom left wand
    WHITE_SWORD_LEFT_PIN,        // left sword
    RED_CUP_LEFT_PIN,            // left cup
    YELLOW_WAND_UPPER_LEFT_PIN,  // upper left wand
    YELLOW_WAND_UPPER_RIGHT_PIN, // upper right wand
    RED_CUP_RIGHT_PIN,           // right cup
    WHITE_SWORD_RIGHT_PIN,       // right sword
    YELLOW_WAND_LOWER_RIGHT_PIN  // bottom right wand
  };

  for (int r = 0; r < repeats; r++) {
    for (int i = 0; i < sequenceLength; i++) {
      // Turn on the current LED
      digitalWrite(ledSequence[i], HIGH);
      // Optionally, turn off the previous LED (if not the first one)
      if (i > 0) {
        digitalWrite(ledSequence[i - 1], LOW);
      }
      else if (i == 0 && r > 0) {
        // Turn off the last LED from the previous repeat
        digitalWrite(ledSequence[sequenceLength - 1], LOW);
      }
      delay(speed);
    }
    // Turn off the last LED in the sequence
    digitalWrite(ledSequence[sequenceLength - 1], LOW);
  }
  Watchdog.reset();
}

// Function to get microphone volume level
float getMicrophoneVolume() {
  // Record audio data
  recordAudio();

  // Process FFT on the recorded audio
  processFFT();

  // Calculate the overall volume from the spectrum
  float volume = calculateVolume();
  Watchdog.reset();
  return volume;
}
