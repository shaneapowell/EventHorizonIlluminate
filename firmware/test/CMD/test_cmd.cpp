

#include <Arduino.h>
#include <gmock/gmock.h>
#include <unity.h>
#include <ALGpio.h>
#include <ALCmd.h>

void mockDelayMs(int ms) { delay(ms);  }
void mockDump() { /* do nothing */ }

// Mock<ALGpio> mockGpio;
// Stream* mockStream = ArduinoFakeMock(Stream);
// Serial_* mockSerial = ArduinoFakeMock(Serial);
// ALCmd cmd = ALCmd(&mockSerial, "test", mockDelayMs, &mockGpio, mockDump);

class MockGpio: public ALGpio
{
    public:
        MockGpio(): ALGpio(0) {};
        MOCK_METHOD(void, begin,        () );
        MOCK_METHOD(void, process,      () );
        MOCK_METHOD(bool, getJoystick,  (JOYSTICK j) );
        MOCK_METHOD(bool, getButton,    (BUTTON b) );
        MOCK_METHOD(int,  setLed,       (LED l, bool on) );
};

class MockSerial: public Serial_
{
    public:
        MockSerial(): Serial_(NULL) {};
        MOCK_METHOD(int,    available,          ());
	    MOCK_METHOD(int,    availableForWrite,  ());
	    MOCK_METHOD(int,    peek,               ());
	    MOCK_METHOD(int,    read,               ());
	    MOCK_METHOD(void,   flush,              ());
	    MOCK_METHOD(size_t, write,              (uint8_t));
	    MOCK_METHOD(size_t, write,              (const uint8_t *buffer, size_t size));

};

MockSerial mockSerial;
MockGpio mockGpio;
ALCmd cmd = ALCmd("test", mockDelayMs, &mockSerial, &mockGpio, mockDump);

void setUp(void)
{
}

 void tearDown(void)
{
    ::testing::Mock::VerifyAndClearExpectations(&mockSerial);
    ::testing::Mock::VerifyAndClearExpectations(&mockGpio);
    TEST_ASSERT_TRUE_MESSAGE(!::testing::Test::HasFailure(), "Test Has Failures");
}

void test_cmd_flash(void)
{
//     /* Given */
//     When(Method(mockGpi, setLed)).AlwaysReturn();

//     /* When */
//     mockStream.println("on 1");
//     cmd.process();

//     /* Then */
//     Verify(Method(mockGpio, setLed)).Once();

}



void runTests()
{
    RUN_TEST(test_cmd_flash);

}
