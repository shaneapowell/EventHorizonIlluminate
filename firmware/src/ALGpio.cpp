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
#include "ALGpio.h"
#include <FreeRTOS_SAMD21.h>
#include <Adafruit_MCP23017.h>
#include <Wire.h>


/*****************************************************************
 * Get the state of the indicated control. 
 * Joystick inputs are OFF=HIGH ON=LOW
 ****************************************************************/
bool ALGpio::getJoystick(JOYSTICK j)
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
bool ALGpio::getButton(BUTTON b)
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
 * LOW is on, HIGH if off.
 * Returns 1 if sucesfull, 0 if fail.
 *****************************************************************/
int ALGpio::setLed(LED l, bool on)
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
        
        return 1;
    }

    return 0;
}

/***********************************************************
 *
 **********************************************************/
void ALGpio::_setupInputPin(int pin, bool pullUp)
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
void ALGpio::_setupOutputPin(int pin)
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
void ALGpio::_gpioSetup()
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
void ALGpio::process()
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