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
#include <ALLed.h>
#include <ALHIDJoystick.hpp>
#include "MCPPinSourceIMPL.h"
#include <HID-Project.h>

#define PIN_ONBOARD_LED     13
#define GPIO_DELAY_MS   4
const char* PROGRAM_NAME  =  "ArcadeIlluminate (v1.0)";


TaskHandle_t handleTaskJoystick;
TaskHandle_t handleTaskCmd;
TaskHandle_t handleTaskGpio;
TaskHandle_t handleTaskLed;

SemaphoreHandle_t _gpioSemaphore;

ALGpio*                     _gpio;
ALHIDJoystick<GamepadAPI>*  _hidJoystick;
ALCmd*                      _cmd;
ALLed*                      _led;

/****************************************************************
 * Can use these function for RTOS delays
 * Takes into account processor speed
 * Use these instead of delay(...) in rtos tasks
 *****************************************************************/
void _threadDelayUs(int us) { vTaskDelay( us / portTICK_PERIOD_US ); }
void _threadDelayMs(int ms) { vTaskDelay( ms/ portTICK_PERIOD_MS );  }

/**********************************************
 * Joystick HID Thread.
 * Highest Priority, blocked the the GPIO thread.
 **********************************************/ 
static void _threadProcessHidJoystick( void *pvParameters )
{
    while(true)
    {
        xSemaphoreTake(_gpioSemaphore, portMAX_DELAY);
        _hidJoystick->process(_led);
    }
  
    vTaskDelete( NULL );
}

/**********************************************
 * Hardware GPIO Thread.
 * 2nd highest priority, sleeps for about 4-5ms interval
 * resulting in a 200x / second RW speed.
 **********************************************/ 
static void _threadProcessGpio( void *pvParameters )
{
    unsigned long lastMs = millis();
    unsigned long thisMs = millis();
    while(true)
    {
        thisMs = millis();
        if (thisMs < lastMs) { lastMs = thisMs; } /* millis() wrap around */
        _gpio->process(thisMs - lastMs, _led);
        lastMs = thisMs;
        xSemaphoreGive(_gpioSemaphore);
        _threadDelayMs(GPIO_DELAY_MS); 
    }
  
    vTaskDelete( NULL );
}

/**********************************************
 * Command Thread.
 * Lowest priority. A few ticks missed here means very little.
 **********************************************/ 
static void _threadProcessCmd( void *pvParameters )
{
    while (true)
    {
        _cmd->process(_led);
        _threadDelayMs(50);
    }
    vTaskDelete( NULL );
}

/**********************************************
 * Status LED Thread.
 * AlsoLowest priority. A few ticks missed here means very little.
 **********************************************/ 
static void _threadProcessLed( void *pvParameters )
{
    while (true) 
    {
        _led->process();
        _threadDelayMs(100);
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
    _led = new ALLed();
    
    pinSource.begin();
    _gpio->begin();
    _hidJoystick->begin();
    _led->begin();

    vSetErrorLed(PIN_ONBOARD_LED, HIGH);
    vSetErrorSerial(&Serial);

    _gpioSemaphore = xSemaphoreCreateBinary();
    xTaskCreate(_threadProcessGpio,        "GPIO Task",      256, NULL, 2, &handleTaskGpio);
    xTaskCreate(_threadProcessHidJoystick, "Joystick Task",  256, NULL, 3, &handleTaskJoystick);
    xTaskCreate(_threadProcessCmd,         "Command Task",   384, NULL, 1, &handleTaskCmd);
    xTaskCreate(_threadProcessLed,         "LED Task",       256, NULL, 1, &handleTaskLed);

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

