# System and Game Configurations
Event Horizon Illuminate Control Boards by themselves work pretty much the same as any other ZeroDelay board on the market.  Simply installing them and using them as is should work no differently.  The `magic` comes when you combine the Event Horizon boards with the retropie software control files.  

## RetroPie event-horizon Software Files
This is the list of all retropie script and configuration files and their purpose, and how you can modify them to suit your needs.

### `illuminate.py`
The python script at the heart of controlling the button lights.  This script reads the configuration .ini files for each specific system, looks up the corisponding game definition within the file, and sends the specific `flash` command to each controller.

### `runcommand-onstart.sh` 
This is a script that EmulationStation runs when a new game is started.  This script is how we hook the 'illuminate.py' python script into the running of a game.

### `runcommand-onend.sh` 
Similar to onstart.sh above, this script is run when a game is ended.  This will restore the button LEDs to the ones used by emulation station.

### `config.ini` 
The main config file used by `illuminate.py`. within this file is a number of sections, each designed to correctly map various button names to the final button port output on the controller.

### `<system>.ini` 
Each emulator system has it's own config .ini file.  Within each config file is a section defined for each game.

##

# Giving Back your Changes.
If you add a game config to a system file, or even create a whole new system config file, I would encourage you to please give back to this project your additions.  This will allow the list of configured games to continue to grow and grow.

Pull Requests will be accepted to the master branch for games and systems.

If you dont know how.