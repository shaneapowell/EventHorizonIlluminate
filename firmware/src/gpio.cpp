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


/* MCP A = pins 0 - 15*/
#define MCP_PIN_INPUT_UP        0
#define MCP_PIN_INPUT_DOWN      1
#define MCP_PIN_INPUT_LEFT      2
#define MCP_PIN_INPUT_RIGHT     3
#define MCP_PIN_INPUT_B1        4
#define MCP_PIN_INPUT_B2        5
#define MCP_PIN_INPUT_B3        6
#define MCP_PIN_INPUT_B4        7
#define MCP_PIN_OUTPUT_B1       8
#define MCP_PIN_OUTPUT_B2       9
//#define MCP_PIN_INPUT_B7        10
//#define MCP_PIN_INPUT_B8        11
//#define MCP_PIN_INPUT_B9        12
//#define MCP_PIN_INPUT_B10       13
//#define MCP_PIN_INPUT_B11       14
//#define MCP_PIN_INPUT_B12       15

/* MCP B = pins 16-31 */
#define MCP_PIN_INPUT_B13       16
#define MCP_PIN_OUTPUT_B8       24
#define MCP_PIN_OUTPUT_B9       25


Adafruit_MCP23017 _mcpA;
Adafruit_MCP23017 _mcpB;

/* 
 * Input and Output BitMasks, made up of a pair of uin16_t that is sent and read from each of the 2 MCUs. 
 *    MCPB       MCPA
 * 31 .... 16 15 .... 0 
*/
uint32_t _mcpOut = 0x0;
uint32_t _mcpIn = 0;


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
            bit = MCP_PIN_INPUT_UP;
            break;
        case JOYSTICK_DOWN:  
            bit = MCP_PIN_INPUT_DOWN;
            break;
        case JOYSTICK_LEFT:  
            bit = MCP_PIN_INPUT_LEFT;
            break;
        case JOYSTICK_RIGHT: 
            bit = MCP_PIN_INPUT_RIGHT;
            break;
        default:
            return false;
    }

    return (_mcpIn & (1 << bit)) == 0;

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
            bit = MCP_PIN_INPUT_B1;
            break;
        case BUTTON_B2: 
            bit = MCP_PIN_INPUT_B2;
            break;
        case BUTTON_B3: 
            bit = MCP_PIN_INPUT_B3;
            break;
        case BUTTON_B4: 
            bit = MCP_PIN_INPUT_B4;
            break;
        // case BUTTON_B5: 
        //     bit = MCP_PIN_INPUT_B5;
        //     break;
        // case BUTTON_B6: 
        //     bit = MCP_PIN_INPUT_B6;
        //     break;
        // case BUTTON_B7: 
        //     bit = MCP_PIN_INPUT_B7;
        //     break;
        // case BUTTON_B8: 
        //     bit = MCP_PIN_INPUT_B8;
        //     break;
        // case BUTTON_B9: 
        //     bit = MCP_PIN_INPUT_B9;
        //     break;
        // case BUTTON_B10: 
        //     bit = MCP_PIN_INPUT_B10;
        //     break;
        // case BUTTON_B11: 
        //     bit = MCP_PIN_INPUT_B11;
        //     break;
        // case BUTTON_B12: 
        //     bit = MCP_PIN_INPUT_B12;
        //     break;
        case BUTTON_B13: 
            bit = MCP_PIN_INPUT_B13;
            break;

        default:
            return false;

    }

    return (_mcpIn & (1 << bit)) > 0;

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
            bit = MCP_PIN_OUTPUT_B1;
            break;
        case LED_B2:
            bit = MCP_PIN_OUTPUT_B2;
            break;
        // case LED_B3:
        //     bitA = MCP_PIN_OUTPUT_B3;
        //     break;
        // case LED_B4:
        //     bitA = MCP_PIN_OUTPUT_B4;
        //     break;

        case LED_B8:
            bit = MCP_PIN_OUTPUT_B8;
            break;

        case LED_B9:
            bit = MCP_PIN_OUTPUT_B9;
            break;

    }


    if (bit > -1) 
    {
        uint32_t mask = (1 << bit);
        if (on) 
        {
            mask = ~mask;
            _mcpOut &= mask;
        }
        else
        {
            _mcpOut |= mask;
        }
        
    }

}

/***********************************************************
 *
 **********************************************************/
static void _setupInputPin(int pin, bool pullUp)
{
    if (pin <= 15)
    {
        _mcpA.pinMode(pin, INPUT);
        if (pullUp)
        {
            _mcpA.pullUp(pin, HIGH);
        }
    }
    else
    {
       pin -= 16;
       _mcpB.pinMode(pin, INPUT);
        if (pullUp)
        {
            _mcpB.pullUp(pin, HIGH);
        }
    }
    
}

/***********************************************************
 *
 **********************************************************/
static void _setupOutputPin(int pin)
{
    if (pin <= 15)
    {
        _mcpA.pinMode(pin, OUTPUT);
    }
    else
    {
       pin -= 16;
       _mcpB.pinMode(pin, OUTPUT);
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
    _mcpB.begin(1, &Wire);

    /* Joystick. default HIGH, pulled low on switch */
    _setupInputPin(MCP_PIN_INPUT_UP,    true);
    _setupInputPin(MCP_PIN_INPUT_DOWN,  true);
    _setupInputPin(MCP_PIN_INPUT_LEFT,  true);
    _setupInputPin(MCP_PIN_INPUT_RIGHT, true);

    /* Primary 3-pin Buttons, default LOW, pulled high on press, external PullDowns wired */
    _setupInputPin(MCP_PIN_INPUT_B1, false);
    _setupInputPin(MCP_PIN_INPUT_B2, false);
    _setupInputPin(MCP_PIN_INPUT_B3, false);
    _setupInputPin(MCP_PIN_INPUT_B4, false);


    /* Primary 3-pin LEDS, default HIGH, pulled low for LED */
    _setupOutputPin(MCP_PIN_OUTPUT_B1);
    _setupOutputPin(MCP_PIN_OUTPUT_B2);
//    _mcpA.pinMode(MCPA_PIN_OUTPUT_B3, OUTPUT);
//    _mcpA.pinMode(MCPA_PIN_OUTPUT_B4, OUTPUT);

    _setupOutputPin(MCP_PIN_OUTPUT_B8);
    _setupOutputPin(MCP_PIN_OUTPUT_B9);


}

/************************************************
 * Thread 
 ************************************************/
void _threadProcessGpio( void *pvParameters )
{
    _gpioSetup();

    while(true)
    {
        /* Write Outputs */
        _mcpA.writeGPIOAB(_mcpOut);
        _mcpB.writeGPIOAB(_mcpOut >> 16);

        /* Read Input */
        uint32_t mcpAIn = _mcpA.readGPIOAB();
        uint32_t mcpBIn = _mcpB.readGPIOAB();
        _mcpIn = mcpBIn << 16 | mcpAIn;
        taskYIELD();
        
    }
  
    vTaskDelete( NULL );

}