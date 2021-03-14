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

#include <Adafruit_DotStar.h>


#define COLOR_GREEN     0x006a00
#define COLOR_BLUE      0x00006a
#define COLOR_RED       0x6a0000
#define COLOR_YELLOW    0x6a6a00
#define COLOR_DIM       0x000100

#define NUMPIXELS 1 
#define DATAPIN   7
#define CLOCKPIN   8

typedef enum LED_STATE
{
    LED_STATE_READY     = COLOR_GREEN,
    LED_STATE_COMMAND   = COLOR_BLUE,
    LED_STATE_ERROR     = COLOR_RED,
    LED_STATE_JOYSTICK  = COLOR_YELLOW,
    LED_STATE_SLEEP     = COLOR_DIM
} LED_STATE;

class ALLed
{
    public:
        ALLed();
        void begin();
        void process();
        void showState(LED_STATE state);
    
    
    private:

        LED_STATE _state = LED_STATE_READY;
        Adafruit_DotStar _strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);
  
       

};