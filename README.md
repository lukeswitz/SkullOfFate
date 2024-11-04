# Skull of Fate Tarot Card SAO

Alone late at night in your castle, you reach out and grab the cobweb-covered head sitting on your mantle. Was this the head of a fierce enemy defeated in battle, a villager slain in one of your many conquests, a fallen comrade, a spiteful lover? You gaze into the black hollow pits where its eyes once were and start to fade in and out of consciousness, as if in a trance. Visions of the future dizzyingly pass by. Fires rage, bodies are strewn, dragons take flight.

**What future will you choose?**

## Purchase Skull of Fate on Tindie:  
https://www.tindie.com/products/makeithackin/skull-of-fate-sao/

---

## Table of Contents

- [Design and Features](#design-and-features)
- [Components and Materials](#components-and-materials)
- [Files Included](#files-included)
- [Demo Video](#demo-video)
- [Detailed Features](#detailed-features)
  - [Eye Animations and Modes](#eye-animations-and-modes)
  - [Tarot Card Mode](#tarot-card-mode)
  - [Interactive Demos](#interactive-demos)
  - [SAO and Badge Interaction](#sao-and-badge-interaction)
- [SAO as an I2C Client](#sao-as-an-i2c-client)
- [Work Journal](#work-journal)
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
- **Interactive Sensors**: Includes an accelerometer, microphone, and magnetic hall effect sensor for reactive animations.
- **Dynamic RFID Writing**: Generates and writes a unique tarot reading to the RFID chip based on a randomly drawn card.
- **I2C Communication**: Acts as an I2C client, allowing advanced interactions with host devices like the Raspberry Pi Pico W.

**Modes of Operation:**

- **Default Mode**: After turning on, you can cycle through eye animations with the **left button** and optionally change the color of the LEDs with the **right button**.
  - Some LED eye modes are special and change based on the accelerometer or microphone input, making them reactive to position and sound.
- **Tarot Card Mode**: Holding down **both buttons** at the same time puts the device in tarot card mode, where a card is "drawn" at random from the deck and can be read by a phone or other NFC devices.
  - The NFC antenna is located at the upper part of the card.
  - Simply hold the reading device above the card any time after pressing both buttons to receive your reading.

---

## Components and Materials

The **Skull of Fate** SAO is meticulously crafted using a combination of high-quality components to ensure functionality, reliability, and an engaging user experience.

| Quantity | Component Name                | Description                                           |
|----------|-------------------------------|-------------------------------------------------------|
| 1        | **ATSAMD21G18A-MU**           | 32-bit Microcontroller with ARM Cortex M0+ Core        |
| 1        | **W25Q128JVSIQ**               | 128 Mbit Serial Flash Memory                          |
| 44       | **XL-1010RGBC-WS2812B**        | Addressable RGB LED                                   |
| 1        | **BH254VS-6P**                  | SAO Connector                                         |
| 1        | **LIS3DHTR**                    | 3-Axis Accelerometer                                  |
| 1        | **ST25DV16K-IER6T3**            | Dynamic NFC/RFID Tag IC with 16-Kbit EEPROM           |
| 4        | **SKTHACE010**                  | Tactile Switch                                        |
| 1        | **AP2112K-3.3TRG1**             | Power Management IC / Linear Voltage Regulator/LDO    |
| 8        | **VLRK31R1S2-GS08**             | Reverse Mounted LEDs                                   |
| 1        | **HX3144ESO**                   | Hall Effect Sensor                                    |
| 1        | **GMA4030H11-F26**              | MEMS Microphone                                       |
| 1        | **YTC-MC5S-42**                  | Micro USB Port                                        |
| 1        | **Various Passive Components**  | Resistors, capacitors, and other passive elements     |

**Additional Components:**

- **Discrete LEDs**:
  - **Wands**: LEDs located at the corners.
  - **Cups**: Two cup-shaped LEDs.
  - **Swords**: Two sword-shaped LEDs.
- **USB Programming Port**: For firmware updates and programming.

---

## Files Included

- **Gerber Files** 🐹: PCB design files for manufacturing.
- **Artwork** 🎨: Original design assets.
- **Schematic** 📂: Circuit diagrams and component layouts.
- **Code Examples**: Arduino and MicroPython scripts for both the SAO and host badge.
- **Bill of Materials (BOM)**: Detailed list of all components used.
- **Work Journal**: Documentation of the development process across different versions.

---

## Demo Video

Watch the Skull of Fate in action:

[![Skull of Fate Demo](https://img.youtube.com/vi/K-RiMK6Kz78/0.jpg)](https://www.youtube.com/watch?v=K-RiMK6Kz78)

---

## Detailed Features

### Eye Animations and Modes

The Skull of Fate offers multiple eye animations that can be cycled through using the **left button**. Some animations are reactive, changing based on sensor inputs.

1. **Default Eye Mode**: Animated eyes that can change color with the **right button**.
2. **Accelerometer Demo**: An animation where LEDs move in the direction the card is tilted.
   - **Interaction**: Tilt the card, and the LED will fall towards that direction.
   - **Customization**: Change the color of the eyes with the **right button**.
3. **Music Reactive Modes**:
   - **Solid Color Flash**: LEDs flash a solid color in response to sound.
     - **Interaction**: Clap or make noise to see the LEDs flash.
     - **Customization**: Change the flashing color with the **right button**.
   - **Rainbow Effect**: LEDs display a rainbow animation reacting to sound.
     - **Sensor Used**: MEMS Microphone located on the front of the card.
4. **Flame Effect**: A dynamic flame-like animation.
5. **Rainbow Swirl**: A swirling rainbow animation across the eyes.
6. **Bouncing Ball**: An animation simulating a ball bouncing within the eyes.
   - **Customization**: Change the color of the ball with the **right button**.
7. **Rainbow Cycle**: A continuous rainbow animation cycling through colors.
8. **Plasma Effect**: A fluid, plasma-like animation.
9. **Cyberpunk Glitch**: A glitchy animation reminiscent of cyberpunk aesthetics.
   - **Customization**: Change the glitch color with the **right button**.
10. **Cyberpunk Circuit**: An animation mimicking electronic circuits.
    - **Customization**: Change the two primary colors used in the animation.

### Tarot Card Mode

- **Activation**: Press **both buttons** simultaneously.
- **Function**:
  - All LEDs illuminate to indicate the mode is active.
  - A random tarot card link is written to the NFC chip based on a randomly drawn number.
- **Dynamic RFID Writing**:
  - Upon activation, the SAO generates a random number corresponding to one of the 78 tarot cards.
  - This number dynamically writes a unique NDEF (NFC Data Exchange Format) text record to the ST25DV RFID chip.
  - Each tarot card's meaning and interpretation are hosted on the project's GitHub repository.
- **Reading Your Tarot**:
  - Hold a smartphone or NFC reader over the top of the card.
  - No app required; a link will appear on your device.
  - Open the link to view your unique tarot reading.
  - **Example**: Drawing the "Three of Cups" provides a vision of triumph and unity amid chaos and challenge.
- **Tarot Deck**:
  - Contains all 78 cards from the Rider Waite Tarot deck.
  - Each card has a unique interpretation available via the NFC link.
  - All readings are available on the project's GitHub.

### Interactive Demos

- **Accelerometer-Based Interactions**: Animations respond to the orientation of the card.
- **Microphone-Based Reactions**: Animations change in response to sound input.
- **Magnetic Sensor**: Allows for interactions based on magnetic fields (e.g., bringing a magnet close to the sensor could trigger an effect).
- **User Customization**:
  - **Left Button**: Cycle through different animations.
  - **Right Button**: Change colors or settings within the current animation.

### SAO and Badge Interaction

The Skull of Fate SAO can interface with a host badge (e.g., Raspberry Pi Pico W) via the SAO ports.

- **Connections**:
  - **Power**: Supplies power to the SAO.
  - **I2C Communication**: Allows for data exchange between the SAO and the badge.
  - **Input/Output Pins**: Enable additional interactions and controls.
- **USER1 and USER2 Ports**:
  - **USER1**:
    - Connected to a NeoPixel on the SAO.
    - Can receive signals from the badge to control the NeoPixel.
  - **USER2**:
    - Connected to the **GPO1** pin of the ST25DV RFID chip and a button on the SAO.
    - Pulled high with a 10kΩ resistor.
    - When the RFID detects a field (read or write) or when the user presses the button, the signal goes low, allowing the badge to trigger an event.

**Interaction Demos:**

1. **GPIO Interaction**:
   - Pressing the button on the SAO sends a signal to the badge.
   - The badge can respond by lighting up its own LEDs or sending signals back to the SAO.
   - Demonstrates two-way communication between the SAO and badge.

2. **Interrupt Triggered by RFID**:
   - Reading the RFID tag on the SAO sends a signal to the badge.
   - The badge can detect this interrupt and execute actions (e.g., display a message or trigger an animation).

3. **I2C Host and Client Communication**:
   - The SAO acts as an I2C client, while the badge acts as the I2C host.
   - Commands can be sent from the badge to the SAO to control features like LEDs and animations.
   - **Examples**:
     - Turning LEDs on or off.
     - Initiating specific animations like the flame effect.
     - Reading sensor data from the SAO.

---

## SAO as an I2C Client

The SAO is designed to act as an I2C client, allowing for advanced interactions with a host device.

- **Arduino Code**: The SAO runs Arduino code to handle I2C communication and respond to commands.
- **Host Device**: Typically a Raspberry Pi Pico W running MicroPython.
- **Capabilities**:
  - **Receive Commands**: The SAO can execute various commands sent from the host, such as changing animations or reading sensors.
  - **Send Data**: The SAO can send status updates or sensor readings back to the host.
- **Dynamic RFID Writing**:
  - The SAO dynamically writes a tarot reading to the RFID chip based on a randomly drawn number.
  - This number corresponds to a unique card in the deck hosted on GitHub.
  - Ensures each tarot reading is unique and directly linked to the user's experience.
- **Example Interactions**:
  - Host sends a command to start the flame effect on the SAO.
  - Host requests accelerometer data from the SAO.
  - Host turns on or off specific LEDs on the SAO.

---

## Work Journal

### v3 Final_Final SAO

For v3, we updated the artwork and enhanced power handling between USB and supplied badge power. The SAO now intelligently selects the power source with the highest voltage, allowing simultaneous connection to USB and badge power without risk. This ensures the device remains powered from the most stable source, enhancing reliability and user convenience.

**Key Updates:**

- **Artwork Enhancements**: Improved visual design for better aesthetics and functionality.
- **Power Management**: Upgraded power circuitry to seamlessly switch between USB and badge power sources.
- **Maintained Features**: All existing features, pins, and functionalities from v2 remain unchanged, ensuring consistency and reliability.

### v2 Prototype PCB

The v2 prototype, showcased in the YouTube demo video, is fully functional with updated artwork and excellent performance. We encountered and resolved artwork layer issues with the manufacturer, leveraging our growing expertise to refine v3.

**Key Features:**

- **LED Configuration**: Both eyes feature 1mm x 1mm NeoPixels.
- **Pin Reassignment**: Updated pin assignments for better compatibility and performance.
- **USB Integration**: Included a flush-mounted USB port directly on the SAO for easier programming and power.
- **I2C Client Software**: Successfully implemented and tested I2C client functionality, enabling advanced interactions with the host badge.

### v1 PCB for SAO

Our v1 prototype was the first to feature a custom PCB, assembled by JLCPCB with a Feather footprint for flexibility. This initial version provided valuable insights into PCB manufacturing nuances.

**Key Learnings and Features:**

- **PCB Design**: Early artwork revealed that silkscreen overlays do not cover exposed copper areas.
- **I2C Pin Compatibility**: Discovered and corrected incompatible pin connections for the I2C client functionality.
- **Component Testing**: Included a piezo buzzer initially but later removed it to maintain silent operation.
- **LED Selection**: Experimented with two sizes of NeoPixels, ultimately standardizing on 1mm x 1mm LEDs for consistency.

### v0 Prototype on Breadboard

The v0 prototype marked our first foray into building the SAO using multiple Adafruit boards and breadboard prototyping.

**Components Used:**

- **Feather M0 Express**: Served as the primary microcontroller.
- **LIS3DH Accelerometer**: Enabled motion detection.
- **ST25DV Breakout Board**: Facilitated NFC/RFID functionality.
- **Lixie Labs LEDs**: Used for initial eye animations.

**Challenges and Learnings:**

- **Wiring Complexity**: Managed numerous wires, highlighting the need for a custom PCB in later versions.
- **SWD Programming**: Gained experience with SWD programming, overcoming initial challenges to successfully flash the microcontroller.
- **Component Integration**: Learned to integrate various sensors and LEDs, laying the foundation for more advanced prototypes.

---

## License

This project is open-source and available under the [MIT License](LICENSE). Feel free to use, modify, and distribute the code and designs.

---

**Note:** For more detailed information, including schematics, artwork, and code examples, please refer to the respective files included in this repository.

If you have any questions or need assistance, please open an issue or contact the project maintainers.

---

*This documentation was generated to provide an overview of the Skull of Fate project, its features, components, and how to interact with it using both Arduino and MicroPython code.*
