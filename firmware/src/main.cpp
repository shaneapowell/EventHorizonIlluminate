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

TaskHandle_t handleTaskJoystick;
TaskHandle_t handleTaskCmd;
TaskHandle_t handleTaskGpio;

/****************************************************************
 * Can use these function for RTOS delays
 * Takes into account processor speed
 * Use these instead of delay(...) in rtos tasks
 *****************************************************************/
void _threadDelayUs(int us)
{
    vTaskDelay( us / portTICK_PERIOD_US );  
}

void _threadDelayMs(int ms)
{
    vTaskDelay( ms/ portTICK_PERIOD_MS );  
}


/************************************************
 * Setup 
 ************************************************/
void setup() 
{
    Serial.begin(57600);
    delay(1000);  // prevents usb driver crash on startup, do not omit this
    
    pinMode(PIN_ONBOARD_LED, OUTPUT);

    vSetErrorLed(PIN_ONBOARD_LED, HIGH);
    vSetErrorSerial(&Serial);


    xTaskCreate(_threadProcessGpio,     "GPIO Task",      256, NULL, tskIDLE_PRIORITY + 1, &handleTaskGpio);
    xTaskCreate(_threadProcessJoystick, "Joystick Task",  256, NULL, tskIDLE_PRIORITY + 1, &handleTaskJoystick);
    xTaskCreate(_threadProcessCmd,      "Command Task",   384, NULL, tskIDLE_PRIORITY + 1, &handleTaskCmd);

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


//*****************************************************************
// Task will periodically print out useful information about the tasks running
// Is a useful tool to help figure out stack sizes being used
// Run time stats are generated from all task timing collected since startup
// No easy way yet to clear the run time stats yet
//*****************************************************************
static char ptrTaskList[400]; //temporary string buffer for task stats

void _dumpProcessMonitor()
{
    int x;
    int measurement;
    

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
