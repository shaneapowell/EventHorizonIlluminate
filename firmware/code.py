import board, digitalio
import supervisor, sys, time
import adafruit_dotstar as dotstar

# Button ID to BIT Dict
BUTTON_BIT_LOOKUP = {
    1: 0,
    2: 1,
    3: 2,
    4: 3,
    5: 7,
    6: 6,
    7: 5,
    8: 4,
    9: 8,
    10: 9,
    11: 10,
    12: 11,
    13: 15,
    14: 14,
    15: 14,
    16: 12,
    17: 16,
    18: 17,
    19: 18,
    20: 19,
    21: 20,
    22: 21,
    23: 22,
    24: 23,
}

RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
PURPLE = (180, 0, 255)
BLACK = (0, 0, 0)

# OnBoard red LED 
led = digitalio.DigitalInOut(board.D13)
led.direction = digitalio.Direction.OUTPUT
dot = dotstar.DotStar(board.APA102_SCK, board.APA102_MOSI, 1, brightness=0.2)

# Shift Register Control Pins
strobePin = digitalio.DigitalInOut(board.D0)
strobePin.direction = digitalio.Direction.OUTPUT
clockPin = digitalio.DigitalInOut(board.D3)
clockPin.direction = digitalio.Direction.OUTPUT
dataPin = digitalio.DigitalInOut(board.D1)
dataPin.direction = digitalio.Direction.OUTPUT
strobePin.value = False
clockPin.value = False
dataPin.value = False

def sendInt(value, debug):
    """Send an int over as a 24 bit shift register sequence. The order of the bits is button 0 is bit 0, button 23 is bit 23."""
    if (debug): 
        print(">> Sending {:b}".format(value))

    strobePin.value = False
    clockPin.value = False
    dataPin.value = False

    for index in range(23, -1, -1):
        if (value >> index) & 1:
            dataPin.value = True
        else:
            dataPin.value = False
        clockPin.value = True 
        clockPin.value = False

    strobePin.value = True
    strobePin.value = False


def buttonsOn(buttonIds, debug):
    """Turn on/off the buttons identified by butonIds 0-23"""
    print("> buttons on %s" % str(buttonIds))
    led.value = True
    bits = 0
    for id in buttonIds:
        bit = BUTTON_BIT_LOOKUP[id]
        bits = bits | (1 << bit)
    sendInt(bits, debug)

def buttonsOff(debug):
    """Short Hand to turn the button all off"""
    print("> buttons off")
    led.value = False
    sendInt(0, debug)

def buttonsFlash(buttonIds, debug):
    """Flash the buttons identified by butonIds 0-23"""
    if len(buttonIds) <= 2:
        print("> Error: Not Enough Arguments")
    count = args.pop(0)
    intervalMs = args.pop(0)
    print("> flash buttons %s %dx for %d ms" % (str(buttonIds), count, intervalMs))
    for index in range(count):
        buttonsOn(buttonIds, debug)
        time.sleep(float(intervalMs) / 1000.0)
        buttonsOff(debug)
        time.sleep(float(intervalMs) / 1000.0)

def buttonsSequence(args, debug):
    """Flash quickly on/off each button 1-24 in quick order"""
    print("> sequence")
    idList = []
    for id in range(1,25):
        idList.append(id)
        buttonsOn(idList, debug)
        time.sleep(0.05)
    while idList:
        idList.pop()
        buttonsOn(idList, debug)
        time.sleep(0.05)

time.sleep(0.5)
# Main Loop
print("Waiting for Command...")
while True:
    dot[0] = GREEN
    try:    
        if supervisor.runtime.serial_bytes_available:
            value = input().strip()
            args = value.split()
            if len(args) > 0:
                dot[0] = BLUE
                cmd = args.pop(0)
                debug = cmd.isupper()
                cmd = cmd.lower()
                args = list(map(int, args))
                if cmd == "on":
                    buttonsOn(args, debug)
                elif cmd == "off":
                    buttonsOff(debug)
                elif cmd == "flash": 
                    buttonsFlash(args, debug)
                elif cmd == "seq":
                    buttonsSequence(args, debug)
            print("Waiting for Command...")
    except Exception as err:
        print("Error: {0}".format(err))
        dot[0] = RED
        time.sleep(0.5)
