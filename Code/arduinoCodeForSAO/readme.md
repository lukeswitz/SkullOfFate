# Tarot Card Electronic Prop with NFC and LED Animations  

CODE WILL BE UPDATED.  CHECK BACK FOR UPDATES :)

Welcome to the Tarot Card Electronic Prop project! This project combines NFC technology, NeoPixel LED animations, and various sensors to create an interactive and visually appealing electronic tarot card. This prop is perfect for cosplay, interactive art installations, or educational purposes.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Installation and Setup](#installation-and-setup)
- [Usage](#usage)
- [File Structure](#file-structure)
- [Contributing](#contributing)
- [License](#license)

## Overview

This project implements an electronic tarot card using an Arduino-compatible board (Adafruit Feather M0), NFC tag writer, accelerometer, microphone, and NeoPixel LEDs. The card responds to various inputs like button presses, accelerometer movements, and sound to display different LED animations and write random NFC URLs to the embedded NFC tag.

## Features

- **NFC Tag Writing**: Writes random URLs to the NFC tag when both buttons are pressed simultaneously.
- **LED Animations**: Includes multiple NeoPixel LED animations such as flame effects, bouncing balls, plasma effects, and more.
- **Sound Reactive**: Uses a PDM microphone to create sound-reactive LED animations.
- **Accelerometer Interaction**: Changes LED animations based on accelerometer data (e.g., shaking the card).
- **Button Controls**: Left and right buttons control different aspects of the card, including changing animations and writing to the NFC tag.
- **Magnetic Sensor**: Detects magnetic fields using a hall-effect sensor for additional interactivity.
- **Configurable Settings**: Allows updating configuration parameters like default animations through code.

## Hardware Requirements

- **Adafruit Feather M0** (or compatible board)
- **Adafruit LIS3DH Accelerometer**
- **Adafruit PDM Microphone Breakout**
- **SparkFun ST25DV64KC NFC Tag**
- **NeoPixel LEDs** (42 pixels recommended)
- **Discrete LEDs**: Red, White, and Yellow LEDs for cups, swords, and wands representations
- **Buttons**: Two momentary push buttons for left and right controls
- **Hall-Effect Sensor**: For magnetic field detection
- **Wiring and Connectors**: As required for your setup

## Software Requirements

- **Arduino IDE** (version 1.8 or higher)
- **Adafruit SleepyDog Library**
- **Adafruit NeoPixel Library**
- **Adafruit LIS3DH Library**
- **Adafruit ZeroPDM Library**
- **Adafruit ZeroFFT Library**
- **SparkFun ST25DV64KC Arduino Library**

## Installation and Setup

### Hardware Setup

1. **Connect the NeoPixels**: Wire the NeoPixels to the appropriate data pin (`NEOPIXEL_PIN`), power, and ground.
2. **Connect the Accelerometer**: Wire the LIS3DH accelerometer to the I2C pins (`SDA`, `SCL`).
3. **Connect the PDM Microphone**: Connect the microphone's clock and data pins to `PIN_PDM_CLK` and `PIN_PDM_DATA`.
4. **Connect the NFC Tag**: Wire the ST25DV64KC NFC tag to the I2C bus.
5. **Connect the Buttons**: Wire the left and right buttons to `LEFT_BUTTON_PIN` and `RIGHT_BUTTON_PIN` with pull-up resistors.
6. **Connect the Hall-Effect Sensor**: Connect the sensor output to `HALL_EFFECT_PIN`.
7. **Connect the Discrete LEDs**: Wire the LEDs to their respective pins with appropriate current-limiting resistors.

### Software Setup

1. **Clone the Repository**: Download or clone this repository to your local machine.
2. **Install Arduino Libraries**: Ensure all the required libraries are installed in your Arduino IDE.
3. **Open the Sketch**: Open the main Arduino sketch file (e.g., `main.ino`) in the Arduino IDE.
4. **Configure Settings**: Adjust any configuration parameters as needed in the `ConfigManager` or directly in the code.
5. **Upload the Sketch**: Connect your Arduino board and upload the sketch.

## Usage

Once everything is set up:

- **Power On**: Turn on the device; it will run the default animation.
- **Change Animations**: Use the right button to cycle through different LED animations.
- **Set Default Animation**: Long-press the right button to set the current animation as the default on startup.
- **Write NFC Tag**: Press both the left and right buttons simultaneously to write a random URL to the NFC tag.
- **Interactivity**:
  - **Accelerometer**: Shake or tilt the card to interact with accelerometer-based animations.
  - **Microphone**: Play sounds or music to interact with sound-reactive animations.
  - **Magnetic Sensor**: Bring a magnet close to the hall-effect sensor to trigger specific behaviors.

## File Structure

### Main Sketch and Configuration

- **`main.ino`**: The primary Arduino sketch that initializes peripherals and runs the main loop.
- **`ConfigManager.h`** and **`ConfigManager.cpp`**: Manages configuration settings for the device.

### NFC Functionality

- **`NFCWriter.h`** and **`NFCWriter.cpp`**: Handles NFC tag initialization, wiping, and writing random URLs.
- **`SparkFun_ST25DV64KC_Arduino_Library.h`**: Library for interacting with the ST25DV64KC NFC tag.

### Utility Functions and Animations

- **`UtilityFunctions.h`** and **`UtilityFunctions.cpp`**: Contains utility functions, LED animations, and sensor interactions.

### Libraries

- **Adafruit and SparkFun Libraries**: Required for hardware components like NeoPixels, accelerometer, microphone, and NFC tag.

## Key Functions and Classes

### NFCWriter Class

Handles NFC tag operations:

- **`initializeTag()`**: Initializes the NFC tag.
- **`wipeEEPROM()`**: Clears the EEPROM of the NFC tag.
- **`writeCCFile()`**: Writes the Capability Container file.
- **`writeRandomURI()`**: Writes a randomly generated URI to the NFC tag.
- **`verifyWrite(uint16_t totalSize)`**: Reads back and verifies the written data.

### Utility Functions

- **LED Control**: Functions to control individual LEDs or groups (`cups`, `swords`, `wands`).
- **NeoPixel Animations**: Multiple animations like `flameEffect`, `rainbowCycleNeoPixelDemo`, `solidColorMusic`, etc.
- **Sensor Interactions**: Functions to read accelerometer values, detect magnetic fields, and process microphone input.
- **Button Handling**: Debounce logic and press duration detection for buttons.

### Animations

- **Flame Effect**: Simulates a flame using the `HeatColor` function and grid mapping.
- **Rainbow Cycle**: Displays a cycling rainbow across all NeoPixels.
- **Bouncing Ball**: Animates a ball bouncing across the NeoPixels.
- **Plasma Effect**: Creates a plasma-like animation using sine functions.
- **Sound-Reactive Animations**: Adjusts LED brightness and color based on sound input from the microphone.
- **Game of Life**: Runs Conway's Game of Life on the NeoPixel grid.
- **Tetris Animation**: Simulates falling Tetris pieces.
- **Falling Drops**: Animates droplets falling down the grid.
- **Eyeball Animation**: Simulates an eyeball moving around the grid.
- **Theater Marquee**: Creates a theater-style chasing lights effect.

## Contributing

Contributions are welcome! Please follow these steps:

1. **Fork the Repository**: Create your own fork of the project.
2. **Create a Branch**: For your feature or bug fix.
3. **Commit Changes**: Make your changes and commit them with clear messages.
4. **Submit a Pull Request**: Explain your changes and submit a PR for review.

## License

This project is licensed under the [MIT License](LICENSE).

---

Feel free to reach out if you have any questions or need assistance setting up the project. Enjoy your interactive electronic tarot card!
