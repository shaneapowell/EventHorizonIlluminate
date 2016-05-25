>MIT License
>
>Copyright (c) 2016 Shane Powell
>
>Permission is hereby granted, free of charge, to any person obtaining a copy
>of this software and associated documentation files (the "Software"), to deal
>in the Software without restriction, including without limitation the rights
>to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
>copies of the Software, and to permit persons to whom the Software is
>furnished to do so, subject to the following conditions:
>
>The above copyright notice and this permission notice shall be included in all
>copies or substantial portions of the Software.
>
>THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
>IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
>FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
>AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
>LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
>OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
>SOFTWARE.



# INCOMPLETE.. Please check back soon!! #
This project is a work in progress with very active development.  Please be patient and check back soon for a complete and useable tool.

# Note:
These instructions are currently tailored for a RetroPie install, adjustments might be necessary if you are using this software on a non RetroPie based system.

# Build the Circuit Board
TBA

# Wire the LEDs to the Circuit Board
TBA

# INSTALL:
1 Ensure you have Python3 installed.  try typing "python --version" or "python3 --version" at a command prompt.   Retropie comes with python3 already installed.

2 Install the i2c Python library 
  `sudo apt-get install python3-smbus` 

3 Download the `illuminate.py`, `config.ini` and each `.xml` file into their own folder. eg `/home/pi/illuminate`.

4 Edit the shell script `runcommand.sh` located in `/opt/retropie/supplementary/runcommand`.  Find the section near the bottom that contains the following section of text:
```
# launch the command - don't redirect stdout for frotz,  when using console output or when not using _SYS_
# frotz is included in case its emulators.cfg is out of date and missing CON: - can be removed in the future
if [[ "$emulator" == frotz || "$is_console" -eq 1 || "$is_sys" -eq 0 ]]; then
  # turn cursor on
  tput cnorm
  eval $command </dev/tty 2>/tmp/runcommand.log
else
  eval $command </dev/tty &>/tmp/runcommand.log
fi
```

    - Above that section add the following lines of text:
```
# Illumination #
/home/pi/illuminate/illuminate.py "$system" "$rom_bn" &>/tmp/illuminate.log
# End-Illumination #
```		
    
5 Below that section add the following lines of text:
```
# Illumination #
/home/pi/illuminate/illuminate.py emulationstation default &>>/tmp/illuminate.log
# End-Illumination #
```
		
6 Edit the shell script `emulationstation.sh` located in `/opt/retropie/supplementary/emulationstation`.  Find the line 
```
./emulationstation "$@"
```
7 Above that line, add the following
```	
/home/pi/illuminate/illuminate.py emulationstation default &>>/tmp/illuminate.log
```	
    
# SETUP / CONFIGURE
1 First, modify the `config.ini` file [pins] section to be a nicely ordered list.  I keep pushing my crazy pin order to the repo.. sorry about that. 
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

#RESOURCES:
- http://forum.arcadecontrols.com/index.php/topic,98241.0.html
