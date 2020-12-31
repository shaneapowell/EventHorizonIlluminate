

#include <unity.h>

//using namespace fakeit;

#include <ALGpio.h>
#include <ALHIDJoystick.h>

    // Mock<ALGpio> mockGpio;
    // Mock<GamepadAPI> mockGamepad;
    // ALHIDJoystick joystick(&mockGpio.get(), &mockGamepad.get);

    void setUp(void)
    {
    //     mockGpio.Reset();
    //     When(Method(mockGpio, begin)).AlwaysReturn();
    //     When(Method(mockGpio, process)).AlwaysReturn();
    }

    void tearDown(void)
    {

    }


    void testJoystick(void)
    {
    //     /* Given */
    //     When(Method(mockGpio, writeGPIO)).AlwaysReturn();
    //     When(Method(mockPinSource, readGPIO)).AlwaysReturn();

    //     /* When */
    //     mockGpi.begin();

    //     /* Then */
    //     Verify(Method(mockPinSource, begin));
    //     Verify(Method(mockPinSource, setupInputPin)).Exactly(20);
    //     Verify(Method(mockPinSource, setupOutputPin)).Exactly(12);
    //     Verify(Method(mockPinSource, writeGPIO)).Once();
    //     Verify(Method(mockPinSource, readGPIO)).Once();

        TEST_ASSERT_EQUAL(1, 1);
    }

    void setup()
    {
        delay(1000);
        UNITY_BEGIN();

        RUN_TEST(testJoystick);
    //     RUN_TEST(testGpioBegin);
    //     RUN_TEST(testGpioProcess);
    //     RUN_TEST(testGpioSetLed);
    //     RUN_TEST(testGpioJoystick);
    //     RUN_TEST(testGpioButtons);

        UNITY_END();
    }

    // void loop() {
    //     digitalWrite(13, HIGH);
    //     delay(100);
    //     digitalWrite(13, LOW);
    //     delay(100);
    //     digitalWrite(13, HIGH);
    //     delay(100);
    //     digitalWrite(13, LOW);
    //     delay(500);
    // }

