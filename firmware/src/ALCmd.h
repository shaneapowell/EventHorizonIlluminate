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
#include <SimpleCLI.h>
#include "const.h"

#define CMD_SERIAL_BUFFER_SIZE 256

typedef void (*fptrDelayMs)(int);
typedef void (*fptrMonitorDump)();

class ALCmd
{
    public:
        ALCmd(fptrDelayMs fptrD, ALGpio* gpio, fptrMonitorDump fptrM);
        void process();

    
    private:
        fptrDelayMs _fptrDelayMs;
        ALGpio* _gpio;
        fptrMonitorDump _fptrMonitorDump;

        /* An array of LEDS identified with the <buttons> param. */
        LED _leds[LED_COUNT] = {LED_NULL};

        SimpleCLI _cli;
    
        Command _cmdOn = _cli.addBoundlessCommand("on");
        Command _cmdOff = _cli.addBoundlessCommand("off");
        Command _cmdFlash = _cli.addBoundlessCommand("flash");
        Command _cmdBootSeq = _cli.addCommand("seq");
        Command _cmdDump = _cli.addCommand("dump");

        char _serialBuffer[CMD_SERIAL_BUFFER_SIZE];   // an array to store the received data
        byte _serialBufferIndex = 0;
        char _serialEndMarker = '\n';
        char _rc;


        void _lightIdToLedArray(int startOffset, cmd* c) ;
        void _setOn(bool on);
        void _dumpIdArray();
        void _onCommand(cmd* c);
        void _offCommand(cmd* c);
        void _flashCommand(cmd* c);
        void _bootSeqCommand(cmd* c);
        void _dumpCommand(cmd* c);

};