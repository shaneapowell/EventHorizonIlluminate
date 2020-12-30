

#include <unity.h>
#include "../fakeit.hpp"

using namespace fakeit;

#include <ALGpio.h>

Mock<ALGpioPinSource> mockPinSource;
ALGpio gpio = ALGpio(&mockPinSource.get());
uint32_t writeVal;
uint32_t readVal;

void setUp(void)
{
    writeVal = 0;
    readVal = 0;
    mockPinSource.Reset();
    When(Method(mockPinSource, writeGPIO)).AlwaysDo([](uint32_t val){ writeVal = val; });
    When(Method(mockPinSource, readGPIO)).AlwaysReturn(readVal);
    TEST_ASSERT_EQUAL(0, writeVal);
    TEST_ASSERT_EQUAL(0, readVal);
}

void tearDown(void)
{

}

void testGpioEnums(void)
{
    TEST_ASSERT_EQUAL(12, LED_COUNT);
    TEST_ASSERT_EQUAL(16, BUTTON_COUNT);
}

void testGpioBegin(void)
{
    /* Given */
    When(Method(mockPinSource, begin)).AlwaysReturn();
    When(Method(mockPinSource, setupInputPin)).AlwaysReturn();
    When(Method(mockPinSource, setupOutputPin)).AlwaysReturn();

    /* When */
    gpio.begin();

    /* Then */
    Verify(Method(mockPinSource, begin));
    Verify(Method(mockPinSource, setupInputPin)).Exactly(20);
    Verify(Method(mockPinSource, setupOutputPin)).Exactly(12);
    TEST_ASSERT_EQUAL(1, 1);

}

void testGpioProcess()
{

    /* When */
    gpio.process();

    /* Then write and read */
    Verify(Method(mockPinSource, writeGPIO) + Method(mockPinSource, readGPIO)).Once();

}

void testGpioSetLed(void)
{
    /* Given */
    TEST_ASSERT_EQUAL(0, writeVal);

    /* When */
    gpio.setLed(LED_B1, false);
    gpio.process();

    /* Then */
    Verify(Method(mockPinSource, writeGPIO)).Once();
    TEST_ASSERT_NOT_EQUAL(0, writeVal);

}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(testGpioEnums);
    RUN_TEST(testGpioBegin);
    RUN_TEST(testGpioProcess);
    RUN_TEST(testGpioSetLed);

    UNITY_END();
}