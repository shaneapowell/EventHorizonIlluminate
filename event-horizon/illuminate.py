#!/usr/bin/python3

'''
MIT License

Copyright (c) 2020 Shane Powell

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
'''
import argparse
import serial, os, sys, getopt, time
import serial.tools.list_ports
from configparser import ConfigParser
import logging


# Constants
_SETTINGS_SECTION="settings"
_CONFIG_FILE="config.ini"
_FLASH_INTERVAL = 70
_KEY_CONTROLLER="controller"
_KEY_SYSTEM="system"
_KEY_ROM="rom"
_KEY_BUTTONS="buttons"
_KEY_NUMPLAYERS="players"
_KEY_ALTERNATING="alternating"
_KEY_CONTROLLERBUTTONS="controllerButtons"
_KEY_PLAYER="player"
_KEY_COIN="coin"
_VAL_DEFAULT="default"

# Class variables
_basePath=None
_config=None

'''
Depending on the extension of the mapfile, load the 
correct Game Info into the returnd dict.
{
    buttons: [X,Y,A,B],
    players: 2
    alternating: 1
}
'''
def createMapFileGameConfig(system, rom):
    # First, try to load the .ini file, if that fails,
    # try the .xml file.  
    gameCfg = getINIGameConfig(system, rom)
    
    if gameCfg == None:
        logging.warning("No .ini mapfile found for system:[%s], no buttons mapped" % (system))
        return None

    gameCfg[_KEY_SYSTEM] = system
    gameCfg[_KEY_ROM] = rom
    return gameCfg



'''
Read the button list from a .ini file for the given system&rom.
Returns the list of named system buttons.  eg [X,Y,A,B]
'''
def getINIGameConfig(system, rom):
    
    mapfile = system + ".ini"
    buttonList = []
    
    try:
        systemConfigFile = "%s/%s" % (_basePath, mapfile)
        cfg = ConfigParser()
        cfg.read(systemConfigFile)
        
        if cfg.has_section(rom) == False:
            logging.warning("ROM:[%s] section not found in [%s]" %(rom, systemConfigFile))
            return None
            
        numPlayers = cfg.getint(rom, _KEY_NUMPLAYERS)
        alternating = False
        try:
            alternating = cfg.getint(rom, _KEY_ALTERNATING) == 1
        except ValueError as e:
            try:
                alternating = cfg.get(rom, _KEY_ALTERNATING).lower() == "true"
            except:
                logging.error("Unable to read alternating=[0,1] from value[%s]" % cfg.get(rom, _KEY_ALTERNATING))
        
        logging.info("Players:[%d] Alternating:[%s]" % (numPlayers, str(alternating)))
        
        if cfg.has_option(rom, "buttons") == False:
            logging.warning("buttons CSV option not found in rom section:[%s]" %(csvName, rom))
        buttonCsv = cfg.get(rom, "buttons")
        buttonList += buttonCsv.split(",")
        logging.info("Button List [%s]" % (str(buttonCsv)))
        
    except Exception as e:
        logging.exception("Error Loading Button Data for [%s][%s] not found in [%s]" % (system, rom, systemConfigFile))
        return None
        
    return {
        _KEY_BUTTONS: buttonList,
        _KEY_NUMPLAYERS: numPlayers,
        _KEY_ALTERNATING: alternating
    }
    
    

'''
generate a button ID list for the given emulator button name list, and add to the controllerButton list..
'''
def addControllerButtonList(gameCfg):
    logging.info("Generating LED button list for...\n  system:[%s]\n  buttons:%s" % (gameCfg[_KEY_SYSTEM], gameCfg[_KEY_BUTTONS]))
    cbList = []
    
    # Iterate of a copy of the list, so we can remove not-found buttons
    for buttonName in list(gameCfg[_KEY_BUTTONS]):
        id = getButtonId(gameCfg[_KEY_SYSTEM], buttonName)
        if id >= 0:
            cbList.append(id)
        else:
            gameCfg[_KEY_BUTTONS].remove(buttonName)

    # The arcade.xml file provides button names for both p1 and p2.  Which is pretty much always a duplicate (see config.ini)
    # Therefore, we'll remove any duplicate button ids.  This shouldn't be a problem with the <system>.ini files.
    cbList = list(dict.fromkeys(cbList))
    cbList = ' '.join(map(str, cbList))
    gameCfg[_KEY_CONTROLLERBUTTONS] = cbList
    
    logging.info("Generated Controller Button Id List:%s" % (cbList))

    gameCfg[_KEY_CONTROLLERBUTTONS] = cbList
    return gameCfg
        
        

