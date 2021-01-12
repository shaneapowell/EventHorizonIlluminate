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

#include <FreeRTOS_SAMD21.h>
#include <ALGpio.h>
#include <ALCmd.hpp>
#include <ALHIDJoystick.hpp>
#include "MCPPinSourceIMPL.h"
#include <HID-Project.h>

#define PIN_ONBOARD_LED     13

const char* PROGRAM_NAME  =  "ArcadeIlluminate (v1.0)";

TaskHandle_t handleTaskJoystick;
TaskHandle_t handleTaskCmd;
TaskHandle_t handleTaskGpio;

ALGpio*         _gpio;
ALHIDJoystick<GamepadAPI>*  _hidJoystick;
ALCmd*          _cmd;

/****************************************************************
 * Can use these function for RTOS delays
 * Takes into account processor speed
 * Use these instead of delay(...) in rtos tasks
 *****************************************************************/
void _threadDelayUs(int us) { vTaskDelay( us / portTICK_PERIOD_US ); }
void _threadDelayMs(int ms) { vTaskDelay( ms/ portTICK_PERIOD_MS );  }

/**********************************************
 * Joystick HID Thread
 **********************************************/ 
static void _threadProcessHidJoystick( void *pvParameters )
{
    while(true)
    {
        _hidJoystick->process();
        _threadDelayMs(1); 
    }
  
    vTaskDelete( NULL );
}

/**********************************************
 * Hardware GPIO Thread
 **********************************************/ 
static void _threadProcessGpio( void *pvParameters )
{
    while(true)
    {
        _gpio->process();
        /* approx <1ms to process + 4ms delay = 200x per second. Should be fast enough I think, and even do a bit of a debounce */
        _threadDelayMs(4); 
    }
  
    vTaskDelete( NULL );
}

/**********************************************
 * Command Thread
 **********************************************/ 
static void _threadProcessCmd( void *pvParameters )
{
    while (true)
    {
        _cmd->process();
        _threadDelayMs(10);
    }
    vTaskDelete( NULL );
}

/******************************************************************
 * Dump to Serial, process stats.
 ******************************************************************/
void _dumpProcessMonitor()
{
    char ptrTaskList[400]; //temporary string buffer for task stats
    

    // run this task afew times before exiting forever
    Serial.flush();
    Serial.println("");			 
    Serial.println("****************************************************");
    Serial.print("Free Heap: ");
    Serial.print(xPortGetFreeHeapSize());
    Serial.println(" bytes");

    Serial.print("Min Heap: ");
    Serial.print(xPortGetMinimumEverFreeHeapSize());
    Serial.println(" bytes");
    Serial.flush();

    Serial.println("----------------------------------------------------");
    Serial.println("Task            ABS             %Util");
    Serial.println("----------------------------------------------------");

    vTaskGetRunTimeStats(ptrTaskList); //save stats to char array
    Serial.println(ptrTaskList); //prints out already formatted stats
    Serial.flush();

    Serial.println("----------------------------------------------------");
    Serial.println("Task            State   Prio    Stack   Num     Core" );
    Serial.println("----------------------------------------------------");

    vTaskList(ptrTaskList); //save stats to char array
    Serial.println(ptrTaskList); //prints out already formatted stats
    Serial.flush();

    Serial.println("****************************************************");
    Serial.flush();

}


/************************************************
 * Setup 
 ************************************************/
void setup() 
{
    pinMode(PIN_ONBOARD_LED, OUTPUT);

    Serial.begin(57600);
    delay(3000);  // prevents usb driver crash on startup, do not omit this

    ALGpioPinSourceImpl pinSource = ALGpioPinSourceImpl();
    _gpio = new ALGpio(&pinSource);
    _hidJoystick = new ALHIDJoystick<GamepadAPI>(_gpio, &Gamepad);
    _cmd = new ALCmd(PROGRAM_NAME, _threadDelayMs, &Serial, _gpio, _dumpProcessMonitor);
    
    pinSource.begin();
    _gpio->begin();
    _hidJoystick->begin();

    vSetErrorLed(PIN_ONBOARD_LED, HIGH);
    vSetErrorSerial(&Serial);


    xTaskCreate(_threadProcessGpio,        "GPIO Task",      256, NULL, tskIDLE_PRIORITY + 1, &handleTaskGpio);
    xTaskCreate(_threadProcessHidJoystick, "Joystick Task",  256, NULL, tskIDLE_PRIORITY + 1, &handleTaskJoystick);
    xTaskCreate(_threadProcessCmd,         "Command Task",   384, NULL, tskIDLE_PRIORITY + 1, &handleTaskCmd);

    /* Start the RTOS, this function will never return and will schedule the tasks. */
    vTaskStartScheduler();

    /* error scheduler failed to start. should never get here */
    while(1)
    {
        Serial.println("Scheduler Failed! \n");
        Serial.flush();
        delay(1000);
    }
   
}


/************************************************
 * Main Looper. AKA RTOS Idle State.
 ************************************************/
void loop() 
{
    // Optional commands, can comment/uncomment below
    delay(100); //delay is interrupt friendly, unlike vNopDelayMS
    return;

}

