
# INCOMPLETE - A work still in progress... Please check back soon!! #
This project is a work in progress with very active development.  Please be patient and check back soon for a complete and useable tool.

# Arcade Illuminate
A super-simple circuit board and python script that can control arcade cabinet button LEDs.  Have the buttons light up on a per-game or per-system basis.  It's running on my system with 5v LEDs, but should have no problem running almost any voltage, and up to about an amp of current.  If you have the skills to build your own RetroPie cabinet.. odds are you have the skills to build one of these circuit boards.

How it works?  The circuit board is designed to be wired to the `ground`, or negative side of the LEDs.  The switching of the current is done by allowing the voltage to flow `into` the board from the LED, not sending voltage `out` to the LEDs.  The advantage of this is that you should be free to use almost any voltage on your LEDs as you want.  It also means you don't need to rely on your Raspberry Pi to provide the 5v of power to your LEDs.  The buttons I have, happen to be 5v LEDs.  This circuit board should have no trouble at all handling 12v LEDs.

This circuit board, combined with this software, and a "Zero Delay LED" usb encoder board is currently working PERFECTLY on my DIY 2-Player Arcade Cabinet.

![My Cabinet](https://raw.githubusercontent.com/shaneapowell/ArcadeIlluminate/master/docs/cabinet-01.jpg)

## See it in action on [YouTube](https://www.youtube.com/watch?v=_jkyXNKug2k)
[![See it in Action](https://upload.wikimedia.org/wikipedia/commons/thumb/4/40/Youtube_icon.svg/256px-Youtube_icon.svg.png)](https://www.youtube.com/watch?v=_jkyXNKug2k)

# Note:
These instructions are currently tailored for a RetroPie install, adjustments might be necessary if you are using this software on a non RetroPie based system.

# Build the Circuit Board
I have a simple [circuit and PCB design](https://easyeda.com/shaneapowell/RetroPieLightController-UxFXfxPJ1) on [EasyEDA](https://easyeda.com) called `RetroPieLightController`.  Don't be put off by having to build a circuit board. It's much easier than you might think.  I use the [Laser-Jet Printer PCB masking](http://www.instructables.com/id/PCB-etching-using-laser-printer/) method, and the [Hydrogen Peroxide + Muratic Acide based etchant](http://www.instructables.com/id/Stop-using-Ferric-Chloride-etchant!--A-better-etc/?ALLSTEPS). It's worked well enough for me to create quite a few home-made PCBs.  2 tips... use a Sharpie to fill in any spots on the copper where the printer ink didn't transfer, and don't be afraid to use plenty of heat and time on the Iron when transfering the ink.

If you really don't want to get into etching your own circuit board, I'm sure this schematic would be easy to transfer to one of those [read-made coper perf-boards](https://www.adafruit.com/products/2670) too.  If someone comes up with a clean perf-board layout, please send me the design and I'll put it here.

![schematic](https://easyeda.com/normal/Schematic-HeqomkhRP.png)

https://easyeda.com/normal/Schematic-HeqomkhRP.png

![PCB](https://easyeda.com/normal/PCB-BVv52CAaK.png)

https://easyeda.com/normal/PCB-BVv52CAaK.png

###Here is a PDF copy of the laser-printer mask I used to create my circuit board.###
[Printer Friendly BOTTOM LAYER PDF of the above PCB. in the source code 'docs' folder](https://github.com/shaneapowell/ArcadeIlluminate/raw/master/docs/led-control.pdf)

I'm not going to explain in detail how to build the PCB... hopefully after reading the Instructables I linked to above, you'll be well on your way to doing your own PCBs in no time (4+ minutes).

###Completed Board###
A look at my completed and soldered circuit board from the back.  Might not look like a mass-produced professional quality board, but it works perfectly.
![Soldered](https://raw.githubusercontent.com/shaneapowell/ArcadeIlluminate/master/docs/circuit-board-back-01.jpg)

Here it is installed close to the LED buttons for shorter wire runs.  Yes, I used red wire despite the connections being on the ground side of the LEDS. It's what I had available.  The globs of glue-gun on the headers are there to hold the wire headers together.  Instead of using an 8-pin header, I used individual ones and decided to hold them together with glue so whenever I needed to removed the wires from the board, they stayed in order.
![Installed](https://raw.githubusercontent.com/shaneapowell/ArcadeIlluminate/master/docs/circuit-board-installed.jpg)

## Parts Needed
Note.. I got all my parts in larger quantities from eBay since I use them for many other side-projects.  But, I'll provide some ADAFruit links for convenience. I've bought stuff from them before and had great success.
- 1 x [MCP23017](https://www.adafruit.com/products/732) IC in a DIP package.
- 2 x [UNL2803](https://www.adafruit.com/product/970) ICs in a DIP package.
- Some [Male pin headers](https://www.adafruit.com/products/2671) 20 to be exact, but a few extra won't hurt.
- Single Side [Copper PCB board](http://www.amazon.com/uxcell-Rectangle-Laminate-Printed-130x250mm/dp/B00N3WU91C/)
- Hydrogen Peroxide.  The standard stuff you can buy at any pharmacy.
- [Muratic Acid](http://www.lowes.com/pd_206474-34228-CR.MA.P.01_1z0weh7__?productId=3024039&pl=1) This is the same stuff you can buy at Lowes or HomeDepot for cleaning pools.  Find a friend who owns a pool, they might have some. You will only need about 1 oz anyway.
- 2 x 1.8Kohm resistors.  Truth be told, 1.8K resistors seems to be what the Raspberry Pi recommends for pullups. I used a pair of 1.5K resistors on mine.  I had tested with a pair of 4.7K resistors on my breadboard with success.  I see no reason why you couldn't use the standard 4.7K, or anywhere in between.
- 4-pin jumper wire * 
- 2 x 8-pin jumper wire *

>   .* The length needed will depend on the distance between your components in your cabinet.  I had to extend my 4-pin power & data wires, but the LED wires were just long enough.  Just!!  A couple of [50cm 2.54mm F/F 2 Pin Solderless Colored Breadboard Jumper Wire 5 Pcs](http://www.amazon.com/2-54mm-Solderless-Colored-Breadboard-Jumper/dp/B00O9YB430/) should work.

## Tools Needed
- Soldering Iron
- Solder
- [1/32 & 3/64 drill bits](http://www.lowes.com/pd_179434-353-628-02___?productId=1207193).
- I needed an adjustable [dremmel chuck](http://www.lowes.com/pd_107391-353-4486-03_0__?productId=1212175&Ntt=) to hold the drill bits.

# Wire the LEDs to the Circuit Board
TBA - I'll include photos and diagrams of exactly how mine is wired, and what parts I used. 

>insert completed wiring image here*

Worth noting, I used a joystick-button kit from Amazon that included a "Zero Delay LED" encoder with built-in 5v LED power pins.  I'll explain here soon (with photos) how I tweaked the wires to easily work with this software.  Also, to limit the current draw from my Raspberry Pi, I installed a small dedicated 2A powered USB hub to connect the controllers through.  This meant that all current requirements for the button LEDs are supplied by the hub, not by the Raspberry Pi. 

# Wire the Circuit Board to the Raspberrry Pi
TBA - I'll include photos an diagrams how to connect it here.
>insert completed wiring image here, and rpi closeup*
You can see the 4 wires running from the illuminate circuit board at the bottom, to my raspberry pi3 at the top.
![Internals](https://raw.githubusercontent.com/shaneapowell/ArcadeIlluminate/master/docs/internals-back-01.jpg)


# Software Install:
(I'm going to assume some basic linux command line skills here)

1 Ensure you have Python3 installed.  try typing "python --version" or "python3 --version" at a command prompt.   Retropie comes with python3 already installed.

2 Install the i2c Python library 
  `sudo apt-get install python3-smbus` 

3 Download the `illuminate.py`, `config.ini` and each `.xml` file into their own folder. eg `/home/pi/illuminate`.  The easiest way to get all the files here is to just clone the repo.  Given that I intend to try and keep the xml definition files as up to date as I can.. this might be good for auto-updating your own instalation. 
> cd /home/pi; git clone https://github.com/shaneapowell/ArcadeIlluminate.git illuminate

4 Create the shell script `/opt/retropie/configs/all/runcommand-onstart.sh` with the following contents:
```
#!/bin/sh
system="$1"
rom=$(basename "$2")
/home/pi/illuminate/illuminate.py "$system" "$rom" &>/tmp/illuminate.log
```

5 Create the shell script `/opt/retropie/configs/all/runcommand-onend.sh` with the follwing contents:
```
#!/bin/sh
/home/pi/illuminate/illuminate.py emulationstation default
```
    
		
6 Edit the shell script `emulationstation.sh` located in `/opt/retropie/supplementary/emulationstation`.  Find the line 
```
$esdir/emulationstation "$@"
```
7 Above that line, add the following
```	
/home/pi/illuminate/illuminate.py emulationstation default &>>/tmp/illuminate.log
```	
    
# SETUP / CONFIGURE
1 First, edit the file `config.ini`. The [pins] section should start like this, with the buttons orderd 0-15.
```
[pins]
B1=0
B2=1
B3=2
B4=3
B5=4
B6=5
B7=6
B8=7
B9=8
B10=9
B11=10
B12=11
B13=12
B14=13
B15=14
B16=15
```
2 Using the command line, we'll light up each button we're using and write down it's button order on a piece of paper.  This will give you the correct Button to Pin mapping incase you plugged your LEDs into pins slightly out of order like I did.
`./illuminate.py TEST B1`
You should now see one of your buttons lit up.  If it's button #1, congrats!!  Write it down, and keep going.  Light each button in order from B1 -> B16 and write down the console button number that you see lit up on your panel.  For example, if you try to light up B1 but button # 4 lights up on your console, write down `4` on your piece of paper.
```
./illuminate.py TEST B2
./illuminate.py TEST B3
./illuminate.py TEST B4
./illuminate.py TEST B5
...
./illuminate.py TEST B16
```
3 Now that you have a button -> Pin order written down.. perhaps it looks like this because you got first 2 & the last 4 backwards
`2,1,3,4,5,6,7,8,9,10,11,12,16,15,14,13`
The sequence of numbers represents the button mapped to a specific pin number.  The first button is pin #0, the last is pin #15.
```
B2 B1 B3 B4 B5 B6 B7 B8 B9 B10 B11 B12 B16 B15 B14 B13  <-- Buttons
 0  1  2  3  4  5  6  7  8  9   10  11  12  13  14  15  <-- Pins
```
4 Now, go back to your `config.ini` and modify the `[pins]` section to match your actual button->pin order.
```
[pins]
B1=1
B2=0
B3=2
B4=3
B5=4
B6=5
B7=6
B8=7
B9=8
B10=9
B11=10
B12=11
B13=15
B14=14
B15=13
B16=12
```
5 Go back and re-run step #2. Your buttons should now light up in the correct order. If not, keep doing Steps 2-4 until it's correct.

# TROUBLESHOOTING
Check the output of illuminate.py when manually running it at the command prompt.  The logfile `/tmp/illuminate.log` is also a good place to start.  If need be, add the `-d` option to add more debugging output.  

# FAQ
***Q: Why do the lights flash slowly 4 times?***

A: This indicates that an error occured trying to find the correct buttons to light up. 4 show flashes means that some sort of "default" button configuration was used.  Check the logfile to see what errors caused this.

---
***Q: Why do the lights flash quickly 2 times?***

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
***Q: What is up with the bloated confusing XML format for the game button mappings?***

A: Sorry about that... but.. since I had somewhere around 3500 MAME games to map, I wasn't about to try and manually map each game myself.  So, using the Controls.xml file from [!src](http://forum.arcadecontrols.com/index.php/topic,98241.0.html) I opted to continue using the same source format. That said.. I hate the XML format, so I'm already working on switching to a .ini format for the game mappings.

#CONTRIBUTE
I'm acceping pull-requests for any modifications to the button definition XML files.  I'll push any changes I make, and I encourage anybody to help me by pushing any changes they make too.

Also.. I'm not a circuit expert.. I'm a hobbyist.  If any electronics experts out there can improve on my design.. please do.. I'll happily link to your designs here.

# FUTURE PLANS
GPIO? Not everybody wants to bother with adding a MCP23107 chip for i2c support.  That said.. some of you will not event want to use the UNL2803 chip to sink the current.. someone will have very low current LEDs wired in, and want to sink, or source the current directly to the LEDs using the Raspberry Pis GPIO pins.  While, I don't think that is a good idea, I'm considering adding direct GPIO control in a future release.  Should be somewhat simple to add afterall.

#RESOURCES:
- http://forum.arcadecontrols.com/index.php/topic,98241.0.html
