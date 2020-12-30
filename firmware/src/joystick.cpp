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
#include <HID-Project.h>
#include <Adafruit_MCP23017.h>
#include <Wire.h>


typedef struct
{
    bool up;
    bool down;
    bool left;
    bool right;
    bool btn1;
    bool btn2;
    bool btn3;
    bool btn4;
    bool btn5;
    bool btn6;
    bool btn7;
    bool btn8;
    bool btn9;
    bool btn10;
    bool btn11;
    bool btn12;
    bool btn13;
    bool btn14;
    bool btn15;
    bool btn16;
}  CONTROLS_STATE;


CONTROLS_STATE _prevState;
CONTROLS_STATE _currState;

/****************************************************************
 * Compare the previous steate to the current state, if they
 * are in any way different, return true.
 ****************************************************************/
bool _isCurrStateDifferent()
{
    return memcmp(&_prevState, &_currState, sizeof(CONTROLS_STATE));
}

/****************************************************************
 * A simple mem-copy.
 ****************************************************************/
void _saveCurrStateToPrevState()
{
    memcpy(&_prevState, &_currState, sizeof(CONTROLS_STATE));
    memset(&_currState, 0, sizeof(CONTROLS_STATE));
}

/****************************************************************
 * Read the inputs into the current state struct
 ****************************************************************/
void _readCurrentState()
{
    memset(&_currState, 0, sizeof(CONTROLS_STATE));

    _currState.up    = gpioGetJoystick(JOYSTICK_UP);
    _currState.down  = gpioGetJoystick(JOYSTICK_DOWN);
    _currState.left  = gpioGetJoystick(JOYSTICK_LEFT);
    _currState.right = gpioGetJoystick(JOYSTICK_RIGHT);

    _currState.btn1  = gpioGetButton(BUTTON_B1);
    _currState.btn2  = gpioGetButton(BUTTON_B2);
    _currState.btn3  = gpioGetButton(BUTTON_B3);
    _currState.btn4  = gpioGetButton(BUTTON_B4);
    _currState.btn5  = gpioGetButton(BUTTON_B5);
    _currState.btn6  = gpioGetButton(BUTTON_B6);
    _currState.btn7  = gpioGetButton(BUTTON_B7);
    _currState.btn8  = gpioGetButton(BUTTON_B8);
    _currState.btn9  = gpioGetButton(BUTTON_B9);
    _currState.btn10  = gpioGetButton(BUTTON_B10);
    _currState.btn11  = gpioGetButton(BUTTON_B11);
    _currState.btn12  = gpioGetButton(BUTTON_B12);
    _currState.btn13  = gpioGetButton(BUTTON_B13);
    _currState.btn14  = gpioGetButton(BUTTON_B14);
    _currState.btn15  = gpioGetButton(BUTTON_B15);
    _currState.btn16  = gpioGetButton(BUTTON_B16);

}

/****************************************************************
 * Send the current state of the controlls
 ****************************************************************/
void _sendCurrentState()
{
    Gamepad.releaseAll();

    /* XY axes not sending any values for some reason. sticking with the gampad for now */
    // if (_currState.up)     Gamepad.yAxis(-0xffff);
    // if (_currState.down)   Gamepad.yAxis(0xffff);
    // if (_currState.left)   Gamepad.xAxis(-0xffff);
    // if (_currState.right)  Gamepad.xAxis(0xffff);
    if (_currState.up)     Gamepad.dPad1(GAMEPAD_DPAD_UP);
    if (_currState.down)   Gamepad.dPad1(GAMEPAD_DPAD_DOWN);
    if (_currState.left)   Gamepad.dPad1(GAMEPAD_DPAD_LEFT);
    if (_currState.right)  Gamepad.dPad1(GAMEPAD_DPAD_RIGHT);
    if (_currState.up   && _currState.left)   Gamepad.dPad1(GAMEPAD_DPAD_UP_LEFT);
    if (_currState.up   && _currState.right)  Gamepad.dPad1(GAMEPAD_DPAD_UP_RIGHT);
    if (_currState.down && _currState.left)   Gamepad.dPad1(GAMEPAD_DPAD_DOWN_LEFT);
    if (_currState.down && _currState.right)  Gamepad.dPad1(GAMEPAD_DPAD_DOWN_RIGHT);

    if (_currState.btn1)   Gamepad.press(1);
    if (_currState.btn2)   Gamepad.press(2);
    if (_currState.btn3)   Gamepad.press(3);
    if (_currState.btn4)   Gamepad.press(4);
    if (_currState.btn5)   Gamepad.press(5);
    if (_currState.btn6)   Gamepad.press(6);
    if (_currState.btn7)   Gamepad.press(7);
    if (_currState.btn8)   Gamepad.press(8);
    if (_currState.btn9)   Gamepad.press(9);
    if (_currState.btn10)  Gamepad.press(10);
    if (_currState.btn11)  Gamepad.press(11);
    if (_currState.btn12)  Gamepad.press(12);
    if (_currState.btn13)  Gamepad.press(13);
    if (_currState.btn14)  Gamepad.press(14);
    if (_currState.btn15)  Gamepad.press(15);
    if (_currState.btn16)  Gamepad.press(16);

    Gamepad.write();
}

/*****************************************************************
 * Create a thread that non-stop reads the input states of the 
 * joystcick and buttons, checks the states for changes, and writes
 * back the report over the HID.
 *****************************************************************/
void _threadProcessJoystick( void *pvParameters ) 
{

    Gamepad.begin();

    while(true)
    {
        _readCurrentState();
        if (_isCurrStateDifferent())
        {
            Serial.println("Sending...");
            _sendCurrentState();
            _saveCurrStateToPrevState();
        }
        
        taskYIELD();
    }
  
    Gamepad.end();
    vTaskDelete( NULL );
    
}


