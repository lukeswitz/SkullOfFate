# Skull of Fate Integration with Raspberry Pi Pico W Badge

This guide provides instructions and explanations for integrating the **Skull of Fate** I2C client with a Raspberry Pi Pico W badge. The provided `boot.py` and `main.py` scripts include new functionalities, such as interacting with the Skull of Fate device, handling serial prompts, and customizing button actions.  

CHECK BACK FOR UPDATES!

## Table of Contents

1. [Introduction](#introduction)
2. [Hardware Setup](#hardware-setup)
   - [SAO Positions and Pin Assignments](#sao-positions-and-pin-assignments)
3. [Software Setup](#software-setup)
   - [Installing Thonny IDE](#installing-thonny-ide)
   - [Uploading Code to Raspberry Pi Pico W](#uploading-code-to-raspberry-pi-pico-w)
4. [Code Overview](#code-overview)
   - [Summary of `boot.py`](#summary-of-bootpy)
   - [Summary of `main.py`](#summary-of-mainpy)
   - [Differences from Original Code](#differences-from-original-code)
5. [Usage](#usage)
   - [Using Serial Prompts](#using-serial-prompts)
   - [Button Presses](#button-presses)
   - [Custom Integrations](#custom-integrations)
6. [Conclusion](#conclusion)
7. [Appendix](#appendix)
   - [Full `boot.py` Code](#full-bootpy-code)
   - [Full `main.py` Code](#full-mainpy-code)

---

## Introduction

The Raspberry Pi Pico W badge can be extended with various SAOs (Shitty Add-Ons). This guide focuses on integrating the **Skull of Fate** I2C client, enhancing the badge's capabilities with new I2C commands, serial interactions, and button mappings.

---

## Hardware Setup

### SAO Positions and Pin Assignments

The badge has **six SAO positions** where you can connect the Skull of Fate. Each position corresponds to specific GPIO pins on the Pico W.

**SAO Positions and Clock Orientation:**

- **Position 1**: 11 o'clock
- **Position 2**: 9 o'clock
- **Position 3**: 7 o'clock
- **Position 4**: 5 o'clock
- **Position 5**: 3 o'clock
- **Position 6**: 1 o'clock

When looking at the main badge, Position 1 starts at the 11 o'clock position, and the positions increase in a clockwise direction.

**Pin Assignments for Each Position:**

- **Position 1**
  - `SOF_INPUT_PIN_NUMBER = 6`
  - `NEOPIXEL_PIN_NUMBER = 7`
- **Position 2**
  - `SOF_INPUT_PIN_NUMBER = 4`
  - `NEOPIXEL_PIN_NUMBER = 5`
- **Position 3**
  - `SOF_INPUT_PIN_NUMBER = 2`
  - `NEOPIXEL_PIN_NUMBER = 3`
- **Position 4**
  - `SOF_INPUT_PIN_NUMBER = 21`
  - `NEOPIXEL_PIN_NUMBER = 22`
- **Position 5**
  - `SOF_INPUT_PIN_NUMBER = 19`
  - `NEOPIXEL_PIN_NUMBER = 20`
- **Position 6** (Default in the provided code)
  - `SOF_INPUT_PIN_NUMBER = 17`
  - `NEOPIXEL_PIN_NUMBER = 18`

**Important:** When connecting the Skull of Fate to a different SAO position, update the `SOF_INPUT_PIN_NUMBER` and `NEOPIXEL_PIN_NUMBER` in `main.py` to match the correct GPIO pins for that position.

---

## Software Setup

### Installing Thonny IDE

We recommend using **Thonny IDE** to upload the scripts to your Raspberry Pi Pico W.

**Steps to Install Thonny:**

1. **Download Thonny:** Visit the [official Thonny website](https://thonny.org/) and download the installer for your operating system.
2. **Install Thonny:** Run the installer and follow the on-screen instructions.
3. **Set Up Thonny for Pico W:**
   - Connect your Raspberry Pi Pico W to your computer via USB.
   - Open Thonny.
   - Go to `Tools` > `Options...` > `Interpreter`.
   - Select `MicroPython (Raspberry Pi Pico)` as the interpreter.
   - Ensure the correct port is selected (Thonny usually auto-detects it).
   - Click `OK`.

**Additional Resources:**

- [Getting Started with Thonny and Pico W](https://projects.raspberrypi.org/en/projects/getting-started-with-the-pico/3)

### Uploading Code to Raspberry Pi Pico W

1. **Open `boot.py` in Thonny:**
   - Copy the contents of `boot.py` (provided below) and paste it into a new file in Thonny.
2. **Save `boot.py` to Pico W:**
   - Go to `File` > `Save as...`.
   - Select `MicroPython device`.
   - Save the file as `boot.py`.
3. **Repeat for `main.py`:**
   - Open a new file in Thonny.
   - Paste the contents of `main.py` (provided below).
   - Save it to the Pico W as `main.py`.

**Note:** Ensure both files are saved directly on the Pico W for them to execute properly.

---

## Code Overview

### Summary of `boot.py`

The `boot.py` script initializes peripherals and sets up I2C devices during the boot process.

**Key Functions:**

- **I2C Addresses:**
  - `PETAL_ADDRESS`: Address for the Petal SAO.
  - `TOUCHWHEEL_ADDRESS`: Address for the Touchwheel SAO.
  - `SKULL_OF_FATE_ADDRESS`: Address for the Skull of Fate (changed from `0x12` to `0x13`).

- **Initialization:**
  - Onboard LED is turned on to indicate the boot process.
  - Buttons A, B, and C are initialized with internal pull-up resistors.
  - GPIO pins for each SAO position are set up.
  - Two I2C buses (`i2c0` and `i2c1`) are initialized.
  - The script attempts to initialize the Petal SAO, Touchwheel SAO, and Skull of Fate device by scanning the I2C buses.

- **Functions:**
  - `petal_init(bus)`: Configures the Petal SAO.
  - `touchwheel_read(bus)`: Reads the touchwheel position.
  - `touchwheel_rgb(bus, r, g, b)`: Sets the RGB color on the touchwheel.

### Summary of `main.py`

The `main.py` script contains the main execution loop and handles user interactions.

**Key Features:**

- **Imports and Pin Configuration:**
  - Imports necessary modules and functions from `boot.py`.
  - Sets the `SOF_INPUT_PIN_NUMBER` and `NEOPIXEL_PIN_NUMBER` based on the SAO position (default is Position 6).

- **Initialization:**
  - Initializes the NeoPixel and onboard LED.
  - Defines helper functions for setting NeoPixel color and onboard LED state.
  - Defines `send_i2c_command` to communicate with I2C devices.

- **Main Loop:**
  - Handles button presses:
    - **Button A:** Sends command `'1'` with `255` to trigger a random animation.
    - **Button B:** Sends command `'4'` to request accelerometer data.
    - **Button C:** Sends command `'2'` to trigger a Tarot Card Reading.
  - Handles NeoPixel color changes based on the Skull of Fate input pin.
  - Reads the touchwheel position and updates the Petal SAO accordingly.
  - Provides a serial input interface for sending custom commands.

- **Functions:**
  - `send_i2c_command(command_char, data_bytes=None, response_length=1)`: Sends I2C commands and processes responses.
  - `decode_response(command_char, data)`: Decodes responses from the Skull of Fate.

### Differences from Original Code

- **Skull of Fate Integration:**
  - Added support for detecting and communicating with the Skull of Fate I2C client.
  - Changed `SKULL_OF_FATE_ADDRESS` from `0x12` to `0x13`.

- **Button Mappings:**
  - Buttons A, B, and C now send specific I2C commands to the Skull of Fate.

- **NeoPixel and Input Pin Handling:**
  - Integrated NeoPixel control and input pin reading from the Skull of Fate.

- **Serial Command Interface:**
  - Enhanced serial prompts for sending various commands to the Skull of Fate.

---

## Usage

### Using Serial Prompts

**Accessing the Serial Console:**

1. Open Thonny IDE.
2. Ensure your Pico W is connected and recognized by Thonny.
3. Use the **Shell** at the bottom of Thonny to interact with the Pico W.

**Available Commands:**

- **`0`**: Stop all effects and turn off LEDs.
- **`1 <ANIMATION_INDEX>`**: Change current animation index.
  - `ANIMATION_INDEX` ranges from `0` to `255`.
  - Use `255` to trigger a random animation.
- **`2`**: Trigger Tarot Card Reading.
- **`3`**: Request magnetic sensor data.
- **`4`**: Request accelerometer data.
- **`5`**: Request button states.
- **`6`**: Request PDM microphone data.
- **`C <LED_ID> <STATE>`**: Control individual LEDs.
  - `LED_ID`: LED number (`1` to `9`).
  - `STATE`: `1` (ON) or `0` (OFF).

**Examples:**

- **Trigger a random animation:**
  ```
  1 255
  ```
- **Turn on LED 5:**
  ```
  C 5 1
  ```
- **Request accelerometer data:**
  ```
  4
  ```

### Button Presses

- **Button A** (`buttonA`):
  - Sends command `'1'` with data `255` to trigger a random animation on the Skull of Fate.
- **Button B** (`buttonB`):
  - Sends command `'4'` to request accelerometer data.
- **Button C** (`buttonC`):
  - Sends command `'2'` to trigger a Tarot Card Reading.

**Note:** Buttons are active LOW, meaning they are considered "pressed" when the input reads `0`.

### Custom Integrations

#### Modifying the Code for Other I2C Commands

You can extend the functionality by adding new I2C commands.

**Steps:**

1. **Define New Commands:**
   - In `send_i2c_command`, add handling for your new command character.
   - In `decode_response`, define how to process the response.

2. **Update Serial Interface:**
   - Modify the serial input section to recognize new commands.

3. **Example:**
   - To add command `'D'` that does something new:
     ```python
     # In send_i2c_command
     send_i2c_command('D', response_length=desired_length)
     ```

     ```python
     # In decode_response
     elif command_char == 'D':
         # Process the response for command 'D'
         pass
     ```

#### Remapping Buttons

You can remap the buttons to perform different actions or send different commands.

**Steps:**

1. **Locate Button Handling Section:**
   - In `main.py`, find the section starting with `# Handle Button A`.

2. **Modify Button Actions:**
   - Change the `send_i2c_command` parameters to send your desired commands.

3. **Example:**
   - To make **Button A** send command `'5'`:
     ```python
     # Original
     send_i2c_command('1', bytes([animation_index]))
     # Modified
     send_i2c_command('5', response_length=1)
     ```

---

## Conclusion

With these scripts and instructions, you can enhance your Raspberry Pi Pico W badge by integrating the Skull of Fate I2C client and customizing its functionalities. Feel free to modify and expand upon the provided code to suit your project needs.

---

Feel free to modify and extend the code to suit your project's needs. Happy hacking!
