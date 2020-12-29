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

#ifndef __MAIN_H__
#define __MAIN_H__

#include <FreeRTOS_SAMD21.h>

#define PROGRAM_NAME    "ArcadeIlluminate"
#define PROGRAM_VERSION "1.0"
#define PIN_ONBOARD_LED     13

typedef enum
{
    JOYSTICK_UP = 0,
    JOYSTICK_DOWN,
    JOYSTICK_LEFT,
    JOYSTICK_RIGHT
} JOYSTICK;

typedef enum
{
    BUTTON_B1 = 0,
    BUTTON_B2,
    BUTTON_B3,
    BUTTON_B4,
    BUTTON_B5,
    BUTTON_B6,
    BUTTON_B7,
    BUTTON_B8,
    BUTTON_B9,
    BUTTON_B10,
    BUTTON_B11,
    BUTTON_B12,
    BUTTON_B13,
    BUTTON_B14,
    BUTTON_B15,
    BUTTON_B16
} BUTTON;

typedef enum 
{
    LED_B1 = 0,
    LED_B2,
    LED_B3,
    LED_B4,
    LED_B5,
    LED_B6,
    LED_B7,
    LED_B8,
    LED_B9,
    LED_B10,
    LED_B11,
    LED_B12,
} LED;


void _threadDelayUs(int us);
void _threadDelayMs(int ms);
void _threadProcessGpio( void *pvParameters );
void _threadProcessJoystick( void *pvParameters );
void _threadProcessSerial( void *pvParameters ); 
void _threadProcessMonitor(void *pvParameters);

void gpioSetup();
bool gpioGetJoystick(JOYSTICK j);
bool gpioGetButton(BUTTON b);
void gpioSetLed(LED l, bool on);

#endif