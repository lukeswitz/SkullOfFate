# main.py for Skull of Fates v6

from machine import Pin
import time
import neopixel
import struct
#import random  # Uncomment if random animation index is needed

# Import variables and functions from boot.py
try:
    from boot import (
        i2c0,
        i2c1,
        petal_bus,
        touchwheel_bus,
        skull_of_fate_bus,      # Renamed from client_bus
        PETAL_ADDRESS,
        TOUCHWHEEL_ADDRESS,
        SKULL_OF_FATE_ADDRESS,  # Renamed from CLIENT_ADDRESS
        buttonA,
        buttonB,
        buttonC,
        touchwheel_read,
    )
except ImportError:
    print("Error importing from boot.py")

# SAO POSITION 6
SOF_INPUT_PIN_NUMBER = 17       # GPIO pin number for Skull of Fate input pin
NEOPIXEL_PIN_NUMBER = 18        # GPIO pin number for NeoPixel control

'''
# Alternate SAO positions (uncomment and adjust as needed)
# SAO POSITION 1
SOF_INPUT_PIN_NUMBER = 6        # GPIO pin number for input
NEOPIXEL_PIN_NUMBER = 7         # GPIO pin number for NeoPixel

# SAO POSITION 2
SOF_INPUT_PIN_NUMBER = 4
NEOPIXEL_PIN_NUMBER = 5

# SAO POSITION 3
SOF_INPUT_PIN_NUMBER = 2
NEOPIXEL_PIN_NUMBER = 3

# SAO POSITION 4
SOF_INPUT_PIN_NUMBER = 21
NEOPIXEL_PIN_NUMBER = 22

# SAO POSITION 5
SOF_INPUT_PIN_NUMBER = 19
NEOPIXEL_PIN_NUMBER = 20

# SAO POSITION 6
SOF_INPUT_PIN_NUMBER = 17
NEOPIXEL_PIN_NUMBER = 18
'''

# NeoPixel Configuration
NUM_PIXELS = 1                    # Number of NeoPixels
NEOPIXEL_BRIGHTNESS = 10          # Brightness level (0-255)

# Onboard LED
onboard_led = Pin("LED", Pin.OUT)  # Initialize onboard LED

# Debounce settings
DEBOUNCE_TIME = 0.05  # 50 milliseconds debounce time for button presses

# Helper Functions
def set_neopixel_color(color):
    """Sets the color of the NeoPixel.

    Args:
        color (tuple): (R, G, B) color values.
    """
    if np:
        np[0] = color
        np.write()

def set_onboard_led(state):
    """Sets the state of the onboard LED.

    Args:
        state (bool): True to turn on, False to turn off.
    """
    onboard_led.value(state)

def send_i2c_command(command_char, data_bytes=None, response_length=1):
    """Sends an I2C command to the Skull of Fate device.

    Args:
        command_char (str): The command character to send.
        data_bytes (bytes, optional): Additional data bytes to send. Defaults to None.
        response_length (int, optional): Expected length of response data. Defaults to 1.
    """
    if not skull_of_fate_bus:
        print("Skull of Fate bus not initialized. Cannot send command.")
        return
    print(f"Sending command: {command_char}")
    data_to_send = bytes([ord(command_char)])
    if data_bytes:
        data_to_send += data_bytes
    try:
        # Send the command and data to the Skull of Fate device
        skull_of_fate_bus.writeto(SKULL_OF_FATE_ADDRESS, data_to_send)
        time.sleep(0.1)  # Allow client device to process the command
        if response_length > 0:
            # Read the response from the device
            data_received = skull_of_fate_bus.readfrom(SKULL_OF_FATE_ADDRESS, response_length)
            if data_received:
                decode_response(command_char, data_received)
            else:
                print("No data received from Skull of Fate.")
    except Exception as e:
        print(f"Error sending I2C command: {e}")

