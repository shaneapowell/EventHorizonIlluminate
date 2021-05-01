# System and Game Configurations
Event Horizon Illuminate Control Boards by themselves work pretty much the same as any other ZeroDelay board on the market.  Simply installing them and using them as is should work no differently.  The `magic` comes when you combine the Event Horizon boards with the retropie software control files provided here.  

## RetroPie event-horizon Software Files
This is the list of all retropie script and configuration files and their purpose, and how you can modify them to suit your needs.  The main thing these multiple config files do is provide layers of button name mappings all the way back to the port on the control board.  It is initially a little confusing to follow, but there is essentially 3 layers of button mappings.  
- 1: The `[controller]` button name to physical button ID.
- 2: The `[<system>]` button name to `[controller]` button name.
- 3: The `[<game>]` button name to `[<system>]' button name.

---
### `illuminate.py`
The python script at the heart of controlling the button lights.  This script reads the `configuration.ini` files for each specific system, looks up the corresponding game definition within the file, and sends the specific `flash` command to each controller.

---
### `runcommand-onstart.sh` 
This is a script that EmulationStation runs when a new game is started.  This script is how we hook the 'illuminate.py' python script into the running of a game.

---
### `runcommand-onend.sh` 
Similar to onstart.sh above, this script is run when a game is ended.  This will restore the button LEDs to the ones used by emulation station.

---
### `config.ini` 
The main config file used by `illuminate.py`. within this file is a number of sections, each designed to correctly map various button names to the final button port output on the controller.  There are also 2 special sections (`controller`, `emulationstation`) defined in this file.  

#### `[controller]` section 
This section is where the final controller hardware output ports are mapped to a button name.  The left value is the primary name of the system button.  The right value is the port number output on the controller board.   I do NOT recommend you alter the left values. The right values is where you can change the output port for each button if you are unable to connect the buttons to the board in the suggested default order.  The final special value that can be modified if needed is the `sort_controllers` value.  This will allow the `illuminate.py` script to change the order of controllers detected on your system from a `natural` sort order to a `reverse` sort order.  It is better to reverse your USB ports on your system than to use this setting.
```
[controller]
B1=1
B2=2
B3=3
B4=4
B5=5
B6=6
B7=7
B8=8
B9=9
B10=10
BPL=11  <-- Player
BCN=12  <-- Coin
sort_controllers=[natural|reverse]
```

#### `[emulationstation]` section
This section defines emulation station as a system to also be controlled.  Essentially allowing the buttons that emulation station uses to also be lit up much like any game can be lit up.  The left value is the button name defined for emulation station. This name isn't really important as the control software simply lights up all buttons found here.  The right value is the name of the button within the `controller` section above.
```
[emulationstation]
A=B2
B=B1
PLAYER=BPL
COIN=BCN
```

#### `<system>` section
Finally, still within this config.ini file, there is a section defined for each system/emulator used.  This list is not complete yet, as I only personally use MAME and NES most of the time.  Each system definition has it's own button mapping list.  This is an example of the `arcade` button mapping.
```
# Mame/Arcade button mapping
[arcade]
PLAYER=BPL
COIN=BCN
BUTTON1=B1
BUTTON2=B2
BUTTON3=B3
BUTTON4=B4
BUTTON5=B5
BUTTON6=B6
BUTTON7=B7
BUTTON8=B8
BUTTON9=B9
BUTTON10=B10

# Nintendo Entertainment System
[nes]
A=B2
B=B1
SELECT=BPL
START=BCN

```
The above list shows the `arcade` emulator system and the `nes` system.  This list of buttons defines a button name unique to the system they define. Within the `arcade` system, the value on the left defines a button name that maps to a `[controller]` button name.  In other words, `BUTTON2` simply maps back to the `B2` item in the `[controller]` section.  In the `nes` section, button `A` maps to the `[controller]` button B2.

---
### `<system>.ini` 
Each emulator system has it's own `<system>.ini` file.  Within each config file is a section defined for each game.  The section name is the game filename without the `.zip` extension.  The names are also case sensitive.  For example, if the game file name is `Earthworm Jim (USA).zip`.  The section will look as follow:
```
[Earthworm Jim (USA)]
players=1
alternating=true
buttons=A,B,Y,SELECT,START
```
The 3 lines defined in this example are all required values.  

`players=X` = The number of players this game supports.
`alternating=[true/false]` = If the game has multiple players, does the game `alternate` with a single joystick? or does the game allow multiple joysticks at the same time.  For example, R-Type is multi-player, but uses a single joystick. Therefore, R-type uses `alternating=true`.  While Joust is multi-player but uses multiple joysticks.  Thus, Joust uses `alternating=false`.
`buttons=A,B,Y` = The list of buttons this game supports.  The button names are the left hand values defined within the `[<system>]` section of the main `config.ini` file.

With this `Earthworm Jim` example, it defines buttons `A,B,Y,SELECT,START` as used for this game.  Looking within the `[nes]` section of the `config.ini` file, we see that button `A` maps to `B2`.  Finally, `B2` within the `[controller]` section maps to the physical port `2` on the control board.

##

# Giving Back your Changes.
If you add a game config to a system file, or even create a whole new system config file, I would encourage you to please give back to this project your additions.  This will allow the list of configured games to continue to grow and grow.  It's not yet setup for really easy updates to these definitions.  It's a work in progress to make .ini file changes easy to download to your local system.

Pull Requests will be accepted to the master branch for games and systems.

If you don't know how to make changes and post PRs to github, you can post a comment in a new [Github Issue](https://github.com/shaneapowell/EventHorizonIlluminate/issues) and I'll manually add your configuration changes. 