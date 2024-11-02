# boot.py for Skull of Fates v6

from machine import I2C, Pin
import time

# I2C addresses for connected devices
PETAL_ADDRESS          = 0x00  # I2C address for the Petal SAO
TOUCHWHEEL_ADDRESS     = 0x54  # I2C address for the Touchwheel SAO
SKULL_OF_FATE_ADDRESS  = 0x13  # I2C address for the Skull of Fate (changed from 0x12 to 0x13)

# Initialize the onboard LED and turn it on to indicate boot process
bootLED = Pin("LED", Pin.OUT)
bootLED.on()

# Initialize buttons with internal pull-up resistors (active LOW)
buttonA = Pin(8, Pin.IN, Pin.PULL_UP)
buttonB = Pin(9, Pin.IN, Pin.PULL_UP)
buttonC = Pin(28, Pin.IN, Pin.PULL_UP)

# GPIOs setup (adjust if necessary to avoid pin conflicts)
# Each SAO position uses two GPIO pins
gpio11 = Pin(7, Pin.OUT)
gpio12 = Pin(6, Pin.OUT)

gpio21 = Pin(5, Pin.OUT)
gpio22 = Pin(4, Pin.OUT)

gpio31 = Pin(3, Pin.OUT)
gpio32 = Pin(2, Pin.OUT)

gpio41 = Pin(22, Pin.OUT)
gpio42 = Pin(21, Pin.OUT)

gpio51 = Pin(20, Pin.OUT)
gpio52 = Pin(19, Pin.OUT)

gpio61 = Pin(18, Pin.OUT)
gpio62 = Pin(17, Pin.OUT)

# Group the GPIO pairs for easier management
GPIOs = [
    [gpio11, gpio12],  # SAO Position 1
    [gpio21, gpio22],  # SAO Position 2
    [gpio31, gpio32],  # SAO Position 3
    [gpio41, gpio42],  # SAO Position 4
    [gpio51, gpio52],  # SAO Position 5
    [gpio61, gpio62],  # SAO Position 6
]

# Initialize I2C peripherals
# I2C0 uses default pins (SDA=GP0, SCL=GP1)
i2c0 = I2C(0, sda=Pin(0), scl=Pin(1), freq=400_000)
# I2C1 uses alternative pins (SDA=GP26, SCL=GP27)
i2c1 = I2C(1, sda=Pin(26), scl=Pin(27), freq=400_000)

def which_bus_has_device_id(i2c_id, debug=False):
    """Returns a list of I2C bus objects that have the requested ID.

    Args:
        i2c_id (int): The I2C address to search for.
        debug (bool): If True, prints the devices found on each bus.

    Returns:
        list: List of I2C bus objects where the device is found.
    """
    i2c0_devices = i2c0.scan()
    i2c1_devices = i2c1.scan()

    if debug:
        print("I2C Bus 0 Devices:", [hex(d) for d in i2c0_devices])
        print("I2C Bus 1 Devices:", [hex(d) for d in i2c1_devices])

    buses = []
    if i2c_id in i2c0_devices:
        buses.append(i2c0)
    if i2c_id in i2c1_devices:
        buses.append(i2c1)

    return buses

def petal_init(bus):
    """Configure the Petal SAO on the specified I2C bus.

    Args:
        bus (machine.I2C): The I2C bus object to communicate with the Petal SAO.
    """
    # Configure the Petal SAO registers for operation
    bus.writeto_mem(PETAL_ADDRESS, 0x09, bytes([0x00]))  # Set raw pixel mode
    bus.writeto_mem(PETAL_ADDRESS, 0x0A, bytes([0x09]))  # Set intensity
    bus.writeto_mem(PETAL_ADDRESS, 0x0B, bytes([0x07]))  # Enable segments
    bus.writeto_mem(PETAL_ADDRESS, 0x0C, bytes([0x81]))  # Undo shutdown
    bus.writeto_mem(PETAL_ADDRESS, 0x0D, bytes([0x00]))  # Reserved
    bus.writeto_mem(PETAL_ADDRESS, 0x0E, bytes([0x00]))  # Default features
    bus.writeto_mem(PETAL_ADDRESS, 0x0F, bytes([0x00]))  # Disable test mode

# Initialize Petal SAO
petal_bus = None
for bus in [i2c0, i2c1]:
    try:
        petal_init(bus)
        petal_bus = bus
        break  # Exit loop after successful initialization
    except Exception as e:
        print(f"Petal initialization failed on bus {bus}: {e}")
        continue

if not petal_bus:
    print("Warning: Petal SAO not found.")

# Initialize Touchwheel SAO
touchwheel_bus = None
for bus in [i2c0, i2c1]:
    if TOUCHWHEEL_ADDRESS in bus.scan():
        touchwheel_bus = bus
        break

if not touchwheel_bus:
    print("Warning: Touchwheel not found.")

def touchwheel_read(bus):
    """Reads the touchwheel position.

    Args:
        bus (machine.I2C): The I2C bus object to communicate with the Touchwheel.

    Returns:
        int: The touch position value (0-255).
    """
    return bus.readfrom_mem(TOUCHWHEEL_ADDRESS, 0, 1)[0]

def touchwheel_rgb(bus, r, g, b):
    """Sets the RGB color on the Touchwheel.

    Args:
        bus (machine.I2C): The I2C bus object to communicate with the Touchwheel.
        r (int): Red component (0-255).
        g (int): Green component (0-255).
        b (int): Blue component (0-255).
    """
    bus.writeto_mem(TOUCHWHEEL_ADDRESS, 15, bytes([r]))  # Set Red LED
    bus.writeto_mem(TOUCHWHEEL_ADDRESS, 16, bytes([g]))  # Set Green LED
    bus.writeto_mem(TOUCHWHEEL_ADDRESS, 17, bytes([b]))  # Set Blue LED

# Indicate initialization status on Petal SAO
if petal_bus:
    # Show yellow light indicating waiting for Touchwheel
    petal_bus.writeto_mem(PETAL_ADDRESS, 3, bytes([0x80]))  # Turn on segment 3
    petal_bus.writeto_mem(PETAL_ADDRESS, 2, bytes([0x80]))  # Turn on segment 2

    # If Touchwheel is configured, show green light
    if touchwheel_bus:
        petal_bus.writeto_mem(PETAL_ADDRESS, 3, bytes([0x00]))  # Turn off segment 3
        petal_bus.writeto_mem(PETAL_ADDRESS, 2, bytes([0x80]))  # Keep segment 2 on
        time.sleep_ms(200)
        petal_bus.writeto_mem(PETAL_ADDRESS, 2, bytes([0x00]))  # Turn off segment 2

# Initialize Skull of Fate I2C Client (Address 0x13)
skull_of_fate_bus = None
for bus in [i2c0, i2c1]:
    if SKULL_OF_FATE_ADDRESS in bus.scan():
        skull_of_fate_bus = bus
        break

if not skull_of_fate_bus:
    print(f"Warning: Skull of Fate at address {hex(SKULL_OF_FATE_ADDRESS)} not found.")
else:
    print(f"Skull of Fate found on bus {skull_of_fate_bus}")