def decode_response(command_char, data):
    """Decodes the response received from the Skull of Fate device.

    Args:
        command_char (str): The command character that was sent.
        data (bytes): The data received in response.
    """
    if command_char == '3':
        # Magnetic sensor data (1 byte)
        magnetic_field_detected = bool(data[0])
        print(f"Magnetic Field Detected: {magnetic_field_detected}")
    elif command_char == '4':
        # Accelerometer data (12 bytes)
        if len(data) != 12:
            print("Incorrect data length for accelerometer.")
            return
        x = float_from_bytes(data[0:4])
        y = float_from_bytes(data[4:8])
        z = float_from_bytes(data[8:12])
        print(f"Accelerometer - X: {x:.2f}, Y: {y:.2f}, Z: {z:.2f}")
    elif command_char == '5':
        # Button states (1 byte)
        buttons = data[0]
        left_button = bool(buttons & 0x01)
        right_button = bool(buttons & 0x02)
        print(f"Left Button: {'Pressed' if left_button else 'Released'}, Right Button: {'Pressed' if right_button else 'Released'}")
    elif command_char == '6':
        # Microphone data (4 bytes)
        if len(data) != 4:
            print("Incorrect data length for microphone.")
            return
        volume = float_from_bytes(data)
        print(f"Microphone Volume: {volume:.2f}")
    elif command_char == '1':
        # Animation command acknowledgment
        if len(data) != 1:
            print("Incorrect data length for animation command.")
            return
        # No further processing needed; command acknowledged
        #animation_index = data[0]
        #print(f"Random Animation Index Sent: {animation_index}")
    else:
        # Handle other commands or statuses
        status = data[0]
        print(f"Received status: {chr(status)}")

def float_from_bytes(bytes_data):
    """Converts 4 bytes into a floating-point number.

    Args:
        bytes_data (bytes): The bytes to convert.

    Returns:
        float: The floating-point value.
    """
    return struct.unpack('<f', bytes_data)[0]

# Initialize the input pin and NeoPixel only if skull_of_fate_bus is found
if skull_of_fate_bus:
    # Initialize the input pin from the Skull of Fate
    sof_input_pin = Pin(SOF_INPUT_PIN_NUMBER, Pin.IN)
    
    # Initialize the NeoPixel
    np = neopixel.NeoPixel(Pin(NEOPIXEL_PIN_NUMBER), NUM_PIXELS)
    RED = (NEOPIXEL_BRIGHTNESS, 0, 0)    # Red color
    GREEN = (0, NEOPIXEL_BRIGHTNESS, 0)  # Green color

    # Initialize NeoPixel and Onboard LED to OFF
    set_neopixel_color((0, 0, 0))
    set_onboard_led(False)
else:
    np = None
    sof_input_pin = None
    print("Skull of Fate not found; skipping NeoPixel and input pin setup.")

# Quick spiral test on Petal SAO
if petal_bus:
    for j in range(8):
        which_leds = (1 << (j+1)) - 1
        for i in range(1, 9):
            petal_bus.writeto_mem(PETAL_ADDRESS, i, bytes([which_leds]))
            time.sleep_ms(30)

# =========================
# I2C Scanner Function (Optional)
# =========================

def i2c_scanner():
    """Scans the I2C buses and prints the addresses of connected devices."""
    print("Scanning I2C buses for connected devices...")
    for bus_number, bus in enumerate([i2c0, i2c1]):
        devices = bus.scan()
        if devices:
            print(f"I2C Bus {bus_number} has devices: {[hex(device) for device in devices]}")
        else:
            print(f"I2C Bus {bus_number} has no devices.")

# Uncomment the following line to run the I2C scanner at startup
# i2c_scanner()

if skull_of_fate_bus:
    # If Skull of Fate device is present, print initialization messages
    print()
    print()
    print("Initialization complete.")
    print()
    # Print Command List for user input
    print("Enter command to send to I2C Client:")
    print("Commands:")
    print("0 - Stop all effects and turn off LEDs")
    print("1 - Change current animation index (Format: 1 <ANIMATION_INDEX>)")
    print("2 - Trigger Tarot Card Reading")
    print("3 - Request magnetic sensor data")
    print("4 - Request accelerometer data")
    print("5 - Request button states")
    print("6 - Request PDM microphone data")
    print("7 - Placeholder command 1")
    print("8 - Placeholder command 2")
    print("9 - Placeholder command 3")
    print("A - Placeholder command 4")
    print("B - Placeholder command 5")
    print("C - Control Individual LED (Format: C <LED_ID> <STATE>)")
    print("Type the command and press Enter.")
    print()
    print("BUTTON A,B,C ON THE BADGE CAN SEND COMMANDS ALSO")
    print()
    print()

# =========================
# Main Loop
# =========================

