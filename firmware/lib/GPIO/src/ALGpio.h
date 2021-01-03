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

/* 
    MCP23017 input for joystick and 12 LED buttons.
    ULN2003 output for LEDs hooked to dedicated MCU pins.
    the 4 2-pin inputs hooked directly to MCU pins.
*/

#pragma once

#include <stdint.h>

#define BUTTON_COUNT    16
#define LED_COUNT       12
#define JOYSTICK_AXIS_COUNT 4

/* MCP A = pins 0 - 15*/
#define PIN_INPUT_UP        0
#define PIN_INPUT_DOWN      1
#define PIN_INPUT_LEFT      2
#define PIN_INPUT_RIGHT     3
#define PIN_INPUT_B1        4
#define PIN_INPUT_B2        5
#define PIN_INPUT_B3        6
#define PIN_INPUT_B4        7
#define PIN_INPUT_B5        8
#define PIN_INPUT_B6        9
#define PIN_INPUT_B7        10
#define PIN_INPUT_B8        11
#define PIN_INPUT_B9        12
#define PIN_INPUT_B10       13
#define PIN_INPUT_B11       14
#define PIN_INPUT_B12       15
#define PIN_INPUT_B13       16
#define PIN_INPUT_B14       17
#define PIN_INPUT_B15       18
#define PIN_INPUT_B16       19

#define PIN_OUTPUT_B1       20
#define PIN_OUTPUT_B2       21
#define PIN_OUTPUT_B3       22
#define PIN_OUTPUT_B4       23
#define PIN_OUTPUT_B5       24
#define PIN_OUTPUT_B6       25
#define PIN_OUTPUT_B7       26
#define PIN_OUTPUT_B8       27
#define PIN_OUTPUT_B9       28
#define PIN_OUTPUT_B10      29
#define PIN_OUTPUT_B11      30
#define PIN_OUTPUT_B12      31

typedef enum
{
    JOYSTICK_UP     = PIN_INPUT_UP,
    JOYSTICK_DOWN   = PIN_INPUT_DOWN,
    JOYSTICK_LEFT   = PIN_INPUT_LEFT,
    JOYSTICK_RIGHT  = PIN_INPUT_RIGHT
} JOYSTICK;
const JOYSTICK ALL_JOYSTICK_AXIS[JOYSTICK_AXIS_COUNT] = {JOYSTICK_UP, JOYSTICK_DOWN, JOYSTICK_LEFT, JOYSTICK_RIGHT};

typedef enum
{
    BUTTON_B1 = PIN_INPUT_B1,
    BUTTON_B2 = PIN_INPUT_B2,
    BUTTON_B3 = PIN_INPUT_B3,
    BUTTON_B4 = PIN_INPUT_B4,
    BUTTON_B5 = PIN_INPUT_B5,
    BUTTON_B6 = PIN_INPUT_B6,
    BUTTON_B7 = PIN_INPUT_B7,
    BUTTON_B8 = PIN_INPUT_B8,
    BUTTON_B9 = PIN_INPUT_B9,
    BUTTON_B10 = PIN_INPUT_B10,
    BUTTON_B11 = PIN_INPUT_B11,
    BUTTON_B12 = PIN_INPUT_B12,
    BUTTON_B13 = PIN_INPUT_B13,
    BUTTON_B14 = PIN_INPUT_B14,
    BUTTON_B15 = PIN_INPUT_B15,
    BUTTON_B16 = PIN_INPUT_B16
} BUTTON;
const BUTTON ALL_BUTTONS[BUTTON_COUNT] = {BUTTON_B1,BUTTON_B2,BUTTON_B3,BUTTON_B4,BUTTON_B5,BUTTON_B6,BUTTON_B7,BUTTON_B8,BUTTON_B9,BUTTON_B10,BUTTON_B11,BUTTON_B12,BUTTON_B13,BUTTON_B14,BUTTON_B15,BUTTON_B16};

typedef enum 
{
    LED_NULL = -1,
    LED_B1 = PIN_OUTPUT_B1,
    LED_B2 = PIN_OUTPUT_B2,
    LED_B3 = PIN_OUTPUT_B3,
    LED_B4 = PIN_OUTPUT_B4,
    LED_B5 = PIN_OUTPUT_B5,
    LED_B6 = PIN_OUTPUT_B6,
    LED_B7 = PIN_OUTPUT_B7,
    LED_B8 = PIN_OUTPUT_B8,
    LED_B9 = PIN_OUTPUT_B9,
    LED_B10 = PIN_OUTPUT_B10,
    LED_B11 = PIN_OUTPUT_B11,
    LED_B12 = PIN_OUTPUT_B12
} LED;
const LED ALL_LEDS[LED_COUNT] = { LED_B1, LED_B2, LED_B3, LED_B4, LED_B5, LED_B6, LED_B7, LED_B8, LED_B9, LED_B10, LED_B11, LED_B12 };

class ALGpioPinSource
{
    public:
        virtual ~ALGpioPinSource() {}
        virtual void begin() = 0;
        virtual void setupInputPin(int pin, bool pullUp) = 0;
        virtual void setupOutputPin(int pin) = 0;
        virtual void writeGPIO(uint32_t val) = 0;
        virtual uint32_t readGPIO() = 0;
};


class ALGpio
{
    public:
        ALGpio(ALGpioPinSource* p);
        virtual void begin();
        virtual void process();
        virtual bool getJoystick(JOYSTICK j);
        virtual bool getButton(BUTTON b);
        virtual int setLed(LED l, bool on);

    private:
        ALGpioPinSource* _pinSource;

        /* 
        * Input and Output BitMasks for all known buttons and inputs.
        * Each bit represents the state of a hardware pin.
        */
        uint32_t _mcpOut = 0;
        uint32_t _mcpIn  = 0;


//        void _setupInputPin(int pin, bool pullUp);
//        void _setupOutputPin(int pin);
  
};

