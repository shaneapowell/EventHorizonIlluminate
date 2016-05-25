#!/usr/bin/python3

'''
MIT License

Copyright (c) 2016 Shane Powell

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
import os, sys, getopt, time
import xml.etree.ElementTree as ET
from configparser import ConfigParser
import smbus
import logging

_SETTINGS_SECTION="settings"
_TEST_SECTION="TEST"
_PINS_SECTION="pins"
_CONFIG_FILE="config.ini"
_REMAP_Pn_PREFIX_FORMAT="P%d_"
_I2C_ADDRESS = 0x20
_I2C_IODIR_BANK0 = 0x00
_I2C_IODIR_BANK1 = 0x01
_I2C_LATCH_BANK0 = 0x12
_I2C_LATCH_BANK1 = 0x13
_I2C_ALL_OFF_PINMASK = 0x0000
_I2C_FLASH_INTERVAL = 0.200
_basePath=None
_config=None

'''
'''
def test():
	# Required Parameters
	if len(sys.argv) != 4:
		print("usage: illuminate.py <controls.xml> <nplayers.ini> <romname>")
		print("\texample:\n\t\tilluminate.py /etc/controls.xml /etc/nplayers.ini doubledr")
		exit(0)


	# Find out the number of buttons used, default is all.
	buttonCount = 8
	try:
		controls = ET.parse(sys.argv[1])
		root = controls.getroot()
		path = ".//Game/[@RomName='%s']/Player/[@Number='1']" % sys.argv[3]
		player = root.findall(path)
		buttonCount = player[0].get("NumButtons")
	except:
		print("ROM [%s] not found in [%s]" % (sys.argv[3], sys.argv[1]))

	# Find the number of simultaneous players
	simplayers = 2
	try:
		nplayers = ConfigParser(strict=False)
		nplayers.read(sys.argv[2])
		players = nplayers.get("NPlayers", sys.argv[3])
		if players == "2P alt" or players == "1P":
			simplayers = 1
		elif "sim" in players:
			simplayers = 2
	except:
		print ("ROM [%s] not found in [%s]" % (sys.argv[3], sys.argv[2]))

	print("Button Count: %s " % buttonCount)
	print("Simultaneous Players: %s " % simplayers)

	# Fire up the LEDs
	# - i2cset -y 1 0x20 0x00 0x00  ← bank 0 OUTPUT
	# - i2cset -y 1 0x20 0x01 0x00 ← bank 1 OUTPUT
	# - i2cset -y 1 0x20 0x12 0xff ← Turn on all bank 0 outputs
	# - i2cset -y 1 0x20 0x13 0xff ← Turn on all bank 1 outputs

	address = 0x20
	bus = smbus.SMBus(1)
	bus.write_i2c_block_data(address, 0x00, [0x00])
	bus.write_i2c_block_data(address, 0x01, [0x00])

	# Player 1
	leds = 0xff ^ (0xff << int(buttonCount))
	bus.write_i2c_block_data(address, 0x13, [leds])

	# Player 2
	if simplayers == 1:
		leds = 0x00
	bus.write_i2c_block_data(address, 0x12, [leds])



'''
return a list of active buttons for the provided emulator&rom. return None if nothing was found.  
{ numPlayers: 2,  alternating: False,  buttons: {P1_BUTTON1:True, P1_BUTTON2:True, P2_BUTTON1:True, P2_BUTTON2:True} }
'''
def getButtonList(emulator, rom):
	buttonList = []
	
	try:
		emulatorConfigFile = "%s/%s.xml" % (_basePath, emulator)
		logging.info("Reading control info from config file:[%s] for rom:[%s]" % (emulatorConfigFile, rom))
		
		controls = ET.parse(emulatorConfigFile)
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
			
			logging.debug("Mapping player[%d] buttons" % (playerIndex+1))
			
			if remapPlayer1:
				p1Prefix = _REMAP_Pn_PREFIX_FORMAT % (1)
				remapPrefix = _REMAP_Pn_PREFIX_FORMAT % (playerIndex+1)
				logging.debug("Player[%d] not found in xml config, creating [%s] from [%s] buttons" % ((playerIndex+1), remapPrefix, p1Prefix))

			
			for button in labels:
				name = button.get("Name")
				if remapPlayer1:
					name = name.replace(p1Prefix, remapPrefix, 1)
				buttonList.append(name)
		
	except Exception as e:
		buttonList = None
		logging.exception("Error Loading Button Data for [%s][%s] not found in [%s]" % (emulator, rom, emulatorConfigFile))
		
	return buttonList
	#return {"numPlayers":len(players), "alternating":alternating == 1, "buttons": buttons}




'''
generate a pin mask for the given emulator and button name list
'''
def getPinMask(emulatorName, buttonNameList):
	logging.info("Generating i2c LED pin mask for...\n  emulator:[%s]\n  buttons:%s" % (emulatorName, buttonNameList))
	
	btnList = []
	i2cPinMask = 0x0000
	
	for buttonName in buttonNameList:
		pin = getButtonPin(emulatorName, buttonName)
		if pin >= 0:
			btnList.append(buttonName)
			i2cPinMask = i2cPinMask | 0x01 << pin
	
	logging.info("Generated PinMask...\n  PinMask:[0x%x]\n  buttons:%s" % (i2cPinMask, btnList))
		
	return i2cPinMask
		
		

'''
Send the provided 4 byte pin mask to the MCP27017 i2c chip.  the boolean printOnly will perform
shifting and calculations to generate the output bytes, but will refrain from actually connecting 
to, and sending the bytes to the i2c chip.  This is usefull mostly just for debugging.
'''
def sendPinMask(pinMask, printOnly):
	
	bank0 = pinMask & 0xff
	bank1 = (pinMask >> 9) & 0xff
	logging.info("Sending PinMask:[0x%x] -> bank1:[0x%x] bank0:[0x%x] to I2C address [0x%x]" % (pinMask, bank1, bank0, _I2C_ADDRESS))
	
	if printOnly == True:
		return
		
	# Set all pins to OUTPUT
	bus = smbus.SMBus(1)	
	bus.write_i2c_block_data(_I2C_ADDRESS, _I2C_IODIR_BANK0, [0x00])
	bus.write_i2c_block_data(_I2C_ADDRESS, _I2C_IODIR_BANK1, [0x00])

	# Latch desired pins
	pinBytes = bytes(2)
	bus.write_i2c_block_data(_I2C_ADDRESS, _I2C_LATCH_BANK0, [bank0])
	bus.write_i2c_block_data(_I2C_ADDRESS, _I2C_LATCH_BANK1, [bank1])



'''
Look up the led pin in the config for the given emulator and button name.
the emulator name is the section title in the config.ini, the button name
is the key of an entry in that section.  After finding they value for the
desired key, a 2nd lookup is done in the "pins" section for the actual
LED output pin.  If the pin was not found, a -1 is returned
'''
def getButtonPin(emulatorName, buttonName):
	global _basePath
	global _config
	try:
		logging.debug("Getting pin # for emulator:[%s] button:[%s]" % (emulatorName, buttonName))
		btnCode = _config.get(emulatorName, buttonName)
		logging.debug("Found Button Code:[%s] in emulator section:[%s]" % (btnCode, emulatorName))
		pin = _config.get(_PINS_SECTION, btnCode)
		logging.debug("Found Pin:[%s] in section:[%s] for button:[%s]" % (pin, _PINS_SECTION, btnCode))
	except:
		logging.debug("Pin not found in [%s] for emulator:[%s] button:[%s] . Skipping" % (_CONFIG_FILE, emulatorName, buttonName))
		return -1
	return int(pin)


'''
Load the config.ini into the module variables and do some basic checking.
'''
def loadConfig():
	global _basePath
	global _config
	filename = "%s/%s" % (_basePath, _CONFIG_FILE)
	logging.debug("Loading Config %s" % filename)
	_config = ConfigParser()
	_config.read(filename)
	
	# Add our special TEST section
	_config.add_section(_TEST_SECTION)
	#pins = _config.options(_PINS_SECTION)
	for pin in _config.items(_PINS_SECTION):
		_config.set(_TEST_SECTION, pin[0], pin[0])
	

'''
'''
def main():
	parser = argparse.ArgumentParser(description="Game Console LED Controller. To manually test lights, use 'TEST' for emulator and the button name from the 'pins' section in the config.\n\n example:  illuminate.py -d TEST B3")
	parser.add_argument("-c", "--config", dest="configdir", default=os.path.dirname(os.path.realpath(__file__)), help="Path to location of all xml conf files. Default is the directory this script is located.")
	parser.add_argument("-p", "--printonly", action="store_true", default=False, help="Print result only, don't send LED lightup commands through i2c bus")
	parser.add_argument("-q", "--quiet", action="store_true", default=False, help="Disalbe Debugging logging, show INFO only.")
	parser.add_argument("-f", "--noflash", action="store_true", default=False, help="Force disable flashing of lights when default buttons are enabled")
	parser.add_argument("emulator", help="The name of the emulator being run 'DEBUG' to try manually setting LED")
	parser.add_argument("rom", help="The rom being run. When using the 'DEBUG' emulator option, pass in an int value from 0 to 16 to manually set a LED")
	args = parser.parse_args()
		
	global _basePath
	global _config
	
	_basePath = args.configdir
			
	if args.quiet == False:
		logging.basicConfig(level=logging.DEBUG)
	else:
		logging.basicConfig(level=logging.INFO)
	
	loadConfig()
	
	# Special Case emulator "button" bypasses the xml lookup, and directly sets the button by name 
	if args.emulator == _TEST_SECTION:
		mask = getPinMask(args.emulator, [args.rom])
		sendPinMask(mask, args.printonly)
	else:
		blist = getButtonList(args.emulator, args.rom)
	
	# If no buttons were found, lets' generate one from the config.ini
	enableFlashIndicator = False
	if blist == None:
		enableFlashIndicator = True
		logging.info("No Button list found for emulator:[%s] rom:[%s], generating a default list." % (args.emulator, args.rom))
		blist = []
		for btn in _config.items(args.emulator):
			blist.append(btn[0])
		
	pinMask = getPinMask(args.emulator, blist)
	
	# Flash the buttons to tell the user a default was used
	if enableFlashIndicator & args.noflash == False:
		for index in range(0, _config.getint(_SETTINGS_SECTION, "not_found_flash_count")):
			sendPinMask(pinMask, args.printonly)
			time.sleep(_I2C_FLASH_INTERVAL)
			sendPinMask(_I2C_ALL_OFF_PINMASK, args.printonly)
			time.sleep(_I2C_FLASH_INTERVAL)
	
	sendPinMask(pinMask, args.printonly)
	
	

if __name__ == "__main__":
	main()
	
