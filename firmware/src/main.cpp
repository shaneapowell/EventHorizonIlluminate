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
//#include <Arduino.h>
#include <SimpleCLI.h>
#include <stdint.h>
#include <HID-Project.h>


/************************************************
 * on 1 4 6 12     <- turns on lights 1 4 6 12
 * off 6 9 18 22   <- turns off lights 6 9 18 22
 * flash 3 400 6 8 <- flashes lights 6 8 3x 400ms interval
 ***********************************************/
#define PROGRAM_NAME    "ArcadeIlluminate"
#define PROGRAM_VERSION "1.0"
#define SERIAL          Serial //Sparkfun Samd21 Boards
#define PIN_ONBOARD_LED  13
#define PIN_DATA       1  // D1
#define PIN_CLOCK      3  // D3
#define PIN_STROBE     0 // D0
#define LIGHT_COUNT  24
#define MAX_FLASH_COUNT  10
#define MAX_FLASH_INTERVAL 1000
#define PIXEL_RED   0x550000
#define PIXEL_GREEN 0x005500
#define PIXEL_BLUE  0x000055
#define PIXEL_BLACK 0x000000

/* A crossreference lookup, the button ID 1-24, here gets the bit for the shift register */
static const int BUTTON_BIT_LOOKUP[LIGHT_COUNT+1] = {
    -1, /* Not Used */
    0, 
    1,
    2,
    3,

    7,
    6,
    5,
    4,

    8,
    9,
    10,
    11,

    15,
    14,
    13,
    12,

    16,
    17,
    18,
    19,

    23,
    22,
    21,
    20
};


int _lightIds[LIGHT_COUNT];

TaskHandle_t handleTaskJoystick;
TaskHandle_t handleTaskB;
TaskHandle_t handleMonitorTask;


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



void sendInt(uint32_t value)
{
    digitalWrite(PIN_STROBE, LOW);
    digitalWrite(PIN_CLOCK, LOW);
    digitalWrite(PIN_DATA, LOW);
    
    for (int index = 23; index > -1; index--)
    {
        if ((value >> index) & 1)
        {
            digitalWrite(PIN_DATA, HIGH);
        }
        else
        {
            digitalWrite(PIN_DATA, LOW);
        }
        digitalWrite(PIN_CLOCK, HIGH);
        digitalWrite(PIN_CLOCK, LOW);
    }

    digitalWrite(PIN_STROBE, HIGH);

}



/* Create a 32bit mask from the light index array. [1, 3, 4] == 0x */
uint32_t _lightIdToBitmask()
{
    uint32_t mask = 0;
    for (int index = 0; index < LIGHT_COUNT; index++)
    {
        int id = _lightIds[index];
        if (id >= 1 && id <= LIGHT_COUNT)
        {
            Serial.print("id "); Serial.println(id);
            mask = mask | (1 << BUTTON_BIT_LOOKUP[id]);
        }
    }
    return mask;
}

/* Turn on the indicated lights */
void _on()
{
    uint32_t mask = _lightIdToBitmask();
    Serial.print("_on ["); Serial.print(mask, BIN); Serial.println("]");
    sendInt(mask);
}

void _off()
{
    uint32_t mask = _lightIdToBitmask();
    SERIAL.print("_off ["); Serial.print(mask, BIN); Serial.println("]");
    sendInt(0);
}

/* Flash on->off the indicated lights x times with n interval in ms between flashes */
void _flash(int flashCount, int intervalMs) 
{
    flashCount = max(0, min(flashCount, MAX_FLASH_COUNT));
    intervalMs = max(0, min(intervalMs, MAX_FLASH_INTERVAL));
    SERIAL.print("_flash count["); Serial.print(flashCount); Serial.print("] interval["); Serial.print(intervalMs); Serial.println("]");
    for (int count = 0; count < flashCount; count++)
    {
        _on();
        _threadDelayMs(intervalMs);
        _off();
        _threadDelayMs(intervalMs);
    }
}

/**
 * Returns an int array of up to LIGHT_COUNT in size, with a list of all passed light
 * index values.  0 -> COUNT-1.  The command line takes in values 1-24, this outputs an array with each
 * entry 1-24. with 0 or -1 meaning nothing.
 * eg.  1 3 4 outputs 0 2 3 -1 -1 -1 -1 -1 -1 -1
 */
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
            _lightIds[index++] = v;
        }
    }
    
}

void _onCommand(cmd* c) 
{
    _getLightIds(0, c);
    _on();
}

void _offCommand(cmd* c)
{
    _getLightIds(0, c);
    _on();
}

void _flashCommand(cmd* c)
{
    Command cmd(c);
    int count = cmd.getArg(0).getValue().toInt();
    int interval = cmd.getArg(1).getValue().toInt();
    _getLightIds(2, c);
    _flash(count, interval);
}

void _bootSeqCommand(cmd* c)
{
    _off();
    for (int id = 1; id <= LIGHT_COUNT; id++)
    {
        _lightIds[id-1] = id;
        _on();
        _threadDelayMs(50);
    }
    delay(100);
    for (int id = 1; id <= LIGHT_COUNT; id++)
    {
        _lightIds[id-1] = 0;
        _on();
        _threadDelayMs(50);
    }

}


/*****************************************************************
 * Create a thread that non-stop reads the input states of the 
 * joystcick and buttons, checks the states for changes, and writes
 * back the report over the HID.
 *****************************************************************/
