#!/bin/sh
# Copy this file into the /opt/retropie/all folder.  
# It's the same folder as the "configs/all" windows share.
system="$1"
rom=$(basename "$3" .zip)
python3 /opt/retropie/even-horizon/illuminate.py "$system" "$rom" &>/dev/shm/illuminate.log

