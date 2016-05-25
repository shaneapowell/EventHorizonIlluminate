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
- Ensure you have Python3 installed.  try typing "python --version" or "python3 --version" at a command prompt.   Retropie comes with python3 already installed.

- Install the i2c Python library 
  > "sudo apt-get install python3-smbus" 

- Download the `illuminate.py`, `config.ini` and each `.xml` file into their own folder. eg `/home/pi/illuminate`.

- Edit the shell script `runcommand.sh` located in `/opt/retropie/supplementary/runcommand`
  - Find the section near the bottom that contains the following section of text:
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
    
    - Below that section add the following lines of text:
```
# Illumination #
/home/pi/illuminate/illuminate.py emulationstation default &>>/tmp/illuminate.log
# End-Illumination #
```
		
- Edit the shell script `emulationstation.sh` located in `/opt/retropie/supplementary/emulationstation`
  - Find the line 
```
./emulationstation "$@"
```
  - Above that line, add the following
```	
/home/pi/illuminate/illuminate.py emulationstation default &>>/tmp/illuminate.log
```	
    
# SETUP / CONFIGURE
TBA

#RESOURCES:
- http://forum.arcadecontrols.com/index.php/topic,98241.0.html