'''
Send the provided command to the USB.  the boolean printOnly will perform
shifting and calculations to generate the output bytes, but will refrain from actually connecting 
to, and sending the bytes to the i2c chip.  This is usefull mostly just for debugging locally.
'''
def sendCommand(tty, command, printOnly):
    
    logging.info("Sending Command:[%s] to [%s]" % (command, tty))
    
    if printOnly == True:
        return
        
    # Send to the usb serial port
    command += "\n"
    ser = serial.Serial(tty, 9600, timeout=0.5)
    ser.write(command.encode())
    ser.flush()
    ser.close()



'''
Look up the led button in the config for the given emulator and button name.
the emulator name is the section title in the config.ini, the button name
is the key of an entry in that section. The value is the Bxx button output
on the control board. The B is stripped off, leaving the 1-12 board output 
numbers.  If the button was not found, a -1 is returned
'''
def getButtonId(emulatorName, buttonName):
    global _basePath
    global _config
    try:
        logging.debug("Getting button Code for emulator:[%s] button:[%s]" % (emulatorName, buttonName))
        btnCode = _config.get(emulatorName, buttonName)
        logging.debug("Found Button Code:[%s] in emulator section:[%s]" % (btnCode, emulatorName))
        btnId = _config.get(_KEY_CONTROLLER, btnCode)
        logging.info("Found Controller Output Id [%s] for Button Code: [%s]" % (btnId, btnCode))
    except Exception as e:
        logging.warning("Button not found in [%s] for emulator:[%s] button:[%s] . Skipping" % (_CONFIG_FILE, emulatorName, buttonName))
        logging.debug(str(e))
        return -1
    return int(btnId)


'''
Load the config.ini into the module variables and do some basic checking.
Also, the special "TEST" emulator section is dynamically created.
'''
def loadConfig():
    global _basePath
    global _config
    configFilename = "%s/%s" % (_basePath, _CONFIG_FILE)
    logging.debug("Loading Config [%s]" % (configFilename))
    _config = ConfigParser()
    _config.read(configFilename)
    

'''Find and sort our serial tty devices by our known vid:pid'''
def findSerialDevices(vid, pid):
    reverse = False
    if _config.get(_KEY_CONTROLLER, "sort_controllers").lower() == "reverse":
        reverse = True
    ports = serial.tools.list_ports.comports()
    ports = filter(lambda x: x.vid != None and x.vid == 0x1209 and x.pid == 0xbdb1, ports)
    ports = sorted(ports, key=lambda x: (x.location), reverse=reverse)
    return ports


