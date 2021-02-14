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
#include <ALLed.h>

#define BUTTON_COUNT            16
#define LED_COUNT               12
#define JOYSTICK_AXIS_COUNT     4

/* 10 minutes of no input, put leds to sleep */
#define MILLIS_NO_INPUT_SLEEP  (1000 * 60 * 10)

#define MCPA_PIN_GPA0            0
#define MCPA_PIN_GPA1            1
#define MCPA_PIN_GPA2            2
#define MCPA_PIN_GPA3            3
#define MCPA_PIN_GPA4            4
#define MCPA_PIN_GPA5            5
#define MCPA_PIN_GPA6            6
#define MCPA_PIN_GPA7            7
#define MCPA_PIN_GPB0            8
#define MCPA_PIN_GPB1            9
#define MCPA_PIN_GPB2            10
#define MCPA_PIN_GPB3            11
#define MCPA_PIN_GPB4            12
#define MCPA_PIN_GPB5            13
#define MCPA_PIN_GPB6            14
#define MCPA_PIN_GPB7            15

#define MCPB_PIN_GPA0            16
#define MCPB_PIN_GPA1            17
#define MCPB_PIN_GPA2            18
#define MCPB_PIN_GPA3            19
#define MCPB_PIN_GPA4            20
#define MCPB_PIN_GPA5            21
#define MCPB_PIN_GPA6            22
#define MCPB_PIN_GPA7            23
#define MCPB_PIN_GPB0            24
#define MCPB_PIN_GPB1            25
#define MCPB_PIN_GPB2            26
#define MCPB_PIN_GPB3            27
#define MCPB_PIN_GPB4            28
#define MCPB_PIN_GPB5            29
#define MCPB_PIN_GPB6            30
#define MCPB_PIN_GPB7            31

/* MCP A = pins 0 - 15*/
#define PIN_INPUT_B1        MCPA_PIN_GPA0
#define PIN_INPUT_B2        MCPA_PIN_GPA2
#define PIN_INPUT_B3        MCPA_PIN_GPA4
#define PIN_INPUT_B4        MCPA_PIN_GPA6
#define PIN_INPUT_B5        MCPA_PIN_GPB1
#define PIN_INPUT_B6        MCPA_PIN_GPB2
#define PIN_INPUT_B7        MCPA_PIN_GPB4
#define PIN_INPUT_B8        MCPA_PIN_GPB6
#define PIN_INPUT_B9        MCPB_PIN_GPA0
#define PIN_INPUT_B10       MCPB_PIN_GPA2
#define PIN_INPUT_B11       MCPB_PIN_GPA4
#define PIN_INPUT_B12       MCPB_PIN_GPA6
#define PIN_INPUT_B13       MCPB_PIN_GPB7
#define PIN_INPUT_B14       MCPB_PIN_GPB6
#define PIN_INPUT_B15       MCPB_PIN_GPB5
#define PIN_INPUT_B16       MCPB_PIN_GPB4

#define PIN_INPUT_UP        MCPB_PIN_GPB0
#define PIN_INPUT_DOWN      MCPB_PIN_GPB3
#define PIN_INPUT_LEFT      MCPB_PIN_GPB1
#define PIN_INPUT_RIGHT     MCPB_PIN_GPB2

#define PIN_OUTPUT_B1       MCPA_PIN_GPA1
#define PIN_OUTPUT_B2       MCPA_PIN_GPA3
#define PIN_OUTPUT_B3       MCPA_PIN_GPA5
#define PIN_OUTPUT_B4       MCPA_PIN_GPA7
#define PIN_OUTPUT_B5       MCPA_PIN_GPB0
#define PIN_OUTPUT_B6       MCPA_PIN_GPB3
#define PIN_OUTPUT_B7       MCPA_PIN_GPB5
#define PIN_OUTPUT_B8       MCPA_PIN_GPB7
#define PIN_OUTPUT_B9       MCPB_PIN_GPA1
#define PIN_OUTPUT_B10      MCPB_PIN_GPA3
#define PIN_OUTPUT_B11      MCPB_PIN_GPA5
#define PIN_OUTPUT_B12      MCPB_PIN_GPA7


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
        virtual void process(int msSinceLastProcess, ALLed *led);
        virtual bool getJoystick(JOYSTICK j);
        virtual bool getButton(BUTTON b);
        virtual void setLed(LED l, bool on);
        virtual bool getLed(LED l);

    private:
        ALGpioPinSource* _pinSource;

        /* 
        * Input and Output BitMasks for all known buttons and inputs.
        * Each bit represents the state of a hardware pin.
        */
        uint32_t _mcpOut = 0;
        uint32_t _mcpIn  = 0;

        /* Tracks the last time an input was read, so we can sleep the LEDs */
        uint32_t _millisSinceLastInput = 0;
        
        void _processAsAwake(int msSinceLastProcess);
        void _processAsAsleep(int msSinceLastProcess);
};

