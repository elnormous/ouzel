// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "GamepadWin.h"
#include "Engine.h"
#include "Utils.h"

namespace ouzel
{
    namespace input
    {
        const int32_t MAX_THUMB_VALUE = 32767;
        const int32_t MIN_THUMB_VALUE = -32768;

        GamepadWin::GamepadWin(int32_t playerIndex) :
            _playerIndex(playerIndex)
        {
            memset(&_state, 0, sizeof(XINPUT_STATE));
            memset(&_vibration, 0, sizeof(XINPUT_VIBRATION));
        }

        void GamepadWin::update(const XINPUT_STATE& state)
        {
            if (state.dwPacketNumber > _state.dwPacketNumber)
            {
                // buttons
                checkButton(state, XINPUT_GAMEPAD_DPAD_UP, GamepadButton::DPAD_UP);
                checkButton(state, XINPUT_GAMEPAD_DPAD_DOWN, GamepadButton::DPAD_DOWN);
                checkButton(state, XINPUT_GAMEPAD_DPAD_LEFT, GamepadButton::DPAD_LEFT);
                checkButton(state, XINPUT_GAMEPAD_DPAD_RIGHT, GamepadButton::DPAD_RIGHT);
                checkButton(state, XINPUT_GAMEPAD_START, GamepadButton::START);
                checkButton(state, XINPUT_GAMEPAD_BACK, GamepadButton::BACK);
                checkButton(state, XINPUT_GAMEPAD_LEFT_THUMB, GamepadButton::LEFT_THUMB);
                checkButton(state, XINPUT_GAMEPAD_RIGHT_THUMB, GamepadButton::RIGHT_THUMB);
                checkButton(state, XINPUT_GAMEPAD_LEFT_SHOULDER, GamepadButton::LEFT_SHOULDER);
                checkButton(state, XINPUT_GAMEPAD_RIGHT_SHOULDER, GamepadButton::RIGHT_SHOULDER);
                checkButton(state, XINPUT_GAMEPAD_A, GamepadButton::A);
                checkButton(state, XINPUT_GAMEPAD_B, GamepadButton::B);
                checkButton(state, XINPUT_GAMEPAD_X, GamepadButton::X);
                checkButton(state, XINPUT_GAMEPAD_Y, GamepadButton::Y);

                // triggers
                if (state.Gamepad.bLeftTrigger != _state.Gamepad.bLeftTrigger)
                {
                    handleButtonValueChange(GamepadButton::LEFT_TRIGGER,
                        state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD,
                        static_cast<float>(state.Gamepad.bLeftTrigger) / 255.0f);
                }

                if (state.Gamepad.bRightTrigger != _state.Gamepad.bRightTrigger)
                {
                    handleButtonValueChange(GamepadButton::RIGHT_TRIGGER,
                        state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD,
                        static_cast<float>(state.Gamepad.bRightTrigger) / 255.0f);
                }

                // left thumbstick
                checkThumbAxis(_state.Gamepad.sThumbLX, state.Gamepad.sThumbLX, GamepadButton::LEFT_THUMB_LEFT, GamepadButton::LEFT_THUMB_RIGHT);
                checkThumbAxis(_state.Gamepad.sThumbLY, state.Gamepad.sThumbLY, GamepadButton::LEFT_THUMB_DOWN, GamepadButton::LEFT_THUMB_UP);

                // right thumbstick
                checkThumbAxis(_state.Gamepad.sThumbRX, state.Gamepad.sThumbRX, GamepadButton::RIGHT_THUMB_LEFT, GamepadButton::RIGHT_THUMB_RIGHT);
                checkThumbAxis(_state.Gamepad.sThumbRY, state.Gamepad.sThumbRY, GamepadButton::RIGHT_THUMB_DOWN, GamepadButton::RIGHT_THUMB_UP);

                _state = state;
            }
        }

        int32_t GamepadWin::getPlayerIndex() const
        {
            return _playerIndex;
        }

        void GamepadWin::checkThumbAxis(SHORT oldValue, SHORT newValue, GamepadButton negativeButton, GamepadButton positiveButton)
        {
            if (newValue != oldValue)
            {
                if (newValue > 0)
                {
                    handleButtonValueChange(positiveButton,
                        newValue > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
                        static_cast<float>(newValue) / static_cast<float>(MAX_THUMB_VALUE));
                }
                else if (newValue < 0)
                {
                    handleButtonValueChange(negativeButton,
                        newValue < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
                        static_cast<float>(newValue) / static_cast<float>(MIN_THUMB_VALUE));
                }
                else // thumbstick is 0
                {
                    if (oldValue > newValue)
                    {
                        handleButtonValueChange(positiveButton, false, 0.0f);
                    }
                    else
                    {
                        handleButtonValueChange(negativeButton, false, 0.0f);
                    }
                }
            }
        }

        void GamepadWin::checkButton(const XINPUT_STATE& state, WORD mask, GamepadButton button)
        {
            if ((state.Gamepad.wButtons & mask) != (_state.Gamepad.wButtons & mask))
            {
                bool pressed = (state.Gamepad.wButtons & mask);
                handleButtonValueChange(button, pressed, pressed ? 1.0f : 0.0f);
            }
        }

        void GamepadWin::setVibration(Motor motor, float speed)
        {
            switch (motor)
            {
            case Motor::ALL:
                _vibration.wLeftMotorSpeed = speed;
                _vibration.wRightMotorSpeed = speed;
                break;
            case Motor::LEFT:
                _vibration.wLeftMotorSpeed = speed;
                break;
            case Motor::RIGHT:
                _vibration.wRightMotorSpeed = speed;
                break;
            }

            XInputSetState(_playerIndex, &_vibration);
        }

        float GamepadWin::getVibration(Motor motor)
        {
            switch (motor)
            {
            case Motor::ALL:
            case Motor::LEFT:
                return _vibration.wLeftMotorSpeed;
                break;
            case Motor::RIGHT:
                return _vibration.wRightMotorSpeed;
            }

            return 0.0f;
        }
    } // namespace input
} // namespace ouzel
