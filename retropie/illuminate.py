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
import xml.etree.ElementTree as ET
from configparser import ConfigParser
import logging


# Constants
_SETTINGS_SECTION="settings"
_TEST_SECTION="TEST"
_USB_SECTION="USB"
_SYSTEMS_FILE="systems.ini"
_CONFIG_FILE="config.ini"
_REMAP_Pn_PREFIX_FORMAT="P%d_"
_FLASH_INTERVAL = 100

# Class variables
_basePath=None
_config=None


'''
Depending on the extension of the mapfile, load the 
correct button list for the desired system & rom
'''
def getButtonList(mapfile, system, rom):
	if mapfile.endswith(".xml"):
		return getXMLButtonList(mapfile, system, rom)
	elif mapfile.endswith(".ini"):
		return getINIButtonList(mapfile, system, rom)
	else:
		logging.warning("No mapfile provided/found for system:[%s], no buttons mapped" % (system))
		return None


'''
Read the emulators XML file and return a list of active buttons for the 
provided emulator&rom. return None if nothing was found.  
[P1_BUTTON1, P1_BUTTON2, P2_BUTTON1, P2_BUTTON2]
'''
def getXMLButtonList(mapfile, emulator, rom):
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
		
	return buttonList
	#return {"numPlayers":len(players), "alternating":alternating == 1, "buttons": buttons}


'''
Read the button list from a .ini file for the given system&rom.
'''
def getINIButtonList(mapfile, system, rom):
	
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
		
		for playerIndex in range(0, numPlayers if simultaneous else 1):
			remapPlayer1 = False
			csvName = "player%d" % (playerIndex+1)
			if cfg.has_option(rom, csvName) == False:
				logging.warning("Player:[%s] option not found in rom section:[%s] defaulting to player1 mapping" %(csvName, rom))
				csvName = "player1"
			buttonCsv = cfg.get(rom, csvName)
			buttonList += buttonCsv.split(",")
			logging.info("Player[%d] Button List [%s]" % (playerIndex, str(buttonCsv)))
		
	except Exception as e:
		buttonList = None
		logging.exception("Error Loading Button Data for [%s][%s] not found in [%s]" % (system, rom, systemConfigFile))
		
	return buttonList
	
	

'''
generate a button ID list for the given emulator and button name list.
The config.ini file is referenced and the IDs are mapped first 
by looking for the emulator named section, then cross-referencing
those names with those in the [USB] section.  Using those ID
numbers, generate a list of IDs to send to the USB.
'''
def getButtonIdList(emulatorName, buttonNameList):
	logging.info("Generating LED button list for...\n  emulator:[%s]\n  buttons:%s" % (emulatorName, buttonNameList))
	
	btnList = []
	idList = []
	
	for buttonName in buttonNameList:
		id = getButtonId(emulatorName, buttonName)
		if id >= 0:
			idList.append(id)
	
	logging.info("Generated ButtonIdList...\n  ButtonIdList:%s\n  buttons:%s" % (idList, btnList))
		
	return idList
		
		

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
	command += "\r"
	ser = serial.Serial(tty, 9600, timeout=0.5)
	ser.write(command.encode())
	ser.flush()
	ser.close()



'''
Look up the led butotn in the config for the given emulator and button name.
the emulator name is the section title in the config.ini, the button name
is the key of an entry in that section.  After finding they value for the
desired key, a 2nd lookup is done in the "USB" section for the actual
LED output index.  If the button was not found, a -1 is returned
'''
def getButtonId(emulatorName, buttonName):
	global _basePath
	global _config
	try:
		logging.debug("Getting button ID for emulator:[%s] button:[%s]" % (emulatorName, buttonName))
		btnCode = _config.get(emulatorName, buttonName)
		logging.debug("Found Button Code:[%s] in emulator section:[%s]" % (btnCode, emulatorName))
		btnId = _config.get(_USB_SECTION, btnCode)
		logging.debug("Found Button Id:[%s] in section:[%s] for button:[%s]" % (btnId, _USB_SECTION, btnCode))
	except:
		logging.error("Button not found in [%s] for emulator:[%s] button:[%s] . Skipping" % (_CONFIG_FILE, emulatorName, buttonName))
		return -1
	return int(btnId)


'''
Load the config.ini into the module variables and do some basic checking.
Also, the special "TEST" emulator section is dynamically created.
'''
def loadConfig():
	global _basePath
	global _config
	systemsFilename = "%s/%s" % (_basePath, _CONFIG_FILE)
	configFilename = "%s/%s" % (_basePath, _SYSTEMS_FILE)
	logging.debug("Loading Configs [%s] & [%s]" % (configFilename, systemsFilename))
	_config = ConfigParser()
	_config.read(systemsFilename)
	_config.read(configFilename)
	
	# Add our special TEST section
	_config.add_section(_TEST_SECTION)
	for btn in _config.items(_USB_SECTION):
		_config.set(_TEST_SECTION, btn[0], btn[0])
	

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
	parser.add_argument("tty", help="The USB Serial Port the module is connected to. eg: /dev/ttyACM0")
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
		
	# Special Case emulator "TEST" bypasses the xml lookup, and directly sets the button by name 
	if systemName == _TEST_SECTION:
		idList = getButtonIdList(systemName, [romName])
		sendCommand(args.tty, "flash 2 %d %s" % (_FLASH_INTERVAL, idList), args.printonly)
		return
	
	
	# Check that a config.ini section exists for the desired emulator.  If it doesn't, default to ALL buttons.
	blist = None
	if _config.has_section(systemName) == False:
		logging.warning("%s file missing section for system:[%s], defaulting to ALL buttons" %(_CONFIG_FILE, systemName))
		systemName = _TEST_SECTION
	else:
		# Obtain the list of buttons from the system.xml file
		mapfile = _config.get(systemName, "mapfile")
		blist = getButtonList(mapfile, systemName, romName)
	
	
	# If no buttons were found, lets' generate one from the config.ini.  Simply turn on all known buttons.
	enableFallbackFlashIndicator = False
	if blist == None:
		enableFallbackFlashIndicator = True
		logging.warning("No Button list found for emulator:[%s] rom:[%s], generating a default list." % (systemName, romName))
		blist = []
		
		# If no mapping section exists for an emulator, fall back to ALL buttons
		if _config.has_section(systemName) == False:
			systemName = _TEST_SECTION
			
		for btn in _config.items(systemName):
			blist.append(btn[0])
		
	# Generate a button ID list for the desired buttons 
	idList = getButtonIdList(systemName, blist)
	idList = map(str, idList)
	idList = ' '.join(idList)
	
	# Flash the buttons to tell the user a default was used
	if enableFallbackFlashIndicator and args.noflash == False:
		sendCommand(args.tty, "flash 2 %d %s" % (_FLASH_INTERVAL*3, idList), args.printonly)
		sendCommand(args.tty, "on %s" % (idList), args.printonly)
	elif args.noflash == False:
		# Quick-Flash the buttons to indicate an accurate mapping
		sendCommand(args.tty, "flash 2 %d %s" % (_FLASH_INTERVAL, idList), args.printonly)
		sendCommand(args.tty, "on %s" % (idList), args.printonly)
	

if __name__ == "__main__":
	main()
	




