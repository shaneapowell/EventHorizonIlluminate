'''
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
'''

#include <Arduino.h>
#include <Adafruit_DotStar.h>
#include <SimpleCLI.h>


/************************************************
 * on 1 4 6 12     <- turns on lights 1 4 6 12
 * off 6 9 18 22   <- turns off lights 6 9 18 22
 * flash 3 400 6 8 <- flashes lights 6 8 3x 400ms interval
 ***********************************************/

#define ONBOARD_LED  13
#define LIGHT_COUNT  24
#define MAX_FLASH_COUNT  10
#define MAX_FLASH_INTERVAL 1000
#define PIXEL_RED   0x550000
#define PIXEL_GREEN 0x005500
#define PIXEL_BLUE  0x000055
#define PIXEL_BLACK 0x000000

Adafruit_DotStar onboardStrip = Adafruit_DotStar(1, INTERNAL_DS_DATA, INTERNAL_DS_CLK, DOTSTAR_BGR);

SimpleCLI _cli;
Command _cmdFlash;
Command _cmdOn;
Command _cmdOff;

/* Turn on the indicated lights */
void _on(int lightIndex[LIGHT_COUNT])
{
    digitalWrite(ONBOARD_LED, 1);
}

/* Turn off the indicated lights */
void _off(int lightIndex[LIGHT_COUNT])
{
    digitalWrite(ONBOARD_LED, 0);
}

/* Flash on->off the indicated lights x times with n interval in ms between flashes */
void _flash(int flashCount, int intervalMs, int lightIndex[LIGHT_COUNT]) 
{
    flashCount = max(0, min(flashCount, MAX_FLASH_COUNT));
    intervalMs = max(0, min(intervalMs, MAX_FLASH_INTERVAL));
    Serial.print("_flash count["); Serial.print(flashCount); Serial.print("] interval["); Serial.print(intervalMs); Serial.println("]");
    for (int count = 0; count < flashCount; count++)
    {
        _on(lightIndex);
        delay(intervalMs);
        _off(lightIndex);
        delay(intervalMs);
    }
}

/**
 * Returns an int array of up to LIGHT_COUNT in size, with a list of all passed light
 * index values.  0 -> COUNT-1
 */
int* _getLightIndexes(int startOffset, cmd* c) 
{
    Command cmd(c);
    int index = 0;
    static int indexes[LIGHT_COUNT] = {-1};
    int argCount = cmd.countArgs();

    for (int argIndex = startOffset; argIndex < argCount; argIndex++)
    {
        Argument arg = cmd.getArg(argIndex);
        String argValue = arg.getValue();
        int v = argValue.toInt();
        if (v >= 0 && v < LIGHT_COUNT)
        {
            indexes[index++] = v;
        }
    }
    
    
    return indexes;
}

void _onCommand(cmd* c) 
{
    int indexes[LIGHT_COUNT] = {1,4};
    _on(indexes);
}

void _offCommand(cmd* c) 
{
    int indexes[LIGHT_COUNT] = {1,4};
    _off(indexes);
}

void _flashCommand(cmd* c)
{
    Command cmd(c);
    int count = cmd.getArg(0).getValue().toInt();
    int interval = cmd.getArg(1).getValue().toInt();
    int *indexes = _getLightIndexes(2, c);
    _flash(count, interval, indexes);

}

/* Setup */
void setup() 
{
    Serial.begin(9600);
    while(!Serial);

    _cmdOn = _cli.addBoundlessCommand("on", _onCommand);
    _cmdOff = _cli.addBoundlessCommand("off", _offCommand);
    _cmdFlash = _cli.addBoundlessCommand("flash", _flashCommand);

    digitalWrite(ONBOARD_LED, 1); delay(200);
    digitalWrite(ONBOARD_LED, 0); delay(200);
    digitalWrite(ONBOARD_LED, 1); delay(200);

    onboardStrip.begin();
    onboardStrip.show();

    Serial.println("Setup Complete");

}


/* Main Looper */
void loop() 
{
    /* Show, ready for input */
    onboardStrip.setPixelColor(0, PIXEL_GREEN); 
    onboardStrip.show();

    if (Serial.available()) 
    {
        /* Wait and Read the command */
        String input = Serial.readStringUntil('\n');

        /* Process the Command */
        onboardStrip.setPixelColor(0, PIXEL_BLUE); 
        onboardStrip.show();
        Serial.print("#");
        Serial.println(input);
        _cli.parse(input);

        if (_cli.errored()) 
        {
            Serial.println(_cli.getError().toString());
            Serial.println("Help:\n> on <button index list>\n> off <button index list>\n> flash <count> <intervalMs> <button index list>\n - <button index list> = space separated list of button indexes 0-23\n");
            for (int index = 0; index < 3; index++) 
            {
                onboardStrip.setPixelColor(0, PIXEL_BLUE); 
                onboardStrip.show();
                delay(200); 
                onboardStrip.setPixelColor(0, PIXEL_BLACK); 
                onboardStrip.show();
                delay(200); 
            }

        }
        
    }


}
