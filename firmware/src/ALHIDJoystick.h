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

#ifndef __ALHIDJOYSTICK_H__
#define __ALHIDJOYSTICK_H__

class ALHIDJoystick
{

    public:
        ALHIDJoystick(ALGpio* gpio);
        ~ALHIDJoystick();
        void process();

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
        CONTROLS_STATE _prevState;
        CONTROLS_STATE _currState;

        bool _isCurrStateDifferent();
        void _saveCurrStateToPrevState();
        void _readCurrentState();
        void _sendCurrentState();

};


#endif