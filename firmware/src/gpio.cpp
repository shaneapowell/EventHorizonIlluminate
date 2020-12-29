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

#include <Adafruit_MCP23017.h>
#include <Wire.h>

/* 
    MCP23017 input for joystick and 12 LED buttons.
    ULN2003 output for LEDs hooked to dedicated MCU pins.
    the 4 2-pin inputs hooked directly to MCU pins.
*/

#define MCPA_PIN_INPUT_UP        0
#define MCPA_PIN_INPUT_DOWN      1
#define MCPA_PIN_INPUT_LEFT      2
#define MCPA_PIN_INPUT_RIGHT     3
#define MCPA_PIN_INPUT_B1        4
#define MCPA_PIN_INPUT_B2        5
#define MCPA_PIN_INPUT_B3        6
#define MCPA_PIN_INPUT_B4        7
#define MCPA_PIN_INPUT_B5        8
#define MCPA_PIN_INPUT_B6        9
#define MCPA_PIN_INPUT_B7        10
#define MCPA_PIN_INPUT_B8        11
#define MCPA_PIN_INPUT_B9        12
#define MCPA_PIN_INPUT_B10       13
#define MCPA_PIN_INPUT_B11       14
#define MCPA_PIN_INPUT_B12       15

// #define MCPA_PIN_INPUT_B13_PIN           20
// #define MCPA_PIN_INPUT_B14_PIN           21
// #define MCPA_PIN_INPUT_B15_PIN           22
// #define MCPA_PIN_INPUT_B16_PIN           23

#define MCPA_PIN_OUTPUT_B1           8
#define MCPA_PIN_OUTPUT_B2           9
#define MCPA_PIN_OUTPUT_B3           10
#define MCPA_PIN_OUTPUT_B4           11
// #define OUTPUT_B3_PIN           24
// #define OUTPUT_B4_PIN           24
// #define OUTPUT_B5_PIN           24
// #define OUTPUT_B6_PIN           24
// #define OUTPUT_B7_PIN           24
// #define OUTPUT_B8_PIN           24
// #define OUTPUT_B9_PIN           24
// #define OUTPUT_B10_PIN          24
// #define OUTPUT_B11_PIN          24
// #define OUTPUT_B12_PIN          24


Adafruit_MCP23017 _mcpA;
//Adafruit_MCP23017 _mcpB;

uint16_t _mcpAOut = 0xFF;
uint16_t _mcpBOut = 0xFF;
uint16_t _mcpAIn = 0;
uint16_t _mcpBIn = 0;

/*****************************************************************
 * Get the state of the indicated control. 
 * Joystick inputs are OFF=HIGH ON=LOW
 ****************************************************************/
bool gpioGetJoystick(JOYSTICK j)
{
    int bit = 0;
    switch (j)
    {
        case JOYSTICK_UP:    
            bit = MCPA_PIN_INPUT_UP;
            break;
        case JOYSTICK_DOWN:  
            bit = MCPA_PIN_INPUT_DOWN;
            break;
        case JOYSTICK_LEFT:  
            bit = MCPA_PIN_INPUT_LEFT;
            break;
        case JOYSTICK_RIGHT: 
            bit = MCPA_PIN_INPUT_RIGHT;
            break;
    }

    return (_mcpAIn & (1 << bit)) == 0;

}

/*****************************************************************
 * Get the state of the indicated control.  ON=HIGH OFF=LOW
 ****************************************************************/
bool gpioGetButton(BUTTON b)
{
    int bit = 0;
    switch (b)
    {
        case BUTTON_B1: 
            bit = MCPA_PIN_INPUT_B1;
            break;
        case BUTTON_B2: 
            bit = MCPA_PIN_INPUT_B2;
            break;
        case BUTTON_B3: 
            bit = MCPA_PIN_INPUT_B3;
            break;
        case BUTTON_B4: 
            bit = MCPA_PIN_INPUT_B4;
            break;
        case BUTTON_B5: 
            bit = MCPA_PIN_INPUT_B5;
            break;
        case BUTTON_B6: 
            bit = MCPA_PIN_INPUT_B6;
            break;
        case BUTTON_B7: 
            bit = MCPA_PIN_INPUT_B7;
            break;
        case BUTTON_B8: 
            bit = MCPA_PIN_INPUT_B8;
            break;
        case BUTTON_B9: 
            bit = MCPA_PIN_INPUT_B9;
            break;
        case BUTTON_B10: 
            bit = MCPA_PIN_INPUT_B10;
            break;
        case BUTTON_B11: 
            bit = MCPA_PIN_INPUT_B11;
            break;
        case BUTTON_B12: 
            bit = MCPA_PIN_INPUT_B12;
            break;
        // case BUTTON_B13: return !_mcp.digitalRead(MCPA_PIN_INPUT_B13);
        // case BUTTON_B14: return !_mcp.digitalRead(MCPA_PIN_INPUT_B14);
        // case BUTTON_B15: return !_mcp.digitalRead(MCPA_PIN_INPUT_B15);
        // case BUTTON_B16: return !_mcp.digitalRead(MCPA_PIN_INPUT_B16);

    }

    return (_mcpAIn & (1 << bit)) > 0;

}

/*****************************************************************
 * Set the indicated LED to on or off.
 * LOW is on, HIGH if off
 *****************************************************************/
