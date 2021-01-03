
#include <gmock/gmock.h>

using ::testing::Return;
using ::testing::_;
using ::testing::SaveArg;
using ::testing::Mock;
using ::testing::Expectation;

#include <ALGpio.h>
#include <ALHIDJoystick.hpp>

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

class MockGamepadAPI
{
    public:
        MOCK_METHOD(void, begin,        ());
        MOCK_METHOD(void, end,          ());
        MOCK_METHOD(void, write,        ());
        MOCK_METHOD(void, press,        (uint8_t b));
        MOCK_METHOD(void, release,      (uint8_t b));
        MOCK_METHOD(void, releaseAll,   ());
        MOCK_METHOD(void, buttons,      (uint32_t b));
        MOCK_METHOD(void, xAxis,        (int16_t a));
        MOCK_METHOD(void, yAxis,        (int16_t a));
        MOCK_METHOD(void, zAxis,        (int8_t a));
        MOCK_METHOD(void, rxAxis,       (int16_t a));
        MOCK_METHOD(void, ryAxis,       (int16_t a));
        MOCK_METHOD(void, rzAxis,       (int8_t a));
        MOCK_METHOD(void, dPad1,        (int8_t d));
        MOCK_METHOD(void, dPad2,        (int8_t d));
        MOCK_METHOD(void, SendReport,   (void* data, int length) ) ;
};


//void test_joystick_begin_reads_initial_state(void)
TEST(HID, begin_reads_initial_state)
{
    /* Given */
    MockGpio mockGpio;
    MockGamepadAPI mockGamepad;
    ALHIDJoystick<MockGamepadAPI> joystick = ALHIDJoystick<MockGamepadAPI>(&mockGpio, &mockGamepad);
    EXPECT_CALL(mockGamepad, begin()).Times(1);
    EXPECT_CALL(mockGamepad, end()).Times(1);
    EXPECT_CALL(mockGpio, getJoystick(_)).Times(JOYSTICK_AXIS_COUNT).WillRepeatedly(Return(false));
    EXPECT_CALL(mockGpio, getButton(_)).Times(BUTTON_COUNT).WillRepeatedly(Return(false));

    /* When */
    joystick.begin();
}

//void test_joystick_process_every_control_checked(void)
TEST(HID, joystick_process_every_control_checked)
{

    /* Given */
    MockGpio mockGpio;
    MockGamepadAPI mockGamepad;
    ALHIDJoystick<MockGamepadAPI> joystick = ALHIDJoystick<MockGamepadAPI>(&mockGpio, &mockGamepad);
    EXPECT_CALL(mockGpio, getButton(BUTTON_B1)).WillOnce(Return(false));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B2)).WillOnce(Return(false));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B3)).WillOnce(Return(false));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B4)).WillOnce(Return(false));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B5)).WillOnce(Return(false));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B6)).WillOnce(Return(false));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B7)).WillOnce(Return(false));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B8)).WillOnce(Return(false));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B9)).WillOnce(Return(false));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B10)).WillOnce(Return(false));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B11)).WillOnce(Return(false));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B12)).WillOnce(Return(false));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B13)).WillOnce(Return(false));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B14)).WillOnce(Return(false));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B15)).WillOnce(Return(false));
    EXPECT_CALL(mockGpio, getButton(BUTTON_B16)).WillOnce(Return(false));
    EXPECT_CALL(mockGpio, getJoystick(JOYSTICK_UP))   .WillOnce(Return(false));
    EXPECT_CALL(mockGpio, getJoystick(JOYSTICK_DOWN)) .WillOnce(Return(false));
    EXPECT_CALL(mockGpio, getJoystick(JOYSTICK_LEFT)) .WillOnce(Return(false));
    EXPECT_CALL(mockGpio, getJoystick(JOYSTICK_RIGHT)).WillOnce(Return(false));
    EXPECT_CALL(mockGamepad, releaseAll()).WillOnce(Return());
    EXPECT_CALL(mockGamepad, press(_)).Times(0);
    EXPECT_CALL(mockGamepad, dPad1(_)).Times(0);
    EXPECT_CALL(mockGamepad, write()).WillOnce(Return());
    EXPECT_CALL(mockGamepad, end()).Times(1);

    /* When */
    joystick.process();

}

//void test_joystick_no_changes_dont_send_updates(void)
TEST(HID, joystick_no_changes_dont_send_updates)
{
    /* Given */
    MockGpio mockGpio;
    MockGamepadAPI mockGamepad;
    ALHIDJoystick<MockGamepadAPI> joystick = ALHIDJoystick<MockGamepadAPI>(&mockGpio, &mockGamepad);
    EXPECT_CALL(mockGamepad, releaseAll()).WillOnce(Return());
    EXPECT_CALL(mockGamepad, press(_)).WillRepeatedly(Return());
    EXPECT_CALL(mockGamepad, dPad1(_)).WillRepeatedly(Return());
    EXPECT_CALL(mockGamepad, write()).WillOnce(Return());
    EXPECT_CALL(mockGamepad, end()).Times(1);
    EXPECT_CALL(mockGpio, getJoystick(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(mockGpio, getButton(_)).WillRepeatedly(Return(false));

    /* When 1st proces is called, SendReport is called. 2nd (and more) call SendReport is not called */
    joystick.process();
    joystick.process();
    joystick.process();
    joystick.process();
    joystick.process();


}


//void test_joystick_axis_change_sends_one_update(void)
TEST(HID, joystick_axis_change_sends_one_update)
{
    /* Given */
    MockGpio mockGpio;
    MockGamepadAPI mockGamepad;
    ALHIDJoystick<MockGamepadAPI> joystick = ALHIDJoystick<MockGamepadAPI>(&mockGpio, &mockGamepad);
    EXPECT_CALL(mockGamepad, write()).Times(2).WillRepeatedly(Return());
    EXPECT_CALL(mockGamepad, releaseAll()).WillRepeatedly(Return());
    EXPECT_CALL(mockGamepad, press(_)).WillRepeatedly(Return());
    EXPECT_CALL(mockGamepad, dPad1(_)).WillRepeatedly(Return());
    EXPECT_CALL(mockGamepad, end()).Times(1);
    EXPECT_CALL(mockGpio, getButton(_)).WillRepeatedly(Return(false));
    EXPECT_CALL(mockGpio, getJoystick(JOYSTICK_UP))   .WillOnce(Return(false)).WillOnce(Return(false)).WillOnce(Return(false));
    EXPECT_CALL(mockGpio, getJoystick(JOYSTICK_DOWN)) .WillOnce(Return(false)).WillOnce(Return(false)).WillOnce(Return(false));
    EXPECT_CALL(mockGpio, getJoystick(JOYSTICK_LEFT)) .WillOnce(Return(false)).WillOnce(Return(false)).WillOnce(Return(false));
    EXPECT_CALL(mockGpio, getJoystick(JOYSTICK_RIGHT)).WillOnce(Return(false)).WillOnce(Return(true)).WillOnce(Return(true));

    /* When */
    joystick.process();
    joystick.process();
    joystick.process();
    
}
