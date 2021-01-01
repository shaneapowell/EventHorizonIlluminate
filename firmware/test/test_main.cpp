#include <gmock/gmock.h>
#include <unity.h>

/* Requred Prototypte */
void runTests();


#ifdef ARDUINO

#include <Arduino.h>
void setup()
{
    ::testing::GTEST_FLAG(throw_on_failure) = false;
    ::testing::InitGoogleMock();

    delay(1000);

    UNITY_BEGIN();
    runTests();
    UNITY_END();
}

#include <Arduino.h>
void loop() 
{
    /* Indicate the tests have all completed */
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(100);
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(500);
}

/* Redirect stdout to Serial, so we can see the output of the gtests */
extern "C" 
{
    int _write(int fd, char *ptr, int len) 
    {
        (void) fd;
        return Serial.write(ptr, len);
    }
}


#else

int main(int argc, char **argv) 
{
    UNITY_BEGIN();
    runTests();
    UNITY_END();
    return 0;
}

#endif
    

