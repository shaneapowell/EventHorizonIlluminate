#include <gmock/gmock.h>
#include <gtest/gtest.h>
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


TEST(GPIO, all_enums)
{
    ASSERT_EQ(12, LED_COUNT);
    ASSERT_EQ(16, BUTTON_COUNT);

    /* Ensure all values in ALL_BUTTONS are unique */


    /* Ensure all values in ALL_LEDS are unique */

}

TEST(GPIO, gpio_begin)
{
    /* Given */
    MockGpioPinSource mockPinSource;
    ALGpio gpio = ALGpio(&mockPinSource);
    EXPECT_CALL(mockPinSource, setupInputPin(_, _)).Times(20);
    EXPECT_CALL(mockPinSource, begin()).Times(0);
    EXPECT_CALL(mockPinSource, setupOutputPin(_)).Times(12);
    EXPECT_CALL(mockPinSource, writeGPIO(_)).Times(1);
    EXPECT_CALL(mockPinSource, readGPIO()).Times(1);

    /* When */
    gpio.begin();

    /* Then */

}

TEST(GPIO, process)
{
    /* Given */
    MockGpioPinSource mockPinSource;
    ALGpio gpio = ALGpio(&mockPinSource);
    Expectation write = EXPECT_CALL(mockPinSource, writeGPIO(_)).Times(1);
    EXPECT_CALL(mockPinSource, readGPIO()).Times(1).After(write);
    

    /* When */
    gpio.process();

}

/*
 * Leds are set ON by turning off the bit, and OFF by turning on the bit.
 */
TEST(GPIO, setLed)
{
    /* Given */
    MockGpioPinSource mockPinSource;
    ALGpio gpio = ALGpio(&mockPinSource);
    uint32_t outVal = 0;
    EXPECT_CALL(mockPinSource, writeGPIO(_)).Times(3).WillRepeatedly(SaveArg<0>(&outVal));
    EXPECT_CALL(mockPinSource, readGPIO()).Times(3).WillRepeatedly(Return(0));

    gpio.process();
    uint32_t startingOutVal = outVal;
    

    /* When */
    gpio.setLed(LED_B1, false);
    gpio.process();

    /* Then */
    ASSERT_NE(startingOutVal, outVal);

    /* Also When */
    gpio.setLed(LED_B1, true);
    gpio.process();

    /* Also Then */
    ASSERT_EQ(startingOutVal, outVal);

}

 /*
  * Leds are set ON by turning off the bit, and OFF by turning on the bit.
  */
TEST(GPIO, getLed)
{
    /* Given */
    MockGpioPinSource mockPinSource;
    ALGpio gpio = ALGpio(&mockPinSource);
    uint32_t outVal = 0;
    EXPECT_CALL(mockPinSource, writeGPIO(_)).Times(3).WillRepeatedly(Return());
    EXPECT_CALL(mockPinSource, readGPIO()).Times(3).WillRepeatedly(Return(outVal));


    /* When */
    bool isOn = gpio.getLed(LED_B1);
    gpio.process();

    /* Then */
    ASSERT_TRUE(isOn);

}

/*
 * Joysticks are pulled LOW when trigered, HIGH when released.
 */
TEST(GPIO, joystick)
{
    /* Given */
    MockGpioPinSource mockPinSource;
    ALGpio gpio = ALGpio(&mockPinSource);
    EXPECT_CALL(mockPinSource, writeGPIO(_)).WillRepeatedly(Return());
    EXPECT_CALL(mockPinSource, readGPIO())
        .WillOnce(Return(0xffffffff))
        .WillOnce(Return(0));
    
    /* When */
    gpio.process();

    /* Then */
    ASSERT_FALSE(gpio.getJoystick(JOYSTICK_UP));
    ASSERT_FALSE(gpio.getJoystick(JOYSTICK_DOWN));
    ASSERT_FALSE(gpio.getJoystick(JOYSTICK_LEFT));
    ASSERT_FALSE(gpio.getJoystick(JOYSTICK_RIGHT));

    /* Also When */
    gpio.process();

    /* Also Then */
    ASSERT_TRUE(gpio.getJoystick(JOYSTICK_UP));
    ASSERT_TRUE(gpio.getJoystick(JOYSTICK_DOWN));
    ASSERT_TRUE(gpio.getJoystick(JOYSTICK_LEFT));
    ASSERT_TRUE(gpio.getJoystick(JOYSTICK_RIGHT));
}


/*
 * Buttons are pulled HIGH when trigered, LOW when released.
 */
TEST(GPIO, buttons)
{
    /* Given */
    MockGpioPinSource mockPinSource;
    ALGpio gpio = ALGpio(&mockPinSource);
    EXPECT_CALL(mockPinSource, writeGPIO(_))
        .WillOnce(Return())
        .WillOnce(Return());
    EXPECT_CALL(mockPinSource, readGPIO())
        .WillOnce(Return(0xffffffff))
        .WillOnce(Return(0));
    
    /* When */
    gpio.process();
    
    /* Then */
    ASSERT_TRUE(gpio.getButton(BUTTON_B1));

    /* Also When */
    gpio.process();

    /* Also Then */
    ASSERT_FALSE(gpio.getButton(BUTTON_B1));
    
}

