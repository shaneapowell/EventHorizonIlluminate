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

#include "main.h"
#include <SimpleCLI.h>
#include <stdint.h>


static const char* HELP_TEXT PROGMEM = 
"\n\
Usage: <command> [options] \n\
\n\
Commands:\n\
  on     <buttons>                       - Turn on the indicated LEDs.\n\
  off    <buttons>                       - Turn off the indicated LEDs.\n\
  flash  <count> <interval> <buttons>    - Flash the LEDs <count> times.\n\
  seq                                    - Run the boot animation sequence.\n\
  dump                                   - Print system process info.\n\
  help                                   - Print this help\n\
\n\
Options:\n\
     <buttons>   - space separated list of button indexes 1-12. Or the key word \"all\" for all buttons.\n\
     <count>     - The number of flashes.\n\
     <inteval>   - Time in milliseconds between flashes.\n\
\n\
Examples:\n\
    >flash 5 250 1 3 5       # Flash LEDs 1, 3, 5  5x with 250ms between.\n\
    >on all                  # Turn on all LEDs.\n\
    >off 1 4 12              # turn off 1 4 and 12\n\
\n\
Command Line Examples:\n\
    echo -e \"off all\\n flash 3 150 1 2\\n on 1 2\\n\" > /dev/ttyACM0     # Turn off all, flash btns 1,2 3x, turn on 1,2\n\
    echo -e \"off all\\n flash 2 400 1 2\\n on 2 4\\n\" > /dev/ttyACM0     # Turn off all, flash btns 1,2 2x, turn on 2,4\n\
";

/* An array of LEDS identified with the <buttons> param.  For each button, a T/F is placed here */
bool _leds[LED_COUNT] = {false};

/******************************************************************
 * Returns an uint16 value, with a bit for each ID provided.
 * The uint16 is 0 indexed, but the IDs provided are mapped from 1.
 * eg.. a led ID of 2 sets bit 1
 *****************************************************************/
static void _lightIdToLedArray(int startOffset, cmd* c) 
{
    /* Null them all out first */
    memset(_leds, false, sizeof(_leds[0]) * LED_COUNT );

    Command cmd(c);
    int argCount = cmd.countArgs();

    Argument arg = cmd.getArg(startOffset);
    String argValue = arg.getValue();

    /* Watch for the "all" keyword */
    if (argValue.equalsIgnoreCase("all"))
    {
        for (int btn = 1; btn <= LED_COUNT; btn++)
        {
            _leds[btn-1] = true;
        }
        return;
    }


    /* Normal ID mapping */
    for (int argIndex = startOffset; argIndex < argCount; argIndex++)
    {
        arg = cmd.getArg(argIndex);
        argValue = arg.getValue();
        int v = argValue.toInt();
        if (v > 0 && v <= LED_COUNT)
        {
            _leds[v] = true;
        }

    }
    
}

/*****************************************************************
 * Given the current led array state, turn the indicated
 * LEDs on or off.
*****************************************************************/
static void _setOn(bool on) 
{
    for (int index = 0; index < LED_COUNT; index++)
    {
        if (_leds[index])
        {
            /* Force the index to it's matching enum */
            gpioSetLed((LED)index, on);
        }
    }
}

/***********************************************************
 * Simple dump to serial the array of IDs
 **********************************************************/
static void _dumpIdArray() 
{
    for (int index = 0; index < LED_COUNT; index++)
    {
        if (_leds[index] != LED_NULL)
        {
            if (index > 0)
            {
                Serial.print(" ");
            }
            Serial.print(_leds[index]); 
        }
    }
}

/*****************************************************************
 * Respond to the "on" command.
*****************************************************************/
static void _onCommand(cmd* c) 
{
    _lightIdToLedArray(0, c);
    Serial.print("  -> Turning ON ["); _dumpIdArray(); Serial.println("]");
    _setOn(true);
}

/*****************************************************************
 * Respond to the "off" command.
*****************************************************************/
static void _offCommand(cmd* c)
{
    _lightIdToLedArray(0, c);
    Serial.print("  -> Turning OFF ["); _dumpIdArray(); Serial.println("]");
    _setOn(false);
}

/*****************************************************************
 * Respond to the "flash" command.
*****************************************************************/
static void _flashCommand(cmd* c)
{
    Command cmd(c);
    int count = cmd.getArg(0).getValue().toInt();
    int interval = cmd.getArg(1).getValue().toInt();
    _lightIdToLedArray(2, c);
    Serial.print("  -> FLASHING ["); _dumpIdArray(); Serial.println("]");
    while (count > 0)
    {
        _setOn(true);
        _threadDelayMs(interval);
        _setOn(false);
        _threadDelayMs(interval);
        count--;
    }

}

/*****************************************************************
 * respond to the "seq" command.
*****************************************************************/
static void _bootSeqCommand(cmd* c)
{
    // TODO:
}

/*****************************************************************
 * respond to the "dump" command.
*****************************************************************/
static void _dumpCommand(cmd* c)
{
    _dumpProcessMonitor();
}

/*****************************************************************
 * Create a thread that process the incomming serial commands,
 * then executes the request.
*****************************************************************/
void _threadProcessCmd( void *pvParameters ) 
{

    SimpleCLI _cli;
    Command _cmdOn = _cli.addBoundlessCommand("on", _onCommand);
    Command _cmdOff = _cli.addBoundlessCommand("off", _offCommand);
    Command _cmdFlash = _cli.addBoundlessCommand("flash", _flashCommand);
    Command _cmdBootSeq = _cli.addBoundlessCommand("seq", _bootSeqCommand);
    Command _cmdDump = _cli.addCmd("dump", _dumpCommand);

    const byte serialBufferSize = 32;
    char serialBuffer[serialBufferSize];   // an array to store the received data
    byte serialBufferIndex = 0;
    char serialEndMarker = '\n';
    char rc;

    while(true)
    {
                
        while (Serial.available() > 0) 
        {
            rc = Serial.read();
            Serial.write(rc);

            if (rc != serialEndMarker) 
            {
                serialBuffer[serialBufferIndex] = rc;
                serialBufferIndex++;
                if (serialBufferIndex >= serialBufferSize)
                {
                    serialBufferIndex = serialBufferSize - 1;
                }
            }
            else
            {
                serialBuffer[serialBufferIndex] = '\0';
                serialBufferIndex = 0;
            
                /* Process the Command */
                _cli.parse(serialBuffer);

                /* Output any errors */
                if (_cli.errored()) 
                {
                    Serial.println(_cli.getError().toString());
                    Serial.println(F("\n-------------------"));
                    Serial.print(F(PROGRAM_NAME)); Serial.print(F(" (")); Serial.print(F(PROGRAM_VERSION)); Serial.println(")");
                    Serial.println(HELP_TEXT);
                }

                Serial.write("\n\n>");
                memset(serialBuffer, '\0', sizeof(char)*serialBufferSize);

            }
            
        }

        taskYIELD();

    }

    vTaskDelete( NULL );
}


