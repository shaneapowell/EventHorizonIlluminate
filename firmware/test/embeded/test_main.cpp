#include <Arduino.h>
#include <unity.h>

/* Requred Prototypte */
void runHidTests();



void setup()
{
    delay(1000);
    UNITY_BEGIN();

    runHidTests();
    
    UNITY_END();
}

#include <Arduino.h>
void loop() 
{
    /* Indicate the tests have all completed with a non-stop double-flash of the onboard LED */
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
    delay(100);
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(100);
    digitalWrite(13, HIGH);
    delay(500);
}

