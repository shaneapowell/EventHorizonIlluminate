

#include <Arduino.h>
#include <unity.h>

// using namespace fakeit;

// #include <ALGpio.h>
// #include <ALCmd.h>


// void mockDelayMs(int ms) { delay(ms);  }
// void mockDump() { /* do nothing */ }

// Mock<ALGpio> mockGpio;
// Stream* mockStream = ArduinoFakeMock(Stream);
// Serial_* mockSerial = ArduinoFakeMock(Serial);
// ALCmd cmd = ALCmd(&mockSerial, "test", mockDelayMs, &mockGpio, mockDump);

// void setUp(void)
// {
//     mockGpio.Reset();
//     When(Method(mockGpio, begin)).AlwaysReturn();
//     When(Method(mockGpio, process)).AlwaysReturn();

// }

// void tearDown(void)
// {

// }

// void testCmdFlash(void)
// {
//     /* Given */
//     When(Method(mockGpi, setLed)).AlwaysReturn();

//     /* When */
//     mockStream.println("on 1");
//     cmd.process();

//     /* Then */
//     Verify(Method(mockGpio, setLed)).Once();

// }


void setup()
{
    delay(1000);
    UNITY_BEGIN();

//     RUN_TEST(testGpioEnums);
//     RUN_TEST(testGpioBegin);
//     RUN_TEST(testGpioProcess);
//     RUN_TEST(testGpioSetLed);
//     RUN_TEST(testGpioJoystick);
//     RUN_TEST(testGpioButtons);

    UNITY_END();
}
