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


#define LIGHT_COUNT  12
#define MAX_FLASH_COUNT  10
#define MAX_FLASH_INTERVAL 1000

static const char* HELP_TEXT PROGMEM = "Help:\n  \"on\" <buttons>\n  \"off\" <buttons>\n  \"flash\" <count> <intervalMs> <buttons>\n    <buttons> = space separated list of button indexes 1-12\n";

/* A crossreference lookup, the button ID 1-24, here gets the bit for the shift register.
Button IDs do not start at 0, but at 1. Hence, the first element is not used */
static const LED BUTTON_ID_TO_LED_LOOKUP[LIGHT_COUNT+1] = {
    LED_B1, /* Not Used */
    LED_B1, 
    LED_B2,
    LED_B3,
    LED_B4,
    LED_B5,
    LED_B6,
    LED_B7,
    LED_B8,
    LED_B9,
    LED_B10,
    LED_B11,
    LED_B12,
};


int _lightIds[LIGHT_COUNT];


/*****************************************************************
 * 
*****************************************************************/
void _on(bool on)
{
    Serial.print("on["); Serial.print(on); Serial.println("]");
    for (int index = 0; index < LIGHT_COUNT; index++)
    {
        int id = _lightIds[index];
        if (id > 0)
        {
            LED led = BUTTON_ID_TO_LED_LOOKUP[id];
            gpioSetLed(led, on);
        }
    }
}

/*****************************************************************
 * 
*****************************************************************/
void _flash(int flashCount, int intervalMs) 
{
    flashCount = max(0, min(flashCount, MAX_FLASH_COUNT));
    intervalMs = max(0, min(intervalMs, MAX_FLASH_INTERVAL));
    Serial.print(F("_flash count[")); Serial.print(flashCount); Serial.print(F("] interval[")); Serial.print(intervalMs); Serial.println(F("]"));
    for (int count = 0; count < flashCount; count++)
    {
        _on(true);
        _threadDelayMs(intervalMs);
        _on(false);
        _threadDelayMs(intervalMs);
    }
}

/******************************************************************
 * Returns an int array of up to LIGHT_COUNT in size, with a list of all passed light
 * index values.  0 -> COUNT-1.  The command line takes in values 1-24, this outputs an array with each
 * entry 1-24. with 0 or -1 meaning nothing.
 * eg.  1 3 4 outputs 0 2 3 -1 -1 -1 -1 -1 -1 -1
 *****************************************************************/
void _getLightIds(int startOffset, cmd* c) 
{
    memset(_lightIds, 0, sizeof(_lightIds));

    Command cmd(c);
    int index = 0;
    int argCount = cmd.countArgs();

    for (int argIndex = startOffset; argIndex < argCount; argIndex++)
    {
        Argument arg = cmd.getArg(argIndex);
        String argValue = arg.getValue();
        int v = argValue.toInt();
        if (v > 0 && v <= LIGHT_COUNT)
        {
            _lightIds[index] = v;
            Serial.print(" id["); Serial.print(index); Serial.print("] = "); Serial.println(v);
            index++;
        }
    }
    
}

/*****************************************************************
 * 
*****************************************************************/
void _onCommand(cmd* c) 
{
    _getLightIds(0, c);
    _on(true);
}

/*****************************************************************
 * 
*****************************************************************/
void _offCommand(cmd* c)
{
    _getLightIds(0, c);
    _on(false);
}

/*****************************************************************
 * 
*****************************************************************/
void _flashCommand(cmd* c)
{
    Command cmd(c);
    int count = cmd.getArg(0).getValue().toInt();
    int interval = cmd.getArg(1).getValue().toInt();
    _getLightIds(2, c);
    _flash(count, interval);
}

