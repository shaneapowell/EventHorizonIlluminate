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

#include "ALGpio.h"

/**************************************************************
 *
 *************************************************************/ 
ALGpio::ALGpio(ALGpioPinSource* p)
{
    _pinSource = p;
}

/*****************************************************************
 * Get the state of the indicated control. 
 * Joystick inputs are OFF=HIGH ON=LOW
 ****************************************************************/
bool ALGpio::getJoystick(JOYSTICK j)
{
    return (_mcpIn & (1 << j)) == 0;
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
            bit = PIN_INPUT_B1;
            break;
        case BUTTON_B2: 
            bit = PIN_INPUT_B2;
            break;
        case BUTTON_B3: 
            bit = PIN_INPUT_B3;
            break;
        case BUTTON_B4: 
            bit = PIN_INPUT_B4;
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
            bit = PIN_INPUT_B13;
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
    if (l == LED_NULL)
    {
        return 0;
    }
 
    uint32_t mask = (1 << l);
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

/*****************************************************************
 * Create a thread that non-stop reads the input states of the 
 * joystcick and buttons, checks the states for changes, and writes
 * back the report over the HID.
 *****************************************************************/
void ALGpio::begin()
{

    /* Joystick. default HIGH, pulled low on switch */
    _pinSource->setupInputPin(PIN_INPUT_UP,    true);
    _pinSource->setupInputPin(PIN_INPUT_DOWN,  true);
    _pinSource->setupInputPin(PIN_INPUT_LEFT,  true);
    _pinSource->setupInputPin(PIN_INPUT_RIGHT, true);

    /* Primary 3-pin Buttons, default LOW, pulled high on press, external PullDowns wired */
    for (int index = 0; index < BUTTON_COUNT; index++)
    {
        _pinSource->setupInputPin(ALL_BUTTONS[index], false);
    }
    

    /* Primary 3-pin LEDS, default HIGH, pulled low for LED */
    for (int index = 0; index < LED_COUNT; index++)
    {
        _pinSource->setupOutputPin(ALL_LEDS[index]);
        setLed(ALL_LEDS[index], true);
    }
    
    /* Initial State */
    process();

}

/************************************************
 * Thread Process stage
 ************************************************/
void ALGpio::process()
{
    /* Write Outputs */
    _pinSource->writeGPIO(_mcpOut);

    /* Read Input */
    _mcpIn = _pinSource->readGPIO();

}


