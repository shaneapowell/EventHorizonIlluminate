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
#include <Adafruit_MCP23017.h>
#include "ALGpio.h"

/************************************************
 * Wrapper Implementation around the ADAFruit MCP23017 library.
 ***********************************************/
class ALGpioPinSourceImpl: public ALGpioPinSource
{

    public:
        
        /**************************************************
         * 
         **************************************************/
        void begin()
        {
            Wire.setClock(1700000); 
            _mcpA.begin(0, &Wire);
            _mcpB.begin(1, &Wire);
        }

        /**************************************************
         * 
         **************************************************/
        void setupInputPin(int pin, bool pullUp)
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

        /**************************************************
         * 
         **************************************************/
        void setupOutputPin(int pin)
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


        /**************************************************
         * 
         **************************************************/
        void writeGPIO(uint32_t val)
        {
            _mcpA.writeGPIOAB(val);
            _mcpB.writeGPIOAB(val >> 16);
        }

        /**************************************************
         * 
         **************************************************/
        uint32_t readGPIO()
        {
            uint32_t mcpAIn = _mcpA.readGPIOAB();
            uint32_t mcpBIn = _mcpB.readGPIOAB();
            return mcpBIn << 16 | mcpAIn;
        }


    private:
        Adafruit_MCP23017 _mcpA;
        Adafruit_MCP23017 _mcpB;
};