/*****************************************************************
 * 
*****************************************************************/
void _bootSeqCommand(cmd* c)
{
    _on(false);
    for (int id = 1; id <= LIGHT_COUNT; id++)
    {
        _lightIds[id-1] = id;
        _on(true);
        _threadDelayMs(50);
    }
    delay(100);
    for (int id = 1; id <= LIGHT_COUNT; id++)
    {
        _lightIds[id-1] = 0;
        _on(true);
        _threadDelayMs(50);
    }

}



/*****************************************************************
 * Create a thread that process the incomming serial commands,
 * then executes the request.
*****************************************************************/
void _threadProcessSerial( void *pvParameters ) 
{

    SimpleCLI _cli;
    Command _cmdOn = _cli.addBoundlessCommand("on", _onCommand);
    Command _cmdOff = _cli.addBoundlessCommand("off", _offCommand);
    Command _cmdFlash = _cli.addBoundlessCommand("flash", _flashCommand);
    Command _cmdBootSeq = _cli.addBoundlessCommand("seq", _bootSeqCommand);

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
                    Serial.println(F("-------------------"));
                    Serial.print(F(PROGRAM_NAME)); Serial.print(F("  ")); Serial.println(F(PROGRAM_VERSION));
                    Serial.println(HELP_TEXT);
                }

                Serial.write("\n\n>");
                memset(serialBuffer, '\0', sizeof(char)*serialBufferSize);

            }
            
        }

        //_threadDelayMs(1);
        taskYIELD();

    }

    vTaskDelete( NULL );
}


// //*****************************************************************
// // Task will periodically print out useful information about the tasks running
// // Is a useful tool to help figure out stack sizes being used
// // Run time stats are generated from all task timing collected since startup
// // No easy way yet to clear the run time stats yet
// //*****************************************************************
// static char ptrTaskList[400]; //temporary string buffer for task stats

// void _threadProcessMonitor(void *pvParameters)
// {
//     int x;
//     int measurement;
    
//     Serial.println("Task Monitor: Started");

//     // run this task afew times before exiting forever
//     while(1)
//     {
//     	_threadDelayMs(10000); // print every 10 seconds

//     	Serial.flush();
// 		Serial.println("");			 
//     	Serial.println("****************************************************");
//     	Serial.print("Free Heap: ");
//     	Serial.print(xPortGetFreeHeapSize());
//     	Serial.println(" bytes");

//     	Serial.print("Min Heap: ");
//     	Serial.print(xPortGetMinimumEverFreeHeapSize());
//     	Serial.println(" bytes");
//     	Serial.flush();

//     	Serial.println("****************************************************");
//     	Serial.println("Task            ABS             %Util");
//     	Serial.println("****************************************************");

//     	vTaskGetRunTimeStats(ptrTaskList); //save stats to char array
//     	Serial.println(ptrTaskList); //prints out already formatted stats
//     	Serial.flush();

// 		Serial.println("****************************************************");
// 		Serial.println("Task            State   Prio    Stack   Num     Core" );
// 		Serial.println("****************************************************");

// 		vTaskList(ptrTaskList); //save stats to char array
// 		Serial.println(ptrTaskList); //prints out already formatted stats
// 		Serial.flush();

// 		Serial.println("****************************************************");
// 		Serial.println("[Stacks Free Bytes Remaining] ");

// 		measurement = uxTaskGetStackHighWaterMark( handleTaskJoystick );
// 		Serial.print("Thread A: ");
// 		Serial.println(measurement);

// 		measurement = uxTaskGetStackHighWaterMark( handleTaskB );
// 		Serial.print("Thread B: ");
// 		Serial.println(measurement);

// 		measurement = uxTaskGetStackHighWaterMark( handleMonitorTask );
// 		Serial.print("Monitor Stack: ");
// 		Serial.println(measurement);

// 		Serial.println("****************************************************");
// 		Serial.flush();

//     }

//     // delete ourselves.
//     // Have to call this or the system crashes when you reach the end bracket and then get scheduled.
//     Serial.println("Task Monitor: Deleting");
//     vTaskDelete( NULL );

// }