void gpioSetLed(LED l, bool on)
{
    int bit = -1;
    switch(l) 
    {
        case LED_B1:
            bit = MCPA_PIN_OUTPUT_B1;
            break;
        case LED_B2:
            bit = MCPA_PIN_OUTPUT_B2;
            break;
        case LED_B3:
            bit = MCPA_PIN_OUTPUT_B3;
            break;
        case LED_B4:
            bit = MCPA_PIN_OUTPUT_B4;
            break;
    }

    if (bit > -1) 
    {
        int mask = (1 << bit);
        if (on) 
        {
            mask = ~mask;
            _mcpAOut &= mask;
        }
        else
        {
            _mcpAOut |= mask;
        }
        
    }

}

/*****************************************************************
 * Create a thread that non-stop reads the input states of the 
 * joystcick and buttons, checks the states for changes, and writes
 * back the report over the HID.
 *****************************************************************/
static void _gpioSetup()
{

    Wire.setClock(1700000); 
    _mcpA.begin(0, &Wire);
    //_mcpB.begin(0, &Wire);

    /* Joystick. default HIGH, pulled low on switch */
    _mcpA.pinMode(MCPA_PIN_INPUT_UP,    INPUT);
    _mcpA.pinMode(MCPA_PIN_INPUT_DOWN,  INPUT);
    _mcpA.pinMode(MCPA_PIN_INPUT_LEFT,  INPUT);
    _mcpA.pinMode(MCPA_PIN_INPUT_RIGHT, INPUT);
    _mcpA.pullUp(MCPA_PIN_INPUT_UP,    HIGH);
    _mcpA.pullUp(MCPA_PIN_INPUT_DOWN,  HIGH);
    _mcpA.pullUp(MCPA_PIN_INPUT_LEFT,  HIGH);
    _mcpA.pullUp(MCPA_PIN_INPUT_RIGHT, HIGH);

    /* Primary 3-pin LED Buttons, default LOW, pulled high on press */
    _mcpA.pinMode(MCPA_PIN_INPUT_B1, INPUT);
    _mcpA.pinMode(MCPA_PIN_INPUT_B2, INPUT);
    _mcpA.pinMode(MCPA_PIN_INPUT_B3, INPUT);
    _mcpA.pinMode(MCPA_PIN_INPUT_B4, INPUT);
    // _mcp.pinMode(MCPA_PIN_INPUT_B5, INPUT);
    // _mcp.pinMode(MCPA_PIN_INPUT_B6, INPUT);
    // _mcp.pinMode(MCPA_PIN_INPUT_B7, INPUT);
    // _mcp.pinMode(MCPA_PIN_INPUT_B8, INPUT);
    // _mcp.pinMode(MCPA_PIN_INPUT_B9, INPUT);
    // _mcp.pinMode(MCPA_PIN_INPUT_B10, INPUT);
    // _mcp.pinMode(MCPA_PIN_INPUT_B11, INPUT);
    // _mcp.pinMode(MCPA_PIN_INPUT_B12, INPUT);
    // // _mcp.pinMode(MCPA_PIN_INPUT_B13, INPUT);
    // _mcp.pinMode(MCPA_PIN_INPUT_B14, INPUT);
    // _mcp.pinMode(MCPA_PIN_INPUT_B15, INPUT);
    // _mcp.pinMode(MCPA_PIN_INPUT_B16, INPUT);

    /* Primary 3-pin LED Buttons, default HIGH, pulled low for LED */
    _mcpA.pinMode(MCPA_PIN_OUTPUT_B1, OUTPUT);
    _mcpA.pinMode(MCPA_PIN_OUTPUT_B2, OUTPUT);
    _mcpA.pinMode(MCPA_PIN_OUTPUT_B3, OUTPUT);
    _mcpA.pinMode(MCPA_PIN_OUTPUT_B4, OUTPUT);

    // _mcp.pullUp(MCPA_PIN_INPUT_B5, HIGH);
    // _mcp.pullUp(MCPA_PIN_INPUT_B6, HIGH);
    // _mcp.pullUp(MCPA_PIN_INPUT_B7, HIGH);
    // _mcp.pullUp(MCPA_PIN_INPUT_B8, HIGH);
    // _mcp.pullUp(MCPA_PIN_INPUT_B9, HIGH);
    // _mcp.pullUp(MCPA_PIN_INPUT_B10, HIGH);
    // _mcp.pullUp(MCPA_PIN_INPUT_B11, HIGH);
    // _mcp.pullUp(MCPA_PIN_INPUT_B12, HIGH);

    // pinMode(MCPA_PIN_INPUT_B13_PIN, INPUT_PULLUP);
    // pinMode(MCPA_PIN_INPUT_B14_PIN, INPUT_PULLUP);
    // pinMode(MCPA_PIN_INPUT_B15_PIN, INPUT_PULLUP);
    // pinMode(MCPA_PIN_INPUT_B16_PIN, INPUT_PULLUP);
 

}

void _threadProcessGpio( void *pvParameters )
{
    _gpioSetup();

    while(true)
    {
        /* Write Outputs */
        _mcpA.writeGPIOAB(_mcpAOut);
        //_mcpB.writeGPIOAB(_mcpBOut);

        /* Read Input */
        _mcpAIn = _mcpA.readGPIOAB();
        
        taskYIELD();
        
    }
  
    vTaskDelete( NULL );

}