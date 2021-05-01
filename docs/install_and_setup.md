# Install and Setup
Install and Setup of an Event Horizaon board is broken into 4 distinct parts.
- The Wiring and Install
- The Event Horizon Illumiate Firmware
- RetroPie Control Software. 

Knowledge of the Linux Command line is not necessary, but will beneficial for testing and diagnostics and troubleshooting.

# The Wiring and Install
The configuration files provided by default are configured to expect the buttons and joystick to be connected into the control board ports in the same order as they are listed here.  If you can connect your buttons in this order, it will be much easier to use the software out of the box.  There will be little, to no configuration changes needed.
- B1 = Button 1 (B)
- B2 = Button 2 (A)
- B3 = Button 3
- B4 = Button 4
- B5 = Button 5
- B6 = Button 6
- B7 = Button 7
- B8 = Button 8
- B11 = PLAYER
- B12 = COIN

If you are unable to use the default button order, you will be able to change your button mapping (See `Setup/Configure` below).

# The Firmware
The EventHorizon Illuminate control board uses a TrinketM0 for it's CPU.  Installing, or updating the firmware on this board is actually quite easy thanks to the UF2 support built into this board.  You can chose to simply download and install the latest firmware, or you can build your own.

---
## Download and Install Current Release Firmware
The most recent release firmware can be downloaded from the [git-repo uf2](https://github.com/shaneapowell/EventHorizonIlluminate/tree/TotalRewrite/firmware/uf2) directory.  Firmware files will be named as `firmware-X.Y.uf2`. With the X and Y values being the current release version.
If you are starting with a brand new TrinketM0, or one already running a version of the Event Horizon firmware, these steps are the same.
1. Plug the USB of the TrinketM0 into your computer.  If this is a new TrinketM0, the device should appear as a [`CircuitPyhon CDC control`](firmware_windowsdm_newM0.png) device.  If this is a device already running Event Horizon firmware, it should appear as an [`Illuminate Joystick`](firmware_windowsdm_illuminate.png) device.
2. Put the trinket into `bootloader` mode by pressing the reset button on the TrinketM0 twice quickly. This should result in a new filesystem device named `TRINKETBOOT` being made available on your machine.  The small onboard red LED should be slowly flashing/fading in and out.  You can browse the contents of this device with your file explorer. If you see these files, you are successfully in bootloader mode.
    - CURRENT.UF2
    - INDEX.HTM
    - INFO_UF2.TXT
3. Drag and Drop (or just copy) the downloaded firmware `firmware-X.Y.uf2` file directly to the `TRINKETBOOT` device. The device should automatically reboot after the copy is done.
4. On Windows, you should now see a device in the Device Manager named [`Illuminate Joystick`](firmware_windowsdm_illuiminate.dm).  On Linux `lsusb` should show each `Event Horizon Illuminate Joystick` connected.

At this point , you have successfully updated/installed the Event Horizon Illuminate Joystick firmware onto your board.  The same steps can be followed for any future updates and bug to the firmware. 

---
## Compile and Install Your Own  Firmware manually
 1. Clone the [git-repo](https://github.com/shaneapowell/EventHorizonIlluminate) to your local machine.
 2. Install [PlatformIO](https://platformio.org/) onto your local machine.
 3. From the `firmware` directory within the cloned repo directory, execute the following command to build the TrinketM0 `firmware.bin`.
 ```
 $ pio run
 ```
 4. Install the new firmware onto your TrinketM0 board.  Plug your TrinketM0 into the USB of your machine.  
 ```
 $ pio run --target upload
 ```
 5. To create your own uf2 `firmware.uf2` file to manually install the firmware with the uf2 file format.
 ```
 $ cd uf2
 $ ./uf2conv.py -c -o firmware.uf2 -f SAMD21 ../.pio/build/TrinketM0/firmware.bin 
 ```


# The RetroPie Control Software: 
The RetroPie portion of the EventHorizon Illuminate control board provides the mechanism that controls which LEDs are lit up based on the game being played.  When you launch into a game, these script files run on your retropie system and send the appropriate commands to each Event Horizon control board.  These instructions also assume you have your retropie system connected to your home network, either hardwired or wifi.  

## Download Softrware Files
You will need the files contiained within the [`event-horizon` folder from the git-repo](https://github.com/shaneapowell/EventHorizonIlluminate/tree/TotalRewrite/event-horizon).  Git doesn't make it trivial to just download the files unfortunatly.  When you select a file to download, it will show you the file contents still within the github system.  To then download the file, click the `RAW` button near the top right of the file view.  This will show the file contents as it is stored on github.  From there, simply `CTRL-S` or `File -> Save` to save the file locally.

Alternatively, you can directly download the list from here.  This list here must be manually maintained, so I would suggest you confirm I did not miss a newly introduced `system.ini` file.
- [illuminate.py](https://raw.githubusercontent.com/shaneapowell/EventHorizonIlluminate/TotalRewrite/event-horizon/illuminate.py)
- [runcommand-onstart.sh](https://raw.githubusercontent.com/shaneapowell/EventHorizonIlluminate/TotalRewrite/event-horizon/runcommand-onstart.sh)
- [runcommand-onend.sh](https://raw.githubusercontent.com/shaneapowell/EventHorizonIlluminate/TotalRewrite/event-horizon/runcommand-onend.sh)
- [config.ini](https://raw.githubusercontent.com/shaneapowell/EventHorizonIlluminate/TotalRewrite/event-horizon/config.ini)
- [arcade.ini](https://raw.githubusercontent.com/shaneapowell/EventHorizonIlluminate/TotalRewrite/event-horizon/arcade.ini)
- [emulationstation.ini](https://raw.githubusercontent.com/shaneapowell/EventHorizonIlluminate/TotalRewrite/event-horizon/emulationstation.ini)
- [megadrive.ini](https://raw.githubusercontent.com/shaneapowell/EventHorizonIlluminate/TotalRewrite/event-horizon/megadrive.ini)
- [n64.ini](https://raw.githubusercontent.com/shaneapowell/EventHorizonIlluminate/TotalRewrite/event-horizon/n64.ini)
- [nes.ini](https://raw.githubusercontent.com/shaneapowell/EventHorizonIlluminate/TotalRewrite/event-horizon/nes.ini)
- [psx.ini](https://raw.githubusercontent.com/shaneapowell/EventHorizonIlluminate/TotalRewrite/event-horizon/psx.ini)
- [snes.ini](https://raw.githubusercontent.com/shaneapowell/EventHorizonIlluminate/TotalRewrite/event-horizon/snes.ini)

Place each of the downloaded files into a folder named `event-horizon`. This is the folder we will need to then copy over to your retropie system.

## Install and Setup 
1. Drop to the command prompt by quitting Emulation Station.  `Main Menu -> Quit -> Quit Emulationstation`. Now, Ensure you have Python3 installed.  try typing `python3 --version` at a command prompt.   Retropie comes with python3 already installed, so you should be fine.  If you do not have python3 installed, you will need to do that. Setting up and installing python3 is beyond the scope of these instructions.  When you are done, type `exit` to go back to Emulationstation.

2. Using your main home computer, open up your file explorer and browse to the file shares of your retropie machine.  `\\<ipaddr>\configs\`.  This is the shared directory `/opt/retropie/configs` on your retropie.  If you have not yet changed the default password on your retropie, the username is `pi` and the password is `raspberry`.

3. You should see a list of many different config directories:
    - all
    - amstradcpc
    - arcade
    - atari800
    - ... etc

4. Copy the entire [`event-horizon` folder](https://github.com/shaneapowell/EventHorizonIlluminate/tree/TotalRewrite/event-horizon) from this git repo to the above shared configs folder on your retropie machine. The final destination will be `/opt/retropie/configs/event-horizon`.

5. The file `\\<ipaddr>\configs\all\runcommand-onstart.sh` will need to be created.  If you already have this file on your system (fresh installs of RetroPie do not have this file), you will need to add the contents below manually.  If you do not already have this file, copy the `runcommand-onstart.sh` file from within the `event-horizon` folder you created in step 4.
```
#!/bin/sh
system="$1"
rom=$(basename "$3" .zip)
python3 /opt/retropie/configs/event-horizon/illuminate.py $system" "$rom" &>/dev/shm/illuminate.log
```

6. The file `\\<ipaddr>\configs\all\runcommand-onend.sh` is also needed.  Similar to step 5, add the following content to the existing file, or copy the file from the `event-horizon` folder from step 4.
```
#!/bin/sh
python3 /opt/retropie/configs/event-horizon/illuminate.py emulationstation default &>>/dev/shm/illuminate.log
```
	
7. This step will need to be done again from the command terminal using the keyboard (see step-1 for how to access the command prompt).  This particular file is not accessible from the shared folder. Using the file editor caled `nano`, edit the shell script `/opt/retropie/supplementary/emulationstation`
```
$ cd /opt/retropie/supplementary/emulationstation
$ sudo nano emulationstation.sh
```
8. Find the line 
```
$esdir/emulationstation "$@"
```
9. Above that line, add the following single line:
```	
python3 /opt/retropie/configs/event-horizon/illuminate.py emulationstation default &>/dev/shm/illuminate.log
```	
10. The file should look like this when you are done
```
#!/bin/sh
esdir="$(dirname $0)"
while true; do
    rm -f /tmp/es-restart /tmp/es-sysrestart /tmp/es-shutdown
    python3 /opt/retropie/configs/event-horizon/illuminate.py emulationstation default &>/dev/shm/illuminate.log
    "$esdir/emulationstation" "$@"
    ret=$?
    [ -f /tmp/es-restart ] && continue
    if [ -f /tmp/es-sysrestart ]; then
        rm -f /tmp/es-sysrestart
        sudo reboot
        break
    fi
    if [ -f /tmp/es-shutdown ]; then
        rm -f /tmp/es-shutdown
        sudo poweroff
        break
    fi
    break
done
exit $ret

```
11. To save the file press `CTRL-X`.  Nano should prompt you at the bottom to `Save modified buffer?`.  press `Y` and hit `ENTER` to write the file.

Congratulations.. all the necessary software is installed and ready to go.  Included with the default configurations of this software is settings for most `arcade` based roms, and a few of my regularly played SNES games.

If you restart EmulationStation, or restart your system, initially all lights should light up. Once emulation station starts however, the buttons A&B should quickly flash 2x to show that emulation station is ready to go.
    
# SETUP / CONFIGURE
Out of the box, most of the default settings and configurations should work as is.  Ideally, it will be far easier to change the connected USB port, or connected button port than to alter the default configuration settings.

## Identify and Set Order of Each Controller
If you have more than 1 Event Horizon Illuminate control boards installed on your system, the order they are detected will be important.   The `illuminate.py` software that sends commands to the boards to control the LEDs, identifies each board as Player 1, Player 2, Player 3.. etc, based on the order of the hardware USB ports they are connected to.  You can identify which controllers are being found in which order by running the following from the command prompt
```
$ python3 /opt/retropie/configs/event-horizon/illuminate.py identify controllers
```
Player 1 will flash all buttons once, Player 2 will flash al buttons twice... etc.

Alternatively, you can also calculate which controller should be identified as 1 and 2 based on the USB `hardware location`.  The USB location is the unique number value assigned to each USB port on your system.  These location values look something like `1-1.3:1.0`.  The `illuminate.py` script sorts your controlers by this location value.  You can manually determine which of your controllers is connected to which unique USB location ahead of time if desired.
- Unpug both controllers USB cables.
- Plug in one controller.
- From the terminal command prompt run the command `dmesg`.
- You should see the most recent device message at the bottom.  It should contain a line with a `ttyACM0` value in it.  As well as the USB location value similar `1-1.3:1.90`.
- Plug in your 2nd controllers USB cable.  
- Again, run the `dmesg` command and again look for the additional `ttyACM1` device, and it's USB location value.

You now have 2 unique location values.  These values do not change when your raspberry pi reboots.  They are hardware locations.  The order the `illuminate.py` script sorts your devices is a simple `alphanumeric` order of those 2 values.  In the case of my personal system I have the following location values, in the following order.
- `1-1.2:1.0`  (ttyACM0)
- `1-1.3:1.0`  (ttyACM1)

---
## Identify The buttons
You can also identify and test each button on each controller.  Run the following command to flash Button 1, then Button 2, and Button 3. on all controllers.
```
$ python3 /opt/retropie/configs/event-horizon/illuminate.py identify B1
$ python3 /opt/retropie/configs/event-horizon/illuminate.py identify B2
$ python3 /opt/retropie/configs/event-horizon/illuminate.py identify B3
...
$ python3 /opt/retropie/configs/event-horizon/illuminate.py identify B12

```
You can do the above for all LED connected buttons to ensure you have all buttons in the order you expect, and also that all controllers have all of their buttons in the same button port.

---
## Manually Access Controller Interface
Each controller presents itslef to retropie as both a `Joystick Input device`, and also as a `Serial TTY device`.  The joystick interface is how the games receive input, but the tty device is how flash commands are sent to each controller.  You can manually attempt to control a controller by connecting to it using `miniterm` (miniterm must be installed on your system first).  Each controller on your system will create a tty device named `/dev/ttyACM0` or `/dev/ttyACM1`, etc.
```
miniterm /dev/ttyACM0
```
To quit miniterm, type `CTRL-]`.  For help while connected type `help` and hit enter.

## Configure Retro Pie
At this point, you should be ready to setup `Emulation Station` and `MAME` to use your new controller boards.  Follow the usual steps in Emulation Station to configure your boards, and the steps to configure your controllers within MAME.  These steps are beyond the scope of this document.

# TROUBLESHOOTING
- Check the output of illuminate.py when manually running it at the command prompt.  
- The logfile `/dev/shm/illuminate.log` is also a good place to start.  If need be, add the `-d` option to add more debugging output.  


# FAQ
***Q: Why do the lights flash slowly 2 times?***

A: This indicates that an error occured trying to find the correct buttons to light up. 4 show flashes means that some sort of "default" button configuration was used.  Check the logfile to see what errors caused this.

---
***Q: Why do the lights flash quickly 3 times?***

A: This indicates a success!  It's quicly telling you that the buttons that are lit up are correct.  It means a mapping was found in one of the .xml files, and in the config.ini file.

---
***Q: Why do the buttons turn off if I leave the machine alone?***

A: LEDs are reliable, and use less power than old-school lights, but they do still suffer a little over time.  In an effort to try and extend the life of the LEDs, Event Horizon Illuminate boards have a built in timeout.  After 30 minutes of no activity, the buttons are turned off autmatically.  They will be turned back on as soon as any input is received.

---
***Q: Why does Player 2 buttons light up? instead of Player 1 for a single player game?

A: In short, you have the control boards plugged into your usb ports backwards.  the `illuminate.py` script uses the hardware order of the raspberry pi USB ports to determinie which controller is #1, #2, #3, .. etc.  The best solution would be to simply reverse the USB plugs.  Alternatively , you can [modify the `config.ini`]((system_configs.md)) file to reverse the order.  

---
***Q: Can I help with this project?***

A: Yes!!  It's opensource afterall.  https://github.com/shaneapowell/EventHorizonIlluminate

---
***Q: I'm having some problems, where can I ask questions?***

A: I think the best place for now is to use the issues section of github.  Feel free to post issues, and even ask help questions.
https://github.com/shaneapowell/EventHorizonIlluminate/issues


---
***Q: How can I add button definitions for a system?***

A: A: See [System and Game Configurations](system_configs.md)

---
***Q How can I control game specific buttons?***

A: A: See [System and Game Configurations](system_configs.md)

---
***Q: How can I get the latest game and button definitions?***

A: See [System and Game Configurations](system_configs.md)

# RESOURCES:
- http://forum.arcadecontrols.com/index.php/topic,98241.0.html
- https://retropie.org.uk/forum/topic/1550/led-controller-that-works-with-retropie/13


# Developer TODO:
- add usbhid.jspoll=1 to /boot/config.txt
- Add auto-config support to retro arch? (https://github.com/libretro/retroarch-joypad-autoconfig/blob/master/hid/8BitDo_Zero2_gamepad_BT.cfg)
