#!/bin/sh
system="$1"
rom=$(basename "$3" .zip)
python3 /home/pi/illuminate/illuminate.py "$system" "$rom" /dev/ttyACM0 &>/tmp/illuminate.log