while True:
    # -------------------------
    # Handle Button A (Send '1' + 255 for random animation)
    # -------------------------
    if skull_of_fate_bus and not buttonA.value():  # Button pressed (active LOW)
        # Send command '1' with data 255 to trigger random animation
        #animation_index = random.randint(0, 255)  # Generate random animation index (if needed)
        animation_index = 255  # Use 255 to trigger random animation on client
        send_i2c_command('1', bytes([animation_index]))
        # Debounce delay
        time.sleep(DEBOUNCE_TIME)
        # Wait until button is released
        while not buttonA.value():
            time.sleep(0.01)
        # Indicate button press on Petal SAO if available
        if petal_bus:
            petal_bus.writeto_mem(PETAL_ADDRESS, 2, bytes([0x80]))
    else:
        if petal_bus:
            petal_bus.writeto_mem(PETAL_ADDRESS, 2, bytes([0x00]))
    
    # -------------------------
    # Handle Button B (Send '4') only if skull_of_fate_bus is present
    # -------------------------
    if skull_of_fate_bus and not buttonB.value():  # Button pressed (active LOW)
        # Send command '4' to request accelerometer data
        send_i2c_command('4', response_length=12)
        # Debounce delay
        time.sleep(DEBOUNCE_TIME)
        # Wait until button is released
        while not buttonB.value():
            time.sleep(0.01)
        # Indicate button press on Petal SAO if available
        if petal_bus:
            petal_bus.writeto_mem(PETAL_ADDRESS, 3, bytes([0x80]))
    else:
        if petal_bus:
            petal_bus.writeto_mem(PETAL_ADDRESS, 3, bytes([0x00]))
    
    # -------------------------
    # Handle Button C (Send '2') only if skull_of_fate_bus is present
    # -------------------------
    if skull_of_fate_bus and not buttonC.value():  # Button pressed (active LOW)
        # Send command '2' to trigger Tarot Card Reading
        send_i2c_command('2')
        # Debounce delay
        time.sleep(DEBOUNCE_TIME)
        # Wait until button is released
        while not buttonC.value():
            time.sleep(0.01)
        # Indicate button press on Petal SAO if available
        if petal_bus:
            petal_bus.writeto_mem(PETAL_ADDRESS, 4, bytes([0x80]))
    else:
        if petal_bus:
            petal_bus.writeto_mem(PETAL_ADDRESS, 4, bytes([0x00]))
    
    # -------------------------
    # Handle NeoPixel and SOF Input Pin only if skull_of_fate_bus is available
    # -------------------------
    if skull_of_fate_bus and sof_input_pin:
        input_state = sof_input_pin.value()
        if input_state == 1:
            # Input is HIGH
            set_neopixel_color(RED)             # Set NeoPixel to Red
            set_onboard_led(False)              # Turn OFF onboard LED
        else:
            # Input is LOW
            set_neopixel_color(GREEN)           # Set NeoPixel to Green
            set_onboard_led(True)               # Turn ON onboard LED
            print("SOF Input Pin Triggered!")
    else:
        # If skull_of_fate_bus is not available, skip this section
        pass
    
    # -------------------------
    # Handle Touchwheel
    # -------------------------
    if touchwheel_bus:
        tw = touchwheel_read(touchwheel_bus)
    else:
        tw = 0

    # Display touchwheel position on Petal SAO
    if petal_bus and touchwheel_bus:
        if tw > 0:
            # Map touchwheel position to petal segments
            tw = (128 - tw) % 256
            petal = int(tw / 32) + 1
        else:
            petal = 999  # Invalid petal index
        # Update Petal SAO LEDs based on touchwheel position
        for i in range(1, 9):
            if i == petal:
                petal_bus.writeto_mem(PETAL_ADDRESS, i, bytes([0x7F]))
            else:
                petal_bus.writeto_mem(PETAL_ADDRESS, i, bytes([0x00]))

    # -------------------------
    # Handle Serial Input for Commands
    # -------------------------
    try:
        import sys
        import select
        poller = select.poll()
        poller.register(sys.stdin, select.POLLIN)
        events = poller.poll(0)
        if events:
            user_input = sys.stdin.readline().strip()
            if len(user_input) == 0:
                continue  # No command entered

            parts = user_input.split()
            command = parts[0].upper()
            print("Sending command:", command)

            if command == 'C':
                # Expected format: C <LED_ID> <STATE>
                if len(parts) != 3:
                    print("Error: Invalid format. Use: C <LED_ID> <STATE>")
                    continue

                ledIdStr = parts[1].strip()
                stateStr = parts[2].strip()

                # Convert LED ID to integer
                try:
                    ledId = int(ledIdStr)
                except ValueError:
                    print("Error: Invalid LED ID.")
                    continue

                if ledId < 1 or ledId > 9:
                    print("Error: Invalid LED ID.")
                    continue

                # Validate State
                if stateStr not in ('0', '1'):
                    print("Error: Invalid state. Use 1 for ON and 0 for OFF.")
                    continue

                stateByte = int(stateStr)  # 1 or 0

                # Prepare data payload: LED_ID and State
                payload = bytes([ledId, stateByte])

                # Send command 'C' followed by LED_ID and State
                send_i2c_command('C', payload)

            elif command == '1':
                # Expected format: 1 <ANIMATION_INDEX>
                if len(parts) != 2:
                    print("Error: Invalid format. Use: 1 <ANIMATION_INDEX>")
                    continue

                indexStr = parts[1].strip()

                # Convert Animation Index to integer
                try:
                    index = int(indexStr)
                except ValueError:
                    print("Error: Invalid animation index.")
                    continue

                if index < 0 or index > 255:
                    print("Error: Invalid animation index. Must be between 0 and 255.")
                    continue

                # Prepare data payload: Animation Index
                payload = bytes([index])

                # Send command '1' followed by Animation Index
                send_i2c_command('1', payload)

            elif command in ['3', '5']:
                # Commands expecting 1 byte response
                send_i2c_command(command, response_length=1)

            elif command == '4':
                # Command expecting 12 bytes response (accelerometer data)
                send_i2c_command(command, response_length=12)

            elif command == '6':
                # Command expecting 4 bytes response (microphone data)
                send_i2c_command(command, response_length=4)

            else:
                # Send the command to the I2C CLIENT without additional data
                send_i2c_command(command)
    except ImportError:
        # If select module is not available, fall back to blocking input
        user_input = input().strip()
        if len(user_input) == 0:
            continue  # No command entered

        parts = user_input.split()
        command = parts[0].upper()
        print("Sending command:", command)

        if command == 'C':
            # Expected format: C <LED_ID> <STATE>
            if len(parts) != 3:
                print("Error: Invalid format. Use: C <LED_ID> <STATE>")
                continue

            ledIdStr = parts[1].strip()
            stateStr = parts[2].strip()

            # Convert LED ID to integer
            try:
                ledId = int(ledIdStr)
            except ValueError:
                print("Error: Invalid LED ID.")
                continue

            if ledId < 1 or ledId > 9:
                print("Error: Invalid LED ID.")
                continue

            # Validate State
            if stateStr not in ('0', '1'):
                print("Error: Invalid state. Use 1 for ON and 0 for OFF.")
                continue

            stateByte = int(stateStr)  # 1 or 0

            # Prepare data payload: LED_ID and State
            payload = bytes([ledId, stateByte])

            # Send command 'C' followed by LED_ID and State
            send_i2c_command('C', payload)

        elif command == '1':
            # Expected format: 1 <ANIMATION_INDEX>
            if len(parts) != 2:
                print("Error: Invalid format. Use: 1 <ANIMATION_INDEX>")
                continue

            indexStr = parts[1].strip()

            # Convert Animation Index to integer
            try:
                index = int(indexStr)
            except ValueError:
                print("Error: Invalid animation index.")
                continue

            if index < 0 or index > 255:
                print("Error: Invalid animation index. Must be between 0 and 255.")
                continue

            # Prepare data payload: Animation Index
            payload = bytes([index])

            # Send command '1' followed by Animation Index
            send_i2c_command('1', payload)

        elif command in ['3', '5']:
            # Commands expecting 1 byte response
            send_i2c_command(command, response_length=1)

        elif command == '4':
            # Command expecting 12 bytes response (accelerometer data)
            send_i2c_command(command, response_length=12)

        elif command == '6':
            # Command expecting 4 bytes response (microphone data)
            send_i2c_command(command, response_length=4)

        else:
            # Send the command to the I2C CLIENT
            send_i2c_command(command)

    # Wait for a short period before next iteration
    time.sleep(0.05)  # 50 milliseconds
