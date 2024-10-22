# Skull of Fate

Alone late at night in your castle, you reach out and grab the cobweb-covered head sitting on your mantle. Was this the head of a fierce enemy defeated in battle, a villager slain in one of your many conquests, a fallen comrade, a spiteful lover? You gaze into the black hollow pits where its eyes once were and start to fade in and out of consciousness, as if in a trance. Visions of the future dizzyingly pass by. Fires rage, bodies are strewn, dragons take flight.

**What future will you choose?**

## Table of Contents

- [Design and Features](#design-and-features)
- [Components and Materials](#components-and-materials)
- [Files Included](#files-included)
- [Demo Video](#demo-video)
- [SAO as an I2C Client](#sao-as-an-i2c-client)
- [SAO and Badge Interaction](#sao-and-badge-interaction)
- [Additional Background](#additional-background)
- [License](#license)

---

## Design and Features

The **Skull of Fate** SAO (Standard Add-On) is a modern take on a tarot card, inspired by ancient Greek mythology. The Moirai—also known as the Fates—were the personifications of destiny. They were three sisters:

- **Clotho** (the spinner)
- **Lachesis** (the allotter)
- **Atropos** (the inevitable, a metaphor for death)

The **Skull of Fate** represents **Atropos**, providing you with a unique tarot reading about the future. Each reading is an interpretation of the cards and their meanings from the original 78-card Rider Waite Tarot deck.

**Features:**

- **Original Artwork**: The card features original artwork inspired by ancient mythology.
- **Tarot Readings**: 78 unique tarot readings accessible via NFC.
- **LED Animations**: 25 different eye animations in a variety of colors.

**Modes of Operation:**

- **Default Mode**: After turning on, you can cycle through eye animations with the **left button** and optionally change the color of the LEDs with the **right button**.
  - Some LED eye modes are special and change based on the accelerometer or microphone input, making them reactive to position and sound.
- **Tarot Card Mode**: Holding down **both buttons** at the same time puts the device in tarot card mode, where a card is "drawn" at random from the deck and can be read by a phone or other NFC devices.
  - The NFC antenna is located at the upper part of the card.
  - Simply hold the reading device above the card any time after pressing both buttons to receive your reading.

---

## Components and Materials

- **Microcontroller**: SAMD21 (Arduino compatible)
- **Accelerometer**: Adafruit LIS3DH
- **NFC/RFID Chip**: ST25DV64KC (Adafruit breakout)
- **PDM Microphone**
- **Magnetic Hall Effect Sensor**
- **NeoPixel LEDs**: 42 x 1mm
- **Discrete LEDs**: Additional LEDs (please specify names and types)
- **External Flash Memory**

---

## Files Included

- **Gerber Files** 🐹: PCB design files for manufacturing.
- **Artwork** 🎨: Original design assets.
- **Schematic** 📂: Circuit diagrams and component layouts.

---

## Demo Video

Watch the Skull of Fate in action:

[![Skull of Fate Demo](https://img.youtube.com/vi/K-RiMK6Kz78/0.jpg)](https://www.youtube.com/watch?v=K-RiMK6Kz78)

---

## SAO as an I2C Client

The SAO can act as an I2C client, allowing it to interface with a host device like a Raspberry Pi Pico W running MicroPython.

- **Arduino Code for SAO I2C Client**: The SAO is programmed using Arduino and can respond to I2C commands from a host. This allows for interactive control of the SAO's features such as LEDs, sensors, and effects.

- **MicroPython Code for I2C Host**: The I2C host is based on MicroPython for the Raspberry Pi Pico W. It communicates with the SAO over I2C, sending commands and receiving responses, enabling control over the SAO's functionality.

---

## SAO and Badge Interaction

The SAO, written in Arduino, has **USER1** and **USER2** ports to interact with the Raspberry Pi Pico Wireless (acting as the host badge).

- **USER1**:
  - Connected to a NeoPixel on the SAO.
- **USER2**:
  - Connected to the **GPO1** pin of the ST25DV RFID chip and a button on the SAO.
  - Pulled high with a 10kΩ resistor.
  - When the RFID detects a field (read or write) or when the user presses the button, the signal goes low, allowing the badge to trigger an event.

---

## Additional Background

The SAO is based on several Adafruit components:

- **Feather M0 Express**: Provides the main microcontroller functionality.
- **ST25DV64KC NFC/RFID Chip**: Allows for NFC interactions, reading and writing data.
- **LIS3DH Accelerometer**: Enables motion detection and orientation sensing.
- **PDM Microphone**: For sound input and reactive animations.
- **Magnetic Hall Effect Sensor**: Detects magnetic fields for interactive features.
- **NeoPixel LEDs**: 42 x 1mm LEDs for eye animations and effects.
- **Discrete LEDs**: Additional LEDs (please specify types and names) for various indicators.
- **External Flash Memory**: For storing data and configurations.

**Programming and Development:**

- The SAO is programmed using **Arduino** but can also support **MicroPython** if desired.
- The badge demo code interacting with the SAO is written in **MicroPython**.

---

## License

This project is open-source and available under the [MIT License](LICENSE). Feel free to use, modify, and distribute the code and designs.

---

**Note:** For more detailed information, including schematics, artwork, and code examples, please refer to the respective files included in this repository.

If you have any questions or need assistance, please open an issue or contact the project maintainers.

---

*This documentation was generated to provide an overview of the Skull of Fate project, its features, components, and how to interact with it using both Arduino and MicroPython code.*

---
