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

#ifndef __ALGPIO_H__
#define __ALGPIO_H__

#include "main.h"
#include <Adafruit_MCP23017.h>
#include <Wire.h>


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

class ALGpio
{
    public:
        void process();
        bool getJoystick(JOYSTICK j);
        bool getButton(BUTTON b);
        int setLed(LED l, bool on);

    private:
        Adafruit_MCP23017 _mcpA;
        Adafruit_MCP23017 _mcpB;

        /* 
        * Input and Output BitMasks, made up of a pair of uin16_t that is sent and read from each of the 2 MCUs. 
        *    MCPB       MCPA
        * 31 .... 16 15 .... 0 
        */
        uint32_t _mcpOut = 0;
        uint32_t _mcpIn  = 0;


        void _setupInputPin(int pin, bool pullUp);
        void _setupOutputPin(int pin);
        void _gpioSetup();
  
};

#endif