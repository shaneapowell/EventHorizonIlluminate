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
    /* Active Low */
    if (b == BUTTON_B13 || b == BUTTON_B14 || b == BUTTON_B15 || b == BUTTON_B16)
    {
        return (_mcpIn & (1 << b)) == 0;
    }
    else /* Active High */
    {
        return (_mcpIn & (1 << b)) != 0;
    }
}

/*****************************************************************
 * Set the indicated LED to on or off.
 * LOW is on, HIGH if off.
 * Returns 1 if sucesfull, 0 if fail.
 *****************************************************************/
void ALGpio::setLed(LED l, bool on)
{
    if (l == LED_NULL)
    {
        return;
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
    
}

/*****************************************************************
 * Get the current indicated state of the LED
 *****************************************************************/
bool ALGpio::getLed(LED l)
{
    uint32_t mask = (1 << l);
    return (_mcpOut & mask) != 0;
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
    _pinSource->setupInputPin(PIN_INPUT_B1, false);
    _pinSource->setupInputPin(PIN_INPUT_B2, false);
    _pinSource->setupInputPin(PIN_INPUT_B3, false);
    _pinSource->setupInputPin(PIN_INPUT_B4, false);
    _pinSource->setupInputPin(PIN_INPUT_B5, false);
    _pinSource->setupInputPin(PIN_INPUT_B6, false);
    _pinSource->setupInputPin(PIN_INPUT_B7, false);
    _pinSource->setupInputPin(PIN_INPUT_B8, false);
    _pinSource->setupInputPin(PIN_INPUT_B9, false);
    _pinSource->setupInputPin(PIN_INPUT_B10, false);
    _pinSource->setupInputPin(PIN_INPUT_B11, false);
    _pinSource->setupInputPin(PIN_INPUT_B12, false);
    
    /* 2-pin Buttons */
    _pinSource->setupInputPin(PIN_INPUT_B13, true);
    _pinSource->setupInputPin(PIN_INPUT_B14, true);
    _pinSource->setupInputPin(PIN_INPUT_B15, true);
    _pinSource->setupInputPin(PIN_INPUT_B16, true);

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
#include <FreeRTOS_SAMD21.h>
void ALGpio::process()
{
    /* Write Outputs */
    _pinSource->writeGPIO(_mcpOut);

    /* Read Input */
    _mcpIn = _pinSource->readGPIO();

}


