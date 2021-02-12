# VID/PID Registered with http://pid.codes/
Import("env")
board_config = env.BoardConfig()
board_config.update("build.hwids", [
    ["0x1209","0xBDB1"] #VID,PID
])

