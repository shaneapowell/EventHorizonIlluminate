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
import xml.etree.ElementTree as ET
from configparser import ConfigParser
import logging


# Constants
_SETTINGS_SECTION="settings"
_CONFIG_FILE="config.ini"
_REMAP_Pn_PREFIX_FORMAT="P%d_"
_FLASH_INTERVAL = 100
_KEY_SYSTEM="system"
_KEY_ROM="rom"
_KEY_BUTTONS="buttons"
_KEY_NUMPLAYERS="numPlayers"
_KEY_SIMULTANEOUS="simultaneous"
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
    numPlayers: 2
    simultaneous: True
}
'''
def createMapFileGameConfig(system, rom):
    # First, try to load the .ini file, if that fails,
    # try the .xml file.  
    gameCfg = getXMLGameConfig(system, rom)
    if gameCfg == None:
        gameCfg = getINIGameConfig(system, rom)
    
    if gameCfg == None:
        logging.warning("No xml or ini mapfile found for system:[%s], no buttons mapped" % (system))
        return None

    gameCfg[_KEY_SYSTEM] = system
    gameCfg[_KEY_ROM] = rom
    return gameCfg


'''
Read the emulators XML file and return a list of active buttons for the 
provided emulator&rom. return None if nothing was found.  
[P1_BUTTON1, P1_BUTTON2, P2_BUTTON1, P2_BUTTON2]
'''
def getXMLGameConfig(emulator, rom):

    mapfile = emulator + ".xml"
    buttonList = []
    
    try:
        systemConfigFile = "%s/%s" % (_basePath, mapfile)
        logging.info("Reading control info from config file:[%s] for rom:[%s]" % (systemConfigFile, rom))
        
        controls = ET.parse(systemConfigFile)
        root = controls.getroot()
        game = root.findall(".//Game/[@RomName='%s']" % (rom))
        
        if len(game) == 0:
            raise Exception("Rom [%s] Not Found in config" % rom)
        
        game = game[0]
        numPlayers = int(game.get("NumPlayers"))
        simultaneous = 0 == int(game.get("Alternating"))
        
        players = game.findall("./Player")
        #labels = game.findall(".//Label")
        
        logging.debug("Game: %s" % game.get("RomName"))
        logging.debug("Players: %d" % numPlayers)
        logging.debug("Simultaneous: %s" % str(simultaneous)) 
        #logging.debug("Labels: " + str(len(labels)))

        for playerIndex in range(0, numPlayers if simultaneous else 1):
            remapPlayer1 = False
            
            if playerIndex >= len(players):
                player = players[0]
                remapPlayer1 = True & simultaneous
            else:
                player = players[playerIndex]
            
            labels = player.findall(".//Label")
            
            logging.debug("Mapping [%d] player[%d] buttons" % (len(labels), playerIndex+1))
            
            if remapPlayer1:
                p1Prefix = _REMAP_Pn_PREFIX_FORMAT % (1)
                remapPrefix = _REMAP_Pn_PREFIX_FORMAT % (playerIndex+1)
                logging.debug("Player[%d] not found in xml config, creating [%s] from [%s] buttons" % ((playerIndex+1), remapPrefix, p1Prefix))

            
            for button in labels:
                name = button.get("Name")
                if remapPlayer1:
                    name = name.replace(p1Prefix, remapPrefix, 1)
                buttonList.append(name)
        
            logging.debug("Found Buttons %s" % buttonList)
        
    except Exception as e:
        buttonList = None
        logging.exception("Error Loading Button Data for [%s][%s] not found in [%s]" % (emulator, rom, systemConfigFile))
        
    return {
        "buttons": buttonList,
        "numPlayers": numPlayers,
        "simultaneous": simultaneous
    }
    #return {"numPlayers":len(players), "alternating":alternating == 1, "buttons": buttons}


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
            
        numPlayers = cfg.getint(rom, "numplayers")
        simultaneous = False == cfg.getboolean(rom, "alternating")
        
        logging.info("Players:[%d] Simultaneous:[%s]" % (numPlayers, simultaneous))
        
        if cfg.has_option(rom, "buttons") == False:
            logging.warning("buttons CSV option not found in rom section:[%s]" %(csvName, rom))
        buttonCsv = cfg.get(rom, "buttons")
        buttonList += buttonCsv.split(",")
        logging.info("Button List [%s]" % (str(buttonCsv)))
        
    except Exception as e:
        buttonList = None
        logging.exception("Error Loading Button Data for [%s][%s] not found in [%s]" % (system, rom, systemConfigFile))
        
    return {
        "buttons": buttonList,
        "numPlayers": numPlayers,
        "simultaneous": simultaneous
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
    
    logging.info("Sending Command:[%s]" % (command))
    
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
        logging.debug("Getting button ID for emulator:[%s] button:[%s]" % (emulatorName, buttonName))
        btnCode = _config.get(emulatorName, buttonName)
        logging.debug("Found Button Code:[%s] in emulator section:[%s]" % (btnCode, emulatorName))
        btnId = btnCode[1:]
    except:
        logging.warning("Button not found in [%s] for emulator:[%s] button:[%s] . Skipping" % (_CONFIG_FILE, emulatorName, buttonName))
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
    ports = serial.tools.list_ports.comports()
    ports = filter(lambda x: x.vid != None and x.vid == 0x1209 and x.pid == 0xbdb1, ports)
    ports = sorted(ports, key=lambda x: (x.location))
    return ports


'''
'''
def main():
    parser = argparse.ArgumentParser(description="Game Console LED Controller. To manually test lights, use 'TEST' for <system> and the button name from the 'USB' section in the config.\n\n example:  illuminate.py -d TEST B3")
    parser.add_argument("-c", "--config", dest="configdir", default=os.path.dirname(os.path.realpath(__file__)), help="Path to location of all xml conf files. Default is the directory this script is located.")
    parser.add_argument("-p", "--printonly", action="store_true", default=False, help="Print result only, don't send LED lightup commands through i2c bus")
    parser.add_argument("-d", "--debug", action="store_true", default=False, help="Enable Debugging logging.")
    parser.add_argument("-f", "--noflash", action="store_true", default=False, help="Force disable flashing of lights when button LEDs are enabled")
    parser.add_argument("system", help="The name of the emulator system being run. eg.. 'mame-mame4all', 'psx', 'snes'...etc.  Use 'TEST' to test specific buttons.")
    parser.add_argument("rom", help="The rom being run. When using the 'TEST' emulator option, pass in B1 to B16 to manually turn on a LED")
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
    
    # TODO: To compensate for the slow(ish) load times of Mame, lets' fire up a background thread that animates the buttons
        
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
        gameCfg = {
            _KEY_SYSTEM: _VAL_DEFAULT,
            _KEY_ROM: _VAL_DEFAULT,
            _KEY_BUTTONS: [],
            _KEY_NUMPLAYERS: 4,
            _KEY_SIMULTANEOUS: True
        }


    # Generate a button ID list for the desired buttons 
    addControllerButtonList(gameCfg)
#    controllerButtons = ' '.join(map(str, controllerButtons))
#    gameCfg[_KEY_CONTROLLERBUTTONS] = controllerButtons
    logging.debug(str(gameCfg))

    # Get our controller tty serial devices by our VID:PID 
    controlerTTYDevices = findSerialDevices(0x1209, 0xbdb1)

    # Now, for each found controller. Turn on the active ones, and off the inactive ones    
    for ttyDevice in controlerTTYDevices:
        logging.info("Found " + hex(ttyDevice.vid) + ":" + hex(ttyDevice.pid) + " " + ttyDevice.manufacturer + " - " + ttyDevice.product + " @ " + ttyDevice.location + " -> " + ttyDevice.device)
        controllerButtons = gameCfg[_KEY_CONTROLLERBUTTONS]

        # Flash the buttons to tell the user a default was used
        if enableFallbackFlashIndicator and args.noflash == False:
            sendCommand(ttyDevice.device, "off all; flash 2 %d %s; on %s;" % (_FLASH_INTERVAL*3, controllerButtons, controllerButtons), args.printonly)
        elif args.noflash == False:
            # Quick-Flash the buttons to indicate an accurate mapping
            sendCommand(ttyDevice.device, "off all; flash 2 %d %s; on %s;" % (_FLASH_INTERVAL, controllerButtons, controllerButtons), args.printonly)
    

if __name__ == "__main__":
    main()
    




