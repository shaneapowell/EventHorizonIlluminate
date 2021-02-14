
# UPDATE(Jan 13 2021) - Major Re-Write and Re-Work under way v2.0 is going to be a vast improvement.  Please Stand By.

I'm in the process of doing a total re-write of this project. Instead of being an add-on to an existing encoder board, I'm creating a fully open-source encoder board that will include the ability to fully control the LED button lights.

# INCOMPLETE - A work still in progress... Please check back soon!! #
This project is a work in progress with very active development.  Please be patient and check back soon for a complete and useable tool.

# NOTES for v2:
- add usbhid.jspoll=1 to /boot/config.txt
- Add auto-config support to retro arch? (https://github.com/libretro/retroarch-joypad-autoconfig/blob/master/hid/8BitDo_Zero2_gamepad_BT.cfg)

# TBD...

# Event Horizon - Illuminate (TBD)
A RetroPie joystick&button encoder board that also controls the LED Buttons.  Lighting up ONLY the buttons used by the game you're play.
A nearly drop-in replacement for the common zero-delay encoder boards used by most retropie builds.  Only works with 5v LED buttons.  Non-Led 
buttons will almost certainly burn up the control board.  

![My Cabinet](https://raw.githubusercontent.com/shaneapowell/ArcadeIlluminate/master/docs/cabinet-01.jpg)

## See it in action on [YouTube](https://www.youtube.com/watch?v=_jkyXNKug2k)
[![See it in Action](https://upload.wikimedia.org/wikipedia/commons/thumb/4/40/Youtube_icon.svg/256px-Youtube_icon.svg.png)](https://www.youtube.com/watch?v=_jkyXNKug2k)

# Specifications
TBD

# Note:
These instructions are currently tailored for a RetroPie install, adjustments might be necessary if you are using this software on a non RetroPie based system.

# Build the Circuit Board
TBD

## Parts Needed
TBD

## Tools Needed
- Soldering Iron
- Solder

# Install The Illiminate Board

# Software Install: TBD
(I'm going to assume some basic linux command line skills here)

1 Ensure you have Python3 installed.  try typing "python --version" or "python3 --version" at a command prompt.   Retropie comes with python3 already installed.

3 Download the `illuminate.py`, `config.ini` and each `<system>.ini` file into their own folder. `/opt/retropie/configs/event-horizon`.  The easiest way to get all the files here is to just clone the repo.  Given that I intend to try and keep the xml definition files as up to date as I can.. this might be good for auto-updating your own instalation. 
> cd /home/pi; git clone https://github.com/shaneapowell/ArcadeIlluminate.git illuminate

4 Create the shell script `/opt/retropie/configs/all/runcommand-onstart.sh` with the following contents:
```
#!/bin/sh
system="$1"
rom=$(basename "$3" .zip)
/home/pi/illuminate/illuminate.py "$system" "$rom" &>/tmp/illuminate.log
```

5 Create the shell script `/opt/retropie/configs/all/runcommand-onend.sh` with the follwing contents:
```
#!/bin/sh
/home/pi/illuminate/illuminate.py emulationstation default
```
    
		
6 Edit the shell script `emulationstation.sh` located in `/opt/retropie/supplementary/emulationstation`.  
```
sudo nano emulationstation.sh
```
7 Find the line 
```
$esdir/emulationstation "$@"
```
7 Above that line, add the following
```	
/opt/retropie/configs/event-horizon/illuminate.py emulationstation default &>>/tmp/illuminate.log
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
***Q: Why do the lights flash quickly 4 times?***

A: This indicates a success!  It's quicly telling you that the buttons that are lit up are correct.  It means a mapping was found in one of the .xml files, and in the config.ini file.

---
***Q: Can I help with this project?***

A: Yes!!  See `CONTRIBUTE` below.

---
***Q: How can I add button definitions for a system?***

A: Button definitions are defined in the `systems.ini` file.  Each emulation system has it's own section defined.  Simply add the section for the system that is missing, map the system specific button name to the button name in `config.ini`.  Now, at a minimum, the default buttons should light up when you launch that emulator.

---
***Q How can I control game specific buttons?***

A: TBA.. (modify the <system name>.xml file)

---
# FUTURE PLANS


# RESOURCES:
- http://forum.arcadecontrols.com/index.php/topic,98241.0.html
- https://retropie.org.uk/forum/topic/1550/led-controller-that-works-with-retropie/13
