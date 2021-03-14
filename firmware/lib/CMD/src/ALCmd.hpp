/***
MIT License

Copyright (c) 2020 Shane Powell

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
***/

#pragma once

#include "ALGpio.h"
#include "ALLed.h"
#include <SimpleCLI.h>

#define CMD_SERIAL_BUFFER_SIZE 256

static const char* HELP_TEXT PROGMEM = 
"\n\
Usage: <command> [options] \n\
\n\
Commands:\n\
  on     <buttons>                       - Turn on the indicated LEDs.\n\
  off    <buttons>                       - Turn off the indicated LEDs.\n\
  flash  <count> <interval> <buttons>    - Flash the LEDs <count> times.\n\
  seq    <animation>                     - Run the named animation sequence.\n\
  dump                                   - Print system process info.\n\
  bootloader                             - Restart into bootloader mode.\n\
  help                                   - Print this help\n\
\n\
Options:\n\
     <buttons>   - space separated list of buttons 1-12. Or the key word \"all\" for all buttons.\n\
     <count>     - The number of flashes.\n\
     <inteval>   - Time in milliseconds between flashes.\n\
     <animation> - boot, fadein, fadeout.\n\
\n\
Examples:\n\
    >flash 5 250 1 3 5       # Flash LEDs 1, 3, 5  5x with 250ms between.\n\
    >on all                  # Turn on all LEDs.\n\
    >off 1 4 12              # turn off 1 4 and 12\n\
\n\
Command Line Examples:\n\
    echo \"off all; flash 3 150 1 2; on 1 2;\" > /dev/ttyACM0     # Turn off all, flash btns 1,2 3x, turn on 1,2\n\
    echo \"off all; flash 2 400 1 2; on 2 4;\" > /dev/ttyACM0     # Turn off all, flash btns 1,2 2x, turn on 2,4\n\
";

#define FLASH_COUNT_MIN     1
#define FLASH_COUNT_MAX     20
#define FLASH_INTERVAL_MIN  10
#define FLASH_INTERVAL_MAX  2000
#define ALL_LED_MASK 0xFFFF

typedef void (*fptrDelayMs)(int);
typedef void (*fptrMonitorDump)();

class ALCmd
{
    public:
        /******************************************************************
         * Constructor
         ******************************************************************/ 
        ALCmd(const char* progName, fptrDelayMs fptrD, Serial_* serial, ALGpio* gpio, fptrMonitorDump fptrM)
        { 
            _serial = serial;
            _progName = progName;
            _fptrDelayMs = fptrD; 
            _fptrMonitorDump = fptrM;
            _gpio = gpio;

            _cmdOn = _cli.addBoundlessCommand("on");
            _cmdOff = _cli.addBoundlessCommand("off");
            _cmdFlash = _cli.addBoundlessCommand("flash");
            _cmdSeq = _cli.addSingleArgumentCommand("seq");
            _cmdDump = _cli.addCommand("dump");
            _cmdBootloader = _cli.addCommand("bootloader");
            _cmdHelp = _cli.addCommand("help");

        }


        /*****************************************************************
         * Create a thread that process the incomming serial commands,
         * then executes the request.
        *****************************************************************/
        void process(ALLed* led) 
        {

            /* First time by default, we run up the boot sequence, and turn them all on. */
            if (!_isFinishedBootSeq)
            {
                Command cmd;
                cmd.addArg("boot");
                _seqCommand(NULL);
            }
            _isFinishedBootSeq = true;
                        
            /* Process the serial commands */
            while (_serial->available() > 0) 
            {
                _rc = _serial->read();
                _serial->write(_rc);

                if (_rc != _serialEndMarker1 && _rc != _serialEndMarker2) 
                {
                    _serialBuffer[_serialBufferIndex] = _rc;
                    _serialBufferIndex++;
                    if (_serialBufferIndex >= CMD_SERIAL_BUFFER_SIZE)
                    {
                        _serialBufferIndex = CMD_SERIAL_BUFFER_SIZE - 1;
                    }
                }
                else
                {
                    _serialBuffer[_serialBufferIndex] = '\0';
                    _serialBufferIndex = 0;
                
                    /* Process the Command */
                    _cli.parse(_serialBuffer);

                    if (_cli.available())
                    {
                        led->showState(LED_STATE_COMMAND);

                        Command cmd = _cli.getCmd();
                        if (cmd == _cmdOn) _onCommand(cmd.getPtr());
                        if (cmd == _cmdOff) _offCommand(cmd.getPtr());
                        if (cmd == _cmdFlash) _flashCommand(cmd.getPtr());
                        if (cmd == _cmdSeq) _seqCommand(cmd.getPtr());
                        if (cmd == _cmdDump) _fptrMonitorDump();
                        if (cmd == _cmdBootloader) _resetToBootloader();
                        if (cmd == _cmdHelp) _helpCommand();

                       
                    }

                    /* Output any errors */
                    if (_cli.errored()) 
                    {
                        led->showState(LED_STATE_ERROR);
                        _serial->println(_cli.getError().toString());
                        _helpCommand();
                    }

                    _serial->write("\n\n>");
                    memset(_serialBuffer, '\0', sizeof(char)*CMD_SERIAL_BUFFER_SIZE);

                }
                
            }

        }



    
    private:
        bool _isFinishedBootSeq = false;
        fptrDelayMs _fptrDelayMs;
        Serial_* _serial;
        ALGpio* _gpio;
        const char* _progName;
        fptrMonitorDump _fptrMonitorDump;

