// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "GamepadXI.hpp"
#include "InputManagerWin.hpp"
#include "core/Engine.hpp"

namespace ouzel
{
    namespace input
    {
        static const int32_t MAX_THUMB_VALUE = 32767;
        static const int32_t MIN_THUMB_VALUE = -32768;

        GamepadXI::GamepadXI(DWORD aPlayerIndex):
            playerIndex(aPlayerIndex)
        {
            ZeroMemory(&state, sizeof(XINPUT_STATE));
            ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
        }

        void GamepadXI::update(const XINPUT_STATE& newState)
        {
            if (newState.dwPacketNumber > state.dwPacketNumber)
            {
                // buttons
                checkButton(newState, XINPUT_GAMEPAD_DPAD_UP, GamepadButton::DPAD_UP);
                checkButton(newState, XINPUT_GAMEPAD_DPAD_DOWN, GamepadButton::DPAD_DOWN);
                checkButton(newState, XINPUT_GAMEPAD_DPAD_LEFT, GamepadButton::DPAD_LEFT);
                checkButton(newState, XINPUT_GAMEPAD_DPAD_RIGHT, GamepadButton::DPAD_RIGHT);
                checkButton(newState, XINPUT_GAMEPAD_START, GamepadButton::START);
                checkButton(newState, XINPUT_GAMEPAD_BACK, GamepadButton::BACK);
                checkButton(newState, XINPUT_GAMEPAD_LEFT_THUMB, GamepadButton::LEFT_THUMB);
                checkButton(newState, XINPUT_GAMEPAD_RIGHT_THUMB, GamepadButton::RIGHT_THUMB);
                checkButton(newState, XINPUT_GAMEPAD_LEFT_SHOULDER, GamepadButton::LEFT_SHOULDER);
                checkButton(newState, XINPUT_GAMEPAD_RIGHT_SHOULDER, GamepadButton::RIGHT_SHOULDER);
                checkButton(newState, XINPUT_GAMEPAD_A, GamepadButton::FACE_BOTTOM);
                checkButton(newState, XINPUT_GAMEPAD_B, GamepadButton::FACE_RIGHT);
                checkButton(newState, XINPUT_GAMEPAD_X, GamepadButton::FACE_LEFT);
                checkButton(newState, XINPUT_GAMEPAD_Y, GamepadButton::FACE_TOP);

                // triggers
                if (newState.Gamepad.bLeftTrigger != state.Gamepad.bLeftTrigger)
                {
                    handleButtonValueChange(GamepadButton::LEFT_TRIGGER,
                        newState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD,
                        static_cast<float>(newState.Gamepad.bLeftTrigger) / 255.0F);
                }

                if (newState.Gamepad.bRightTrigger != state.Gamepad.bRightTrigger)
                {
                    handleButtonValueChange(GamepadButton::RIGHT_TRIGGER,
                        newState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD,
                        static_cast<float>(newState.Gamepad.bRightTrigger) / 255.0F);
                }

                // left thumbstick
                checkThumbAxis(state.Gamepad.sThumbLX, newState.Gamepad.sThumbLX, GamepadButton::LEFT_THUMB_LEFT, GamepadButton::LEFT_THUMB_RIGHT);
                checkThumbAxis(state.Gamepad.sThumbLY, newState.Gamepad.sThumbLY, GamepadButton::LEFT_THUMB_DOWN, GamepadButton::LEFT_THUMB_UP);

                // right thumbstick
                checkThumbAxis(state.Gamepad.sThumbRX, newState.Gamepad.sThumbRX, GamepadButton::RIGHT_THUMB_LEFT, GamepadButton::RIGHT_THUMB_RIGHT);
                checkThumbAxis(state.Gamepad.sThumbRY, newState.Gamepad.sThumbRY, GamepadButton::RIGHT_THUMB_DOWN, GamepadButton::RIGHT_THUMB_UP);

                state = newState;
            }
        }

        int32_t GamepadXI::getPlayerIndex() const
        {
            return static_cast<int32_t>(playerIndex);
        }

        void GamepadXI::checkThumbAxis(SHORT oldValue, SHORT newValue, GamepadButton negativeButton, GamepadButton positiveButton)
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
                        handleButtonValueChange(positiveButton, false, 0.0F);
                    else
                        handleButtonValueChange(negativeButton, false, 0.0F);
                }
            }
        }

        void GamepadXI::checkButton(const XINPUT_STATE& newState, WORD mask, GamepadButton button)
        {
            if ((newState.Gamepad.wButtons & mask) != (state.Gamepad.wButtons & mask))
            {
                bool pressed = ((newState.Gamepad.wButtons & mask) == mask);
                handleButtonValueChange(button, pressed, pressed ? 1.0F : 0.0F);
            }
        }

        void GamepadXI::setVibration(Motor motor, float speed)
        {
            switch (motor)
            {
            case Motor::ALL:
                vibration.wLeftMotorSpeed = static_cast<WORD>(speed);
                vibration.wRightMotorSpeed = static_cast<WORD>(speed);
                break;
            case Motor::LEFT:
                vibration.wLeftMotorSpeed = static_cast<WORD>(speed);
                break;
            case Motor::RIGHT:
                vibration.wRightMotorSpeed = static_cast<WORD>(speed);
                break;
            default:
                return;
            }

            XInputSetState(playerIndex, &vibration);
        }

        float GamepadXI::getVibration(Motor motor)
        {
            switch (motor)
            {
            case Motor::ALL:
            case Motor::LEFT:
                return vibration.wLeftMotorSpeed;
                break;
            case Motor::RIGHT:
                return vibration.wRightMotorSpeed;
            default:
                return 0.0f;
            }
        }
    } // namespace input
} // namespace ouzel
