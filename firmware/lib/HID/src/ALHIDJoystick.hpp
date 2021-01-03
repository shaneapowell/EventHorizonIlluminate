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

#pragma once

#include "ALGpio.h"

/* These are the master/source Classes to the GamepadAPI_T template. Hidden away to allow for native unit tests */
//#include <HID-APIs/GamepadAPI.h>
//#include <HID-Project.h>

/* Copy and Pasted directly from HID-APIs/GamepadAPI.h. We need these defines, but it breaks our native unit tests
to include GamepadAPI.h here.  I don't like this, but I love unit tests.. so.. lesser of 2 evils IMO.
How about an embedded unit test to ensure they always match */
#define AL_GAMEPAD_DPAD_CENTERED 0
#define AL_GAMEPAD_DPAD_UP 1
#define AL_GAMEPAD_DPAD_UP_RIGHT 2
#define AL_GAMEPAD_DPAD_RIGHT 3
#define AL_GAMEPAD_DPAD_DOWN_RIGHT 4
#define AL_GAMEPAD_DPAD_DOWN 5
#define AL_GAMEPAD_DPAD_DOWN_LEFT 6
#define AL_GAMEPAD_DPAD_LEFT 7
#define AL_GAMEPAD_DPAD_UP_LEFT 8


template <class GamepadAPI_T>
class ALHIDJoystick
{

    public:
        
        /***************************************************************
         * Constructor
         ***************************************************************/ 
        ALHIDJoystick(ALGpio* gpio, GamepadAPI_T* gamepad) 
        { 
            _gpio = gpio ;
            _gamepad = gamepad;
        }


        /***************************************************************
         * Destructor
         ***************************************************************/ 
        ~ALHIDJoystick()
        {
            _gamepad->end();
        }

        
        /***************************************************************
         * 
         ***************************************************************/ 
        void begin(void)
        {
            _gamepad->begin();
            _readCurrentState();
        }


        /*****************************************************************
         * A sinle process stage. To be repeated by it's thread.
         *****************************************************************/
        void process()
        {

            _readCurrentState();
            if (_isCurrStateDifferent())
            {
                _sendCurrentState();
                _saveCurrStateToPrevState();
            }
            
        }



    private:
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

        ALGpio *_gpio;
        GamepadAPI_T *_gamepad;
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

            _currState.up    = _gpio->getJoystick(JOYSTICK_UP);
            _currState.down  = _gpio->getJoystick(JOYSTICK_DOWN);
            _currState.left  = _gpio->getJoystick(JOYSTICK_LEFT);
            _currState.right = _gpio->getJoystick(JOYSTICK_RIGHT);

            _currState.btn1  = _gpio->getButton(BUTTON_B1);
            _currState.btn2  = _gpio->getButton(BUTTON_B2);
            _currState.btn3  = _gpio->getButton(BUTTON_B3);
            _currState.btn4  = _gpio->getButton(BUTTON_B4);
            _currState.btn5  = _gpio->getButton(BUTTON_B5);
            _currState.btn6  = _gpio->getButton(BUTTON_B6);
            _currState.btn7  = _gpio->getButton(BUTTON_B7);
            _currState.btn8  = _gpio->getButton(BUTTON_B8);
            _currState.btn9  = _gpio->getButton(BUTTON_B9);
            _currState.btn10  = _gpio->getButton(BUTTON_B10);
            _currState.btn11  = _gpio->getButton(BUTTON_B11);
            _currState.btn12  = _gpio->getButton(BUTTON_B12);
            _currState.btn13  = _gpio->getButton(BUTTON_B13);
            _currState.btn14  = _gpio->getButton(BUTTON_B14);
            _currState.btn15  = _gpio->getButton(BUTTON_B15);
            _currState.btn16  = _gpio->getButton(BUTTON_B16);

        }


        /****************************************************************
         * Send the current state of the controlls
         ****************************************************************/
        void _sendCurrentState()
        {
            _gamepad->releaseAll();

            /* XY axes not sending any values for some reason. sticking with the gampad for now */
            // if (_currState.up)     Gamepad.yAxis(-0xffff);
            // if (_currState.down)   Gamepad.yAxis(0xffff);
            // if (_currState.left)   Gamepad.xAxis(-0xffff);
            // if (_currState.right)  Gamepad.xAxis(0xffff);
            if (_currState.up)     _gamepad->dPad1(AL_GAMEPAD_DPAD_UP);
            if (_currState.down)   _gamepad->dPad1(AL_GAMEPAD_DPAD_DOWN);
            if (_currState.left)   _gamepad->dPad1(AL_GAMEPAD_DPAD_LEFT);
            if (_currState.right)  _gamepad->dPad1(AL_GAMEPAD_DPAD_RIGHT);
            if (_currState.up   && _currState.left)   _gamepad->dPad1(AL_GAMEPAD_DPAD_UP_LEFT);
            if (_currState.up   && _currState.right)  _gamepad->dPad1(AL_GAMEPAD_DPAD_UP_RIGHT);
            if (_currState.down && _currState.left)   _gamepad->dPad1(AL_GAMEPAD_DPAD_DOWN_LEFT);
            if (_currState.down && _currState.right)  _gamepad->dPad1(AL_GAMEPAD_DPAD_DOWN_RIGHT);

            if (_currState.btn1)   _gamepad->press(1);
            if (_currState.btn2)   _gamepad->press(2);
            if (_currState.btn3)   _gamepad->press(3);
            if (_currState.btn4)   _gamepad->press(4);
            if (_currState.btn5)   _gamepad->press(5);
            if (_currState.btn6)   _gamepad->press(6);
            if (_currState.btn7)   _gamepad->press(7);
            if (_currState.btn8)   _gamepad->press(8);
            if (_currState.btn9)   _gamepad->press(9);
            if (_currState.btn10)  _gamepad->press(10);
            if (_currState.btn11)  _gamepad->press(11);
            if (_currState.btn12)  _gamepad->press(12);
            if (_currState.btn13)  _gamepad->press(13);
            if (_currState.btn14)  _gamepad->press(14);
            if (_currState.btn15)  _gamepad->press(15);
            if (_currState.btn16)  _gamepad->press(16);

            _gamepad->write();
        }


};