        SimpleCLI _cli;
    
        Command _cmdOn;
        Command _cmdOff;
        Command _cmdFlash;
        Command _cmdSeq;
        Command _cmdDump;
        Command _cmdBootloader;
        Command _cmdHelp;

        char _serialBuffer[CMD_SERIAL_BUFFER_SIZE];   // an array to store the received data
        byte _serialBufferIndex = 0;
        char _serialEndMarker1 = '\n';
        char _serialEndMarker2 = ';';
        char _rc;


        /******************************************************************
         * Returns an uint16 value, with a bit for each ID provided.
         * The uint16 is 0 indexed, but the IDs provided are mapped from 1.
         * eg.. a led ID of 2 sets bit 1
         *****************************************************************/
        uint16_t _buttonIdToLedMask(int startOffset, cmd* c) 
        {

            uint16_t ledMask = 0;
            Command cmd(c);
            int argCount = cmd.countArgs();

            Argument arg = cmd.getArg(startOffset);
            String argValue = arg.getValue();

            /* Watch for the "all" keyword */
            if (argValue.equalsIgnoreCase("all"))
            {
                return ALL_LED_MASK;
            }


            /* Normal ID mapping */
            for (int argIndex = startOffset; argIndex < argCount; argIndex++)
            {
                arg = cmd.getArg(argIndex);
                argValue = arg.getValue();
                int button = argValue.toInt();
                if (button > 0 && button <= LED_COUNT)
                {
                    ledMask = ledMask | (1 << (button-1));
                }

            }

            return ledMask;
            
        }


        /*****************************************************************
         * Given an LED bitmask that indicates which LEDs to alter, and
         * a value to alter those LEDs to.. well.. do just that.
        *****************************************************************/
        void _sendLedState(uint16_t ledMask, bool on) 
        {
            for (int index = 0; index < LED_COUNT; index++)
            {
                if (ledMask & (1 << index))
                {
                    _gpio->setLed(ALL_LEDS[index], on);
                }
            }
        }

        
        
        /*****************************************************************
         * Respond to the "on" command.
        *****************************************************************/
        void _onCommand(cmd* c) 
        {
            uint16_t ledMask = _buttonIdToLedMask(0, c);
            _serial->print("  -> Turning ON ["); _serial->print(ledMask, BIN); _serial->println("]");
            _sendLedState(ledMask, true);
        }


        /*****************************************************************
         * Respond to the "off" command.
        *****************************************************************/
        void _offCommand(cmd* c)
        {
            uint16_t ledMask = _buttonIdToLedMask(0, c);
            _serial->print("  -> Turning OFF ["); _serial->print(ledMask, BIN); _serial->println("]");
            _sendLedState(ledMask, false);
        }


        /*****************************************************************
         * Respond to the "flash" command.
        *****************************************************************/
        void _flashCommand(cmd* c)
        {
            Command cmd(c);
            int count = cmd.getArg(0).getValue().toInt();
            int interval = cmd.getArg(1).getValue().toInt();
            count = min(count, FLASH_COUNT_MAX);
            count = max(count, FLASH_COUNT_MIN);
            interval = min(interval, FLASH_INTERVAL_MAX);
            interval = max(interval, FLASH_INTERVAL_MIN);
            uint16_t ledMask = _buttonIdToLedMask(2, c);
            _serial->print("  -> FLASHING "); _serial->print(count); _serial->print("x @"); _serial->print(interval); _serial->print("ms ["); _serial->print(ledMask, BIN); _serial->println("]");

            _sendLedState(ledMask, false);
            _fptrDelayMs(interval);

            while (count > 0)
            {
                _sendLedState(ledMask, true);
                _fptrDelayMs(interval);
                _sendLedState(ledMask, false);
                _fptrDelayMs(interval);
                count--;
            }

        }


        /*****************************************************************
         * respond to the "bootseq" command. Just a quick set of 2 animated
         * on-off-on-off up the LEDs.
        *****************************************************************/
        void _seqCommand(cmd* c)
        {
            Command cmd(c);
            Argument arg = cmd.getArgument(0);
            String argVal = arg.getValue();

            _serial->print("  -> Sequence ["); _serial->print(argVal.c_str()); _serial->println("]");
            
            if (argVal == "boot")
            {
                /* Start Off */
                uint16_t ledMask = ALL_LED_MASK;
                _sendLedState(ledMask, true);

                /* off -> on -> off -> on */
                for (int onoff = 0; onoff < 4; onoff++)
                {
                    ledMask = 1;
                    for (int index = 0; index < LED_COUNT; index++)
                    {
                        _sendLedState(ledMask, onoff % 2 != 0);
                        _fptrDelayMs(20);
                        ledMask = (ledMask << 1) | 1;
                    }
                }
            }
            else
            {
                _serial->println("Unknown Sequence Name");
            }

        }

        /*****************************************************************
         * Reboot SAMD21 into bootloader mode for firmware updates.
         ****************************************************************/
        void _resetToBootloader() 
        {
            *(uint32_t *)(0x20000000 + 32768 -4) = 0xf01669ef;     // Store special flag value in last word in RAM.
            NVIC_SystemReset();    // Like pushing the reset button.
        }

        /*****************************************************************
         * respond to the "help" command.
        *****************************************************************/
        void _helpCommand()
        {    
            _serial->println(F("\n-------------------"));
            _serial->println(_progName);
            _serial->println(HELP_TEXT);
        }


};