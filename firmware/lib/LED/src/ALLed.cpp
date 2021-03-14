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

#include <ALLed.h>


/******************************************************************
 * Constructor
 ******************************************************************/ 
ALLed::ALLed()
{ 

}

/******************************************************************
 * 
 *****************************************************************/
void ALLed::begin()
{
    _strip.begin();
    _strip.show();
}


/*****************************************************************
 * Create a thread that process the incomming serial commands,
 * then executes the request.
*****************************************************************/
void ALLed::process() 
{
    /* Shw the current staate color, then set to ready, next time 
        into this method will show green. Be sure the threads
        only flash for a short tiem, but long enough to see */
    _strip.setPixelColor(0, _state);
    _strip.show();
    if (_state != LED_STATE_SLEEP)
    {
        showState(LED_STATE_READY);
    }
}


/*****************************************************************
 * 
 ****************************************************************/
void ALLed::showState(LED_STATE state) 
{
    _state = state;
}
    
