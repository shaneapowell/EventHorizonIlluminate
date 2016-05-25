


INSTALL:
- Ensure you have Python3 installed.  try typing "python --version" or "python3 --version" at a command prompt.   Retropie comes with python3 already installed.

- Install the i2c Python library 
	"sudo apt-get install python3-smbus" 

- Unzip contents into illuminate folder.

- copy illuminate folder to your desired location, eg /home/pi

- Edit the shell script "runcommand.sh" located in "/opt/retropie/supplementary/runcommand"

	- Find the section near the bottom that contains the following section of text:

			# launch the command - don't redirect stdout for frotz,  when using console output or when not using _SYS_
			# frotz is included in case its emulators.cfg is out of date and missing CON: - can be removed in the future
			if [[ "$emulator" == frotz || "$is_console" -eq 1 || "$is_sys" -eq 0 ]]; then
				# turn cursor on
				tput cnorm
				eval $command </dev/tty 2>/tmp/runcommand.log
			else
				eval $command </dev/tty &>/tmp/runcommand.log
			fi


    - Above that section add the following lines of text:
		
		# Illumination #
		/home/pi/illuminate/illuminate.py "$system" "$rom_bn" &>/tmp/illuminate.log
		# End-Illumination #
		
		
	- Below that section add the following lines of text:

		# Illumination #
		/home/pi/illuminate/illuminate.py emulationstation default &>>/tmp/illuminate.log
		# End-Illumination #

		
- Edit the shell script "emulationstation.sh" located in "/opt/retropie/supplementary/emulationstation"
	- Find the line 

		./emulationstation "$@"

	- Above that line, add the following
	
		/home/pi/illuminate/illuminate.py emulationstation default &>>/tmp/illuminate.log
	
    


RESOURCES:
- http://forum.arcadecontrols.com/index.php/topic,98241.0.html
