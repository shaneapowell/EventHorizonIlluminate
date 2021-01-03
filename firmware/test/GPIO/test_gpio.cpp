#include <gmock/gmock.h>
#include <unity.h>
#include <stdio.h>

#include <ALGpio.h>

using ::testing::Return;
using ::testing::_;
using ::testing::SaveArg;
using ::testing::Mock;
using ::testing::Expectation;


class MockGpioPinSource: public ALGpioPinSource
{
    public:
        MOCK_METHOD(void,     begin,         (),                     (override) );
        MOCK_METHOD(void,     setupInputPin, (int pin, bool pullUp), (override) );
        MOCK_METHOD(void,     setupOutputPin,(int pin),              (override) );
        MOCK_METHOD(void,     writeGPIO,     (uint32_t val),         (override) );
        MOCK_METHOD(uint32_t, readGPIO,      (),                     (override) );
};

MockGpioPinSource mockPinSource;
ALGpio gpio = ALGpio(&mockPinSource);

void setUp(void)
{
}

void tearDown(void)
{
    ::testing::Mock::VerifyAndClearExpectations(&mockPinSource);
    TEST_ASSERT_TRUE_MESSAGE(!::testing::Test::HasFailure(), "Test Has Failures");
}

void test_gpio_enums(void)
{
    TEST_ASSERT_EQUAL(12, LED_COUNT);
    TEST_ASSERT_EQUAL(16, BUTTON_COUNT);

    /* Ensure all values in ALL_BUTTONS are unique */


    /* Ensure all values in ALL_LEDS are unique */

}

void test_gpio_begin(void)
{
    /* Given */
    EXPECT_CALL(mockPinSource, setupInputPin(_, _)).Times(20);
    EXPECT_CALL(mockPinSource, begin()).Times(0);
    EXPECT_CALL(mockPinSource, setupOutputPin(_)).Times(12);
    EXPECT_CALL(mockPinSource, writeGPIO(_)).Times(1);
    EXPECT_CALL(mockPinSource, readGPIO()).Times(1);

    /* When */
    gpio.begin();

    /* Then */

}

void test_gpio_process()
{
    /* Given */
    Expectation write = EXPECT_CALL(mockPinSource, writeGPIO(_)).Times(1);
    EXPECT_CALL(mockPinSource, readGPIO()).Times(1).After(write);
    

    /* When */
    gpio.process();

}

/*
 * Leds are set ON by turning off the bit, and OFF by turning on the bit.
 */
void test_gpio_setLed(void)
{
    /* Given */
    uint32_t outVal = 0;
    EXPECT_CALL(mockPinSource, writeGPIO(_)).Times(3).WillRepeatedly(SaveArg<0>(&outVal));
    EXPECT_CALL(mockPinSource, readGPIO()).Times(3).WillRepeatedly(Return(0));

    gpio.process();
    uint32_t startingOutVal = outVal;
    

    /* When */
    gpio.setLed(LED_B1, false);
    gpio.process();

    /* Then */
    TEST_ASSERT_NOT_EQUAL(startingOutVal, outVal);

    /* Also When */
    gpio.setLed(LED_B1, true);
    gpio.process();

    /* Also Then */
    TEST_ASSERT_EQUAL(startingOutVal, outVal);

}

/*
 * Joysticks are pulled LOW when trigered, HIGH when released.
 */
void test_gpio_joystick()
{
    /* Given */
    EXPECT_CALL(mockPinSource, writeGPIO(_)).WillRepeatedly(Return());
    EXPECT_CALL(mockPinSource, readGPIO())
        .WillOnce(Return(0xffffffff))
        .WillOnce(Return(0));
    
    /* When */
    gpio.process();

    /* Then */
    TEST_ASSERT_EQUAL(false, gpio.getJoystick(JOYSTICK_UP));
    TEST_ASSERT_EQUAL(false, gpio.getJoystick(JOYSTICK_DOWN));
    TEST_ASSERT_EQUAL(false, gpio.getJoystick(JOYSTICK_LEFT));
    TEST_ASSERT_EQUAL(false, gpio.getJoystick(JOYSTICK_RIGHT));

    /* Also When */
    gpio.process();

    /* Also Then */
    TEST_ASSERT_EQUAL(true, gpio.getJoystick(JOYSTICK_UP));
    TEST_ASSERT_EQUAL(true, gpio.getJoystick(JOYSTICK_DOWN));
    TEST_ASSERT_EQUAL(true, gpio.getJoystick(JOYSTICK_LEFT));
    TEST_ASSERT_EQUAL(true, gpio.getJoystick(JOYSTICK_RIGHT));
}


/*
 * Buttons are pulled HIGH when trigered, LOW when released.
 */
void test_gpio_buttons()
{
    /* Given */
    EXPECT_CALL(mockPinSource, writeGPIO(_))
        .WillOnce(Return())
        .WillOnce(Return());
    EXPECT_CALL(mockPinSource, readGPIO())
        .WillOnce(Return(0xffffffff))
        .WillOnce(Return(0));
    
    /* When */
    gpio.process();
    
    /* Then */
    TEST_ASSERT_EQUAL(true, gpio.getButton(BUTTON_B1));

    /* Also When */
    gpio.process();

    /* Also Then */
    TEST_ASSERT_EQUAL(false , gpio.getButton(BUTTON_B1));
    
}


void runTests()
{
    RUN_TEST(test_gpio_enums);
    RUN_TEST(test_gpio_begin);
    RUN_TEST(test_gpio_process);
    RUN_TEST(test_gpio_setLed);
    RUN_TEST(test_gpio_joystick);
    RUN_TEST(test_gpio_buttons);
}
