

#include <ArduinoFake.h>
#include <unity.h>

using namespace fakeit;

#include <ALGpio.h>

Mock<ALGpioPinSource> mockPinSource;
ALGpio gpio = ALGpio(&mockPinSource.get());

void setUp(void)
{
    mockPinSource.Reset();
    When(Method(mockPinSource, begin)).AlwaysReturn();
    When(Method(mockPinSource, setupInputPin)).AlwaysReturn();
    When(Method(mockPinSource, setupOutputPin)).AlwaysReturn();
//    When(Method(mockPinSource, writeGPIO)).AlwaysDo([](uint32_t val){ outVal = val; });
}

void tearDown(void)
{

}

void testGpioEnums(void)
{
    TEST_ASSERT_EQUAL(12, LED_COUNT);
    TEST_ASSERT_EQUAL(16, BUTTON_COUNT);

    /* Ensure all values in ALL_BUTTONS are unique */


    /* Ensure all values in ALL_LEDS are unique */

}

void testGpioBegin(void)
{
    /* Given */
    When(Method(mockPinSource, writeGPIO)).AlwaysReturn();
    When(Method(mockPinSource, readGPIO)).AlwaysReturn();

    /* When */
    gpio.begin();

    /* Then */
    Verify(Method(mockPinSource, begin));
    Verify(Method(mockPinSource, setupInputPin)).Exactly(20);
    Verify(Method(mockPinSource, setupOutputPin)).Exactly(12);
    Verify(Method(mockPinSource, writeGPIO)).Once();
    Verify(Method(mockPinSource, readGPIO)).Once();

}

void testGpioProcess()
{
    /* Given */
    When(Method(mockPinSource, writeGPIO)).AlwaysReturn();
    When(Method(mockPinSource, readGPIO)).AlwaysReturn();

    /* When */
    gpio.process();

    /* Then write and read */
    Verify(Method(mockPinSource, writeGPIO) + Method(mockPinSource, readGPIO)).Once();

}

/*
 * Leds are set ON by turning off the bit, and OFF by turning on the bit.
 */
void testGpioSetLed(void)
{
    /* Given */
    static uint32_t outVal;
    When(Method(mockPinSource, writeGPIO)).AlwaysDo([](uint32_t val){ outVal = val; });
    When(Method(mockPinSource, readGPIO)).AlwaysReturn();
    gpio.process();
    uint32_t startingOutVal = outVal;
    

    /* When */
    gpio.setLed(LED_B1, false);
    gpio.process();

    /* Then */
    Verify(Method(mockPinSource, writeGPIO)).Exactly(2);
    Verify(Method(mockPinSource, readGPIO)).Exactly(2);
    TEST_ASSERT_NOT_EQUAL(startingOutVal, outVal);

    /* Also When */
    gpio.setLed(LED_B1, true);
    gpio.process();

    /* Also Then */
    Verify(Method(mockPinSource, writeGPIO)).Exactly(3);
    Verify(Method(mockPinSource, readGPIO)).Exactly(3);
    TEST_ASSERT_EQUAL(startingOutVal, outVal);

}

/*
 * Joysticks are pulled LOW when trigered, HIGH when released.
 */
void testGpioJoystick()
{
    /* Given */
    When(Method(mockPinSource, writeGPIO)).AlwaysReturn();
    When(Method(mockPinSource, readGPIO)).AlwaysReturn(0xFFFFFFFF);
    
    /* When */
    gpio.process();
    bool up = gpio.getJoystick(JOYSTICK_UP);
    bool down = gpio.getJoystick(JOYSTICK_DOWN);
    bool left = gpio.getJoystick(JOYSTICK_LEFT);
    bool right = gpio.getJoystick(JOYSTICK_RIGHT);

    /* Then */
    Verify(Method(mockPinSource, writeGPIO)).Once();
    Verify(Method(mockPinSource, readGPIO)).Once();
    TEST_ASSERT_EQUAL(false, up);
    TEST_ASSERT_EQUAL(false, down);
    TEST_ASSERT_EQUAL(false, left);
    TEST_ASSERT_EQUAL(false, right);

    /* Also Given */
    When(Method(mockPinSource, readGPIO)).AlwaysReturn(0);

    /* Also When */
    gpio.process();
    up = gpio.getJoystick(JOYSTICK_UP);
    down = gpio.getJoystick(JOYSTICK_DOWN);
    left = gpio.getJoystick(JOYSTICK_LEFT);
    right = gpio.getJoystick(JOYSTICK_RIGHT);

    /* Also Then */
    Verify(Method(mockPinSource, writeGPIO)).Twice();
    Verify(Method(mockPinSource, readGPIO)).Twice();
    TEST_ASSERT_EQUAL(true, up);
    TEST_ASSERT_EQUAL(true, down);
    TEST_ASSERT_EQUAL(true, left);
    TEST_ASSERT_EQUAL(true, right);
}


/*
 * Buttons are pulled HIGH when trigered, LOW when released.
 */
void testGpioButtons()
{
    /* Given */
    When(Method(mockPinSource, writeGPIO)).AlwaysReturn();
    When(Method(mockPinSource, readGPIO)).AlwaysReturn(0xFFFFFFFF);
    
    /* When */
    gpio.process();
    bool b1 = gpio.getButton(BUTTON_B1);
    
    /* Then */
    Verify(Method(mockPinSource, writeGPIO)).Once();
    Verify(Method(mockPinSource, readGPIO)).Once();
    TEST_ASSERT_EQUAL(true, b1);

    /* Also Given */
    When(Method(mockPinSource, readGPIO)).AlwaysReturn(0);

    /* Also When */
    gpio.process();
    b1 = gpio.getButton(BUTTON_B1);

    /* Also Then */
    Verify(Method(mockPinSource, writeGPIO)).Twice();
    Verify(Method(mockPinSource, readGPIO)).Twice();
    TEST_ASSERT_EQUAL(false, b1);
    
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(testGpioEnums);
    RUN_TEST(testGpioBegin);
    RUN_TEST(testGpioProcess);
    RUN_TEST(testGpioSetLed);
    RUN_TEST(testGpioJoystick);
    RUN_TEST(testGpioButtons);

    UNITY_END();
}