import board, digitalio
import supervisor, sys, time
import adafruit_dotstar as dotstar


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
strobePin = digitalio.DigitalInOut(board.D1)
strobePin.direction = digitalio.Direction.OUTPUT
clockPin = digitalio.DigitalInOut(board.D0)
clockPin.direction = digitalio.Direction.OUTPUT
dataPin = digitalio.DigitalInOut(board.D2)
dataPin.direction = digitalio.Direction.OUTPUT

def sendInt(value, debug = False):
    """Send an int over as a 24 bit shift register sequence. The order of the bits is button 0 is bit 0, button 23 is bit 23."""
    if (debug): 
        print("Sending {:b}".format(value))
    for index in range(23, 0, -1):
        dataPin.value = True if (value >> index & 1) else False
        clockPin.value = True
        clockPin.value = False

    strobePin.value = True
    strobePin.value = False


def buttonsOn(buttonIds, debug = False):
    """Turn on/off the buttons identified by butonIds 0-23"""
    print("buttons on %s" % str(buttonIds))
    led.value = True
    bits = 0
    for id in buttonIds:
        bits = bits | (1 << id-1)
    sendInt(bits)

def buttonsOff(debug = False):
    """Short Hand to turn the button all off"""
    print("buttons all off")
    led.value = False
    sendInt(0)

def buttonsFlash(buttonIds, debug = False):
    """Flash the buttons identified by butonIds 0-23"""
    if len(buttonIds) <= 2:
        print("Error: Not Enough Arguments")
    count = args.pop(0)
    intervalMs = args.pop(0)
    print("flash buttons %s %dx for %d ms" % (str(buttonIds), count, intervalMs))
    for index in range(count):
        buttonsOn(buttonIds)
        time.sleep(intervalMs / 1000)
        buttonsOff()
        time.sleep(intervalMs / 1000)


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
            print("Waiting for Command...")
    except Exception as err:
        print("Error: {0}".format(err))
        dot[0] = RED
        time.sleep(0.5)
