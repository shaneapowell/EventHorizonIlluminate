

# Event Horizon - Illuminate Joystick Controller 
Last Updated (Mar 13 2021)

Like many of us, I was annoyed by the lack of LED control from the standard zero-delay joystick conrol boards.  All they did was light up every button LED.  I wanted to light up only the buttons that were used for any given game.   Since I couldn't find any inexpensive control boards that could do it, I made my own.

Event Horizon Illuminate Joytick is a RetroPie joystick and button encoder board that also controls the LEDs in the Buttons.  Lighting up ONLY the buttons used by the game you're playing.  A nearly drop-in replacement (size-wize) for the common zero-delay encoder boards used by many retropie builds.  Only works with 5v LED buttons.  Non-Led buttons will almost certainly burn up the control board.  Make sure your buttons are 5v LEDs.

On the left what I received back from JLCPCB. On the right, a completed board.
![Board Bare and Assembled](docs/board_bare_and_assembled.jpg)

![My Cabinet](https://raw.githubusercontent.com/shaneapowell/ArcadeIlluminate/master/docs/cabinet-01.jpg)

## See it in action on [YouTube](https://www.youtube.com/watch?v=Lij-8IOpjkc)
[![See it in Action](https://i9.ytimg.com/vi_webp/Lij-8IOpjkc/mqdefault.webp?time=1615676100000&sqp=CMSFtYIG&rs=AOn4CLB9gDKG8wdxP_Ji1OudFpIBKo9HMA)](https://www.youtube.com/watch?v=Lij-8IOpjkc)

# Features
- Control the LEDs of standard 5v LED Arcade Buttons.
- Compatible with common 3-pin button wiring configuration.
- Nearly Size-Identical most to Zero-Delay boards. For easy retrofit.
- Compatible with all emulator and game.
- Comes with a pre-configured button setup for most Mame games.
- Simple Text-Files allow easy modification or addition of games.
- Has a SleepMode to turn off the LEDs after a period of inactivity.

# Specifications
- Adafruit Trinket M0 CPU
- 12 jst 3-pin button ports.
- 4 jst 2-pin button ports.
- 1 jst 5-pin joystick port.
- 1 jst 2-pin 5v power port.
- 1 micro-usb port.
- max current per LED button 15ma.
- max current for all LED buttons 200ma.
- Compatible with common 5v LED arcade buttons and 3-pin connectors.
- Compatible with common 5-pin Joystick.


# Where can I get a board or 2?
I did an initial batch of 10 boards through JLCPCB.  I built 2 for myself, and 4 for 2 friends of mine.  I have 4 more boards on hand that I'll happily sell for cost + shipping. 

# The Circuit Board
The circuit board is 100% MIT OpenSource Licensed. Created with EasyEDA, and shared on oshwlab.
https://oshwlab.com/shaneapowell/arcadeilluminatev3

# How can I get a board or 2?
[Make Your Own](docs/board_assemble)

[Buy Read Made (not yet, but soon I hope)]()

I have ready-to-go Gerber, BOM and PNP files that can be used on [JLCPCB.com](https://jlcpcb.com/).  I used these to build my batch of 10. All 10 came out perfectly.
- [Gerber .zip file](docs/pcb_gerber.zip)
- [BOM .csv file](docs/pcb_bom.csv)
- [PickNPlace .csv file](docs/pcb_pnp.csv)

## Parts Needed
- [Main Circuit Board](https://oshwlab.com/shaneapowell/arcadeilluminatev3) I had them built and assembled by JLCPCB.  The only parts I had to solder in myself were the JST connectors, and the Trinket M0.
- [Adafruit Trinket M0](https://www.adafruit.com/product/3500) & Micro USB cable
- 12x B3B-XH JST thru-hole [Digikey #455-2231-ND](https://www.digikey.com/en/products/detail/jst-sales-america-inc/B3B-XH-AM(LF)(SN)/1651029)
- 4x B2B-XH JST thru-hole [Digikey #455-2273-ND](https://www.digikey.com/en/products/detail/jst-sales-america-inc/B2B-XH-AM(LF)(SN)/1016630)
- 1x B5B-XH JST thru-hole [Digikey #455-2239-ND](https://www.digikey.com/en/products/detail/jst-sales-america-inc/B5B-XH-AM(LF)(SN)/1651037)


# Install The Illiminate Board

# Software Install: 

1. Drop to the command propmpt by quitting Emulation Station.  `Main Menu -> Quit -> Quit Emulationstation`. Now, Ensure you have Python3 installed.  try typing `python --version` or `python3 --version` at a command prompt.   Retropie comes with python3 already installed, so you should be fine.  When you are done, type `exit` to go back to Emulationstation.

2. Using your main computer or laptop , open up your file explorer and browse to the file shares of your retropie machine.  `\\<ipaddr>\configs\`.

3. Copy the `event-horizon` folder from this git repo to the above shared configs folder on your retropie machine. The full path to this share is `/opt/retropie/configs`

4. Create the shell script `\\<ipaddr>\configs\all\runcommand-onstart.sh` with the following contents:
```
#!/bin/sh
system="$1"
rom=$(basename "$3" .zip)
python3 /opt/retropie/configs/event-horizon/illuminate.py $system" "$rom" &>/tmp/illuminate.log
```

5. Create the shell script `\\<ipaddr>\configs\all\runcommand-onend.sh` with the follwing contents:
```
#!/bin/sh
python3 /opt/retropie/configs/event-horizon/illuminate.py emulationstation default &>>/tmp/illuminate.log
```
    
	
6. This will need to be done again from the command terminal using the keyboard. Edit the shell script `/opt/retropie/supplementary/emulationstation`.  
```
sudo nano emulationstation.sh
```
7 Find the line 
```
$esdir/emulationstation "$@"
```
7 Above that line, add the following
```	
python3 /opt/retropie/configs/event-horizon/illuminate.py emulationstation default &>/tmp/illuminate.log
```	
    
# SETUP / CONFIGURE
- Identify Each Controller
- Test the buttons


# TROUBLESHOOTING
Check the output of illuminate.py when manually running it at the command prompt.  The logfile `/tmp/illuminate.log` is also a good place to start.  If need be, add the `-d` option to add more debugging output.  

# FAQ
***Q: Why do the lights flash slowly 2 times?***

A: This indicates that an error occured trying to find the correct buttons to light up. 4 show flashes means that some sort of "default" button configuration was used.  Check the logfile to see what errors caused this.

---
***Q: Why do the lights flash quickly 3 times?***

A: This indicates a success!  It's quicly telling you that the buttons that are lit up are correct.  It means a mapping was found in one of the .xml files, and in the config.ini file.

---
***Q: Can I help with this project?***

A: Yes!!  It's opensource afterall.  https://github.com/shaneapowell/EventHorizonIlluminate

---
***Q: I'm having some problems, where can I ask questions?***

A: I think the best place for now is to use the issues section of github.  Feel free to post issues, and even ask help questions.
https://github.com/shaneapowell/EventHorizonIlluminate/issues


---
***Q: How can I add button definitions for a system?***

A: TBD

---
***Q How can I control game specific buttons?***

A: TBA.. (modify the <system name>.ini file)


# RESOURCES:
- http://forum.arcadecontrols.com/index.php/topic,98241.0.html
- https://retropie.org.uk/forum/topic/1550/led-controller-that-works-with-retropie/13


# TODO:
- add usbhid.jspoll=1 to /boot/config.txt
- Add auto-config support to retro arch? (https://github.com/libretro/retroarch-joypad-autoconfig/blob/master/hid/8BitDo_Zero2_gamepad_BT.cfg)