'''
'''
def main():
    parser = argparse.ArgumentParser(description="Game Console LED Controller. To manually test lights, use 'TEST' for <system> and the button name from the 'USB' section in the config.\n\n example:  illuminate.py -d TEST B3")
    parser.add_argument("-c", "--config", dest="configdir", default=os.path.dirname(os.path.realpath(__file__)), help="Path to location of all .ini conf files. Default is the directory this script is located.")
    parser.add_argument("-p", "--printonly", action="store_true", default=False, help="Print result only, don't send LED lightup commands through i2c bus")
    parser.add_argument("-d", "--debug", action="store_true", default=False, help="Enable Debugging logging.")
    parser.add_argument("-f", "--noflash", action="store_true", default=False, help="Force disable flashing of lights when button LEDs are enabled")
    parser.add_argument("system", help="The name of the emulator system being run. eg.. 'arcade', 'psx', 'snes'...etc. Use 'identify' for help finding your controllers")
    parser.add_argument("rom", help="The rom being run. When using the 'identify' system option, pass in 'controllers' to find your controllers, or B1 to B16 to manually turn on a LED")
    args = parser.parse_args()
        
    global _basePath
    global _config
    
    systemName = args.system
    romName = args.rom
    
    # Find the path to this script file, to find all necessary data files.
    _basePath = args.configdir

    # Alter the desired verbosity of the output logging
    if args.debug == True:
        logging.basicConfig(level=logging.DEBUG)
    else:
        logging.basicConfig(level=logging.INFO)
    
    logging.info("System:[%s] ROM:[%s]" % (systemName, romName))
    
    # Load the confg file into memory
    loadConfig()
    
    # Get our controller tty serial devices by our VID:PID 
    controlerTTYDevices = findSerialDevices(0x1209, 0xbdb1)

    # The special "identify" system.
    if systemName == "identify":
        if romName == "controllers":
            deviceIndex = 0
            for ttyDevice in controlerTTYDevices:
                deviceIndex += 1
                sendCommand(ttyDevice.device, "flash %d 500 all" % (deviceIndex),  args.printonly)
                time.sleep((1+ deviceIndex) * (500 / 1000))
        elif romName.startswith("C"):
            deviceIndex = int(romName[1:])
            ttyDevice = controlerTTYDevices[deviceIndex-1]
            sendCommand(ttyDevice.device, "flash %d 500 all" % (deviceIndex),  args.printonly)
        elif romName.startswith("B"):
            buttonIndex = int(romName[1:])
            for ttyDevice in controlerTTYDevices:
                sendCommand(ttyDevice.device, "flash 1 500 %d" % (buttonIndex),  args.printonly)

        return

        
    # Check that a config.ini section exists for the desired emulator.  If it doesn't, default to ALL buttons.
    if _config.has_section(systemName) == False:
        logging.warning("%s file missing section for system:[%s], defaulting to ALL buttons" %(_CONFIG_FILE, systemName))
    else:
        # Obtain the list of game buttons from the <system>.[xml|ini] file
        gameCfg = createMapFileGameConfig(systemName, romName)
    
    
    # If no buttons were found, lets' generate one from the config.ini.  Simply turn on all known buttons for the given emulator.
    enableFallbackFlashIndicator = False
    if gameCfg == None:
        enableFallbackFlashIndicator = True
        logging.warning("No Button list found for system:[%s] rom:[%s], generating a default list." % (systemName, romName))
        blist = []
        for btn in _config.items(systemName):
            blist.append(btn[0].upper())
        gameCfg = {
            _KEY_SYSTEM: systemName,
            _KEY_ROM: _VAL_DEFAULT,
            _KEY_BUTTONS: blist,
            _KEY_NUMPLAYERS: 4,
            _KEY_ALTERNATING: False
        }


    # Generate a button ID list for the desired buttons 
    addControllerButtonList(gameCfg)
    logging.debug(str(gameCfg))

       
    # Now, for each found controller. Turn on the active ones, and off the inactive ones    
    deviceIndex = -1
    for ttyDevice in controlerTTYDevices:
        deviceIndex += 1
        logging.info("[%d] Found %s:%s %s - %s @ %s -> %s" % (deviceIndex, hex(ttyDevice.vid), hex(ttyDevice.pid), ttyDevice.manufacturer, ttyDevice.product, ttyDevice.location, ttyDevice.device))
        controllerButtons = gameCfg[_KEY_CONTROLLERBUTTONS]

        if deviceIndex == 0 or (deviceIndex < gameCfg[_KEY_NUMPLAYERS] and not gameCfg[_KEY_ALTERNATING]):
            
            # Normal fash flash 
            cmd = "off all; flash 3 %d %s; on %s;" % (_FLASH_INTERVAL, controllerButtons, controllerButtons)

            # Flash the buttons to tell the user a default was used
            if enableFallbackFlashIndicator and args.noflash == False:
                cmd = "off all; flash 2 %d %s; on %s;" % (_FLASH_INTERVAL*3, controllerButtons, controllerButtons)
            
            sendCommand(ttyDevice.device, cmd, args.printonly)

        else:
            sendCommand(ttyDevice.device, "off all", args.printonly)


if __name__ == "__main__":
    main()
    




