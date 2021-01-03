
#include <gmock/gmock.h>
#include <unity.h>

using ::testing::Return;
using ::testing::_;
using ::testing::SaveArg;
using ::testing::Mock;
using ::testing::Expectation;

#include <ALGpio.h>
#include <ALHIDJoystick.h>

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

class MockGamepad: public GamepadAPI
{
    public:
        MOCK_METHOD(void, SendReport,   (void* data, int length), (override) ) ;
};

MockGpio mockGpio;
MockGamepad mockGamepad;
ALHIDJoystick joystick = ALHIDJoystick(&mockGpio, &mockGamepad);

void setUp(void)
{
}

void tearDown(void)
{
    ::testing::Mock::VerifyAndClearExpectations(&mockGamepad);
    ::testing::Mock::VerifyAndClearExpectations(&mockGpio);
    TEST_ASSERT_TRUE_MESSAGE(!::testing::Test::HasFailure(), "Test Has Failures");
}

void test_joystick_begin_sends_initial_report(void)
{
    /* Given */
    EXPECT_CALL(mockGamepad, SendReport(_,_)).Times(1).WillRepeatedly(Return());
    EXPECT_CALL(mockGpio, getJoystick(_)).Times(JOYSTICK_AXIS_COUNT).WillRepeatedly(Return(false));
    EXPECT_CALL(mockGpio, getButton(_)).Times(BUTTON_COUNT).WillRepeatedly(Return(false));

    /* When */
    joystick.begin();


}

void test_joystick_every_control_checked(void)
{

    /* Given */
    EXPECT_CALL(mockGamepad, SendReport).Times(1).WillRepeatedly(Return());
    EXPECT_CALL(mockGpio, getButton(BUTTON_B1)).WillOnce(Return(true));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B2)).WillOnce(Return(true));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B3)).WillOnce(Return(true));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B4)).WillOnce(Return(true));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B5)).WillOnce(Return(true));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B6)).WillOnce(Return(true));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B7)).WillOnce(Return(true));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B8)).WillOnce(Return(true));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B9)).WillOnce(Return(true));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B10)).WillOnce(Return(true));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B11)).WillOnce(Return(true));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B12)).WillOnce(Return(true));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B13)).WillOnce(Return(true));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B14)).WillOnce(Return(true));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B15)).WillOnce(Return(true));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B16)).WillOnce(Return(true));
    EXPECT_CALL(mockGpio, getJoystick(JOYSTICK_UP))   .WillOnce(Return(true));
    EXPECT_CALL(mockGpio, getJoystick(JOYSTICK_DOWN)) .WillOnce(Return(true));
    EXPECT_CALL(mockGpio, getJoystick(JOYSTICK_LEFT)) .WillOnce(Return(true));
    EXPECT_CALL(mockGpio, getJoystick(JOYSTICK_RIGHT)).WillOnce(Return(true));

    /* When */
    joystick.process();

}

void test_joystick_no_changes_dont_send_updates(void)
{
    /* Given */
    EXPECT_CALL(mockGamepad, SendReport(_,_)).Times(1).WillRepeatedly(Return());
    EXPECT_CALL(mockGpio, getJoystick(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(mockGpio, getButton(_)).WillRepeatedly(Return(false));

    /* When 1st proces is called, SendReport is called. 2nd (and more) call SendReport is not called */
    joystick.process();
    joystick.process();
    joystick.process();
    joystick.process();
    joystick.process();


}


void test_joystick_axis_change_sends_one_update(void)
{
    /* Given */
    EXPECT_CALL(mockGamepad, SendReport(_,_)).Times(2).WillRepeatedly(Return());
    EXPECT_CALL(mockGpio, getButton(_)).WillRepeatedly(Return(false));
    EXPECT_CALL(mockGpio, getJoystick(JOYSTICK_UP))   .WillOnce(Return(true)).WillOnce(Return(false)).WillOnce(Return(false));
    EXPECT_CALL(mockGpio, getJoystick(JOYSTICK_DOWN)) .WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(mockGpio, getJoystick(JOYSTICK_LEFT)) .WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true));
    EXPECT_CALL(mockGpio, getJoystick(JOYSTICK_RIGHT)).WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true));

    /* When */
    joystick.begin();
    joystick.process();
    joystick.process();
    
}

void runTests()
{
    RUN_TEST(test_joystick_begin_sends_initial_report);
    RUN_TEST(test_joystick_every_control_checked);
    RUN_TEST(test_joystick_no_changes_dont_send_updates);
    RUN_TEST(test_joystick_axis_change_sends_one_update);
}