static void _threadProcessJoystick( void *pvParameters ) 
{

    Gamepad.begin();

    while(true)
    {
        Gamepad.press(1);
        Gamepad.write();
        _threadDelayMs(500);
        Gamepad.release(1);
        Gamepad.write();
        _threadDelayMs(500);
    }
  
    Gamepad.end();
    vTaskDelete( NULL );
    
}




/*****************************************************************
 * Create a thread that process the incomming serial commands,
 * then executes the request.
*****************************************************************/
static void _threadProcessSerial( void *pvParameters ) 
{


    SimpleCLI _cli;

    Command _cmdOn = _cli.addBoundlessCommand("on", _onCommand);
    Command _cmdOff = _cli.addBoundlessCommand("off", _offCommand);
    Command _cmdFlash = _cli.addBoundlessCommand("flash", _flashCommand);
    Command _cmdBootSeq = _cli.addBoundlessCommand("seq", _bootSeqCommand);

    while(true)
    {
        
        if (SERIAL.available()) 
        {
            /* Wait and Read the command */
            String input = SERIAL.readStringUntil('\n');

            /* Process the Command */
            SERIAL.print("#");
            SERIAL.println(input);
            _cli.parse(input);

            /* Output any errors */
            if (_cli.errored()) 
            {
                SERIAL.println(_cli.getError().toString());
                SERIAL.println("Help:\n> on <button index list>\n> off <button index list>\n> flash <count> <intervalMs> <button index list>\n - <button index list> = space separated list of button indexes 1-24\n");
            }
            
        }
        _threadDelayMs(2);
    }

    vTaskDelete( NULL );
}


//*****************************************************************
// Task will periodically print out useful information about the tasks running
// Is a useful tool to help figure out stack sizes being used
// Run time stats are generated from all task timing collected since startup
// No easy way yet to clear the run time stats yet
//*****************************************************************
static char ptrTaskList[400]; //temporary string buffer for task stats

void taskMonitor(void *pvParameters)
{
    int x;
    int measurement;
    
    SERIAL.println("Task Monitor: Started");

    // run this task afew times before exiting forever
    while(1)
    {
    	_threadDelayMs(10000); // print every 10 seconds

    	SERIAL.flush();
		SERIAL.println("");			 
    	SERIAL.println("****************************************************");
    	SERIAL.print("Free Heap: ");
    	SERIAL.print(xPortGetFreeHeapSize());
    	SERIAL.println(" bytes");

    	SERIAL.print("Min Heap: ");
    	SERIAL.print(xPortGetMinimumEverFreeHeapSize());
    	SERIAL.println(" bytes");
    	SERIAL.flush();

    	SERIAL.println("****************************************************");
    	SERIAL.println("Task            ABS             %Util");
    	SERIAL.println("****************************************************");

    	vTaskGetRunTimeStats(ptrTaskList); //save stats to char array
    	SERIAL.println(ptrTaskList); //prints out already formatted stats
    	SERIAL.flush();

		SERIAL.println("****************************************************");
		SERIAL.println("Task            State   Prio    Stack   Num     Core" );
		SERIAL.println("****************************************************");

		vTaskList(ptrTaskList); //save stats to char array
		SERIAL.println(ptrTaskList); //prints out already formatted stats
		SERIAL.flush();

		SERIAL.println("****************************************************");
		SERIAL.println("[Stacks Free Bytes Remaining] ");

		measurement = uxTaskGetStackHighWaterMark( handleTaskJoystick );
		SERIAL.print("Thread A: ");
		SERIAL.println(measurement);

		measurement = uxTaskGetStackHighWaterMark( handleTaskB );
		SERIAL.print("Thread B: ");
		SERIAL.println(measurement);

		measurement = uxTaskGetStackHighWaterMark( handleMonitorTask );
		SERIAL.print("Monitor Stack: ");
		SERIAL.println(measurement);

		SERIAL.println("****************************************************");
		SERIAL.flush();

    }

    // delete ourselves.
    // Have to call this or the system crashes when you reach the end bracket and then get scheduled.
    SERIAL.println("Task Monitor: Deleting");
    vTaskDelete( NULL );

}


/************************************************
 * Setup 
 ************************************************/
void setup() 
{
    pinMode(PIN_DATA, OUTPUT);
    pinMode(PIN_CLOCK, OUTPUT);
    pinMode(PIN_STROBE, OUTPUT);
    pinMode(PIN_ONBOARD_LED, OUTPUT);

    SERIAL.begin(9600);
    delay(1000);  // prevents usb driver crash on startup, do not omit this

    SERIAL.println("");
    SERIAL.println("******************************");
    SERIAL.println(PROGRAM_NAME);
    SERIAL.println(PROGRAM_VERSION);
    SERIAL.println("******************************");
    SERIAL.flush();

    vSetErrorLed(PIN_ONBOARD_LED, HIGH);
    vSetErrorSerial(&SERIAL);

    xTaskCreate(_threadProcessJoystick,     "Joystick Task",       256, NULL, tskIDLE_PRIORITY + 3, &handleTaskJoystick);
    xTaskCreate(_threadProcessSerial,     "Task B",       256, NULL, tskIDLE_PRIORITY + 2, &handleTaskB);
//    xTaskCreate(taskMonitor, "Task Monitor", 256, NULL, tskIDLE_PRIORITY + 1, &handleMonitorTask);

    // Start the RTOS, this function will never return and will schedule the tasks.
    vTaskStartScheduler();

    // error scheduler failed to start
    // should never get here
    while(1)
    {
        SERIAL.println("Scheduler Failed! \n");
        SERIAL.flush();
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
