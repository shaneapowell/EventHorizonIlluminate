
#include <unity.h>


void setUp(void)
{

}

void tearDown(void)
{

}

void testGpioGetJoystick(void)
{
    TEST_ASSERT_EQUAL(1, 1);
}

void testGpioSetLed(void)
{

}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(testGpioGetJoystick);
    RUN_TEST(testGpioSetLed);

    UNITY_END();
}