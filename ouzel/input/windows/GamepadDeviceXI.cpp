// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "GamepadDeviceXI.hpp"
#include "InputManagerWin.hpp"
#include "InputSystemWin.hpp"
#include "utils/Errors.hpp"

namespace ouzel
{
    namespace input
    {
        static const int32_t MAX_THUMB_VALUE = 32767;
        static const int32_t MIN_THUMB_VALUE = -32768;

        GamepadDeviceXI::GamepadDeviceXI(InputSystemWin& initInputSystemWin,
                                         uint32_t initId,
                                         DWORD aPlayerIndex):
            GamepadDeviceWin(initInputSystemWin, initId),
            playerIndex(aPlayerIndex)
        {
            ZeroMemory(&state, sizeof(XINPUT_STATE));
            ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
        }

        void GamepadDeviceXI::update()
        {
            XINPUT_STATE newState;
            ZeroMemory(&newState, sizeof(XINPUT_STATE));

            DWORD result = XInputGetState(playerIndex, &newState);

            if (result != ERROR_SUCCESS)
            {
                if (result == ERROR_DEVICE_NOT_CONNECTED)
                    throw SystemError("Gamepad " + std::to_string(playerIndex) + " disconnected");
                else
                    throw SystemError("Failed to get state for gamepad " + std::to_string(playerIndex));
            }

            if (newState.dwPacketNumber > state.dwPacketNumber)
            {
                // buttons
                checkButton(newState, XINPUT_GAMEPAD_DPAD_UP, Gamepad::Button::DPAD_UP);
                checkButton(newState, XINPUT_GAMEPAD_DPAD_DOWN, Gamepad::Button::DPAD_DOWN);
                checkButton(newState, XINPUT_GAMEPAD_DPAD_LEFT, Gamepad::Button::DPAD_LEFT);
                checkButton(newState, XINPUT_GAMEPAD_DPAD_RIGHT, Gamepad::Button::DPAD_RIGHT);
                checkButton(newState, XINPUT_GAMEPAD_START, Gamepad::Button::START);
                checkButton(newState, XINPUT_GAMEPAD_BACK, Gamepad::Button::BACK);
                checkButton(newState, XINPUT_GAMEPAD_LEFT_THUMB, Gamepad::Button::LEFT_THUMB);
                checkButton(newState, XINPUT_GAMEPAD_RIGHT_THUMB, Gamepad::Button::RIGHT_THUMB);
                checkButton(newState, XINPUT_GAMEPAD_LEFT_SHOULDER, Gamepad::Button::LEFT_SHOULDER);
                checkButton(newState, XINPUT_GAMEPAD_RIGHT_SHOULDER, Gamepad::Button::RIGHT_SHOULDER);
                checkButton(newState, XINPUT_GAMEPAD_A, Gamepad::Button::FACE_BOTTOM);
                checkButton(newState, XINPUT_GAMEPAD_B, Gamepad::Button::FACE_RIGHT);
                checkButton(newState, XINPUT_GAMEPAD_X, Gamepad::Button::FACE_LEFT);
                checkButton(newState, XINPUT_GAMEPAD_Y, Gamepad::Button::FACE_TOP);

                // triggers
                if (newState.Gamepad.bLeftTrigger != state.Gamepad.bLeftTrigger)
                {
                    inputSystemWin.handleButtonValueChange(*this,
                                                           Gamepad::Button::LEFT_TRIGGER,
                                                           newState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD,
                                                           static_cast<float>(newState.Gamepad.bLeftTrigger) / 255.0F);
                }

                if (newState.Gamepad.bRightTrigger != state.Gamepad.bRightTrigger)
                {
                    inputSystemWin.handleButtonValueChange(*this,
                                                           Gamepad::Button::RIGHT_TRIGGER,
                                                           newState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD,
                                                           static_cast<float>(newState.Gamepad.bRightTrigger) / 255.0F);
                }

                // left thumbstick
                checkThumbAxis(state.Gamepad.sThumbLX, newState.Gamepad.sThumbLX, Gamepad::Button::LEFT_THUMB_LEFT, Gamepad::Button::LEFT_THUMB_RIGHT);
                checkThumbAxis(state.Gamepad.sThumbLY, newState.Gamepad.sThumbLY, Gamepad::Button::LEFT_THUMB_DOWN, Gamepad::Button::LEFT_THUMB_UP);

                // right thumbstick
                checkThumbAxis(state.Gamepad.sThumbRX, newState.Gamepad.sThumbRX, Gamepad::Button::RIGHT_THUMB_LEFT, Gamepad::Button::RIGHT_THUMB_RIGHT);
                checkThumbAxis(state.Gamepad.sThumbRY, newState.Gamepad.sThumbRY, Gamepad::Button::RIGHT_THUMB_DOWN, Gamepad::Button::RIGHT_THUMB_UP);

                state = newState;
            }
        }

        int32_t GamepadDeviceXI::getPlayerIndex() const
        {
            return static_cast<int32_t>(playerIndex);
        }

        void GamepadDeviceXI::checkThumbAxis(SHORT oldValue, SHORT newValue, Gamepad::Button negativeButton, Gamepad::Button positiveButton)
        {
            if (newValue != oldValue)
            {
                if (newValue > 0)
                {
                    inputSystemWin.handleButtonValueChange(*this,
                                                           positiveButton,
                                                           newValue > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
                                                           static_cast<float>(newValue) / static_cast<float>(MAX_THUMB_VALUE));
                }
                else if (newValue < 0)
                {
                    inputSystemWin.handleButtonValueChange(*this,
                                                           negativeButton,
                                                           newValue < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
                                                           static_cast<float>(newValue) / static_cast<float>(MIN_THUMB_VALUE));
                }
                else // thumbstick is 0
                {
                    if (oldValue > newValue)
                        inputSystemWin.handleButtonValueChange(*this, positiveButton, false, 0.0F);
                    else
                        inputSystemWin.handleButtonValueChange(*this, negativeButton, false, 0.0F);
                }
            }
        }

        void GamepadDeviceXI::checkButton(const XINPUT_STATE& newState, WORD mask, Gamepad::Button button)
        {
            if ((newState.Gamepad.wButtons & mask) != (state.Gamepad.wButtons & mask))
            {
                bool pressed = ((newState.Gamepad.wButtons & mask) == mask);
                inputSystemWin.handleButtonValueChange(*this, button, pressed, pressed ? 1.0F : 0.0F);
            }
        }

        void GamepadDeviceXI::setVibration(Gamepad::Motor motor, float speed)
        {
            switch (motor)
            {
            case Gamepad::Motor::ALL:
                vibration.wLeftMotorSpeed = static_cast<WORD>(speed);
                vibration.wRightMotorSpeed = static_cast<WORD>(speed);
                break;
            case Gamepad::Motor::LEFT:
                vibration.wLeftMotorSpeed = static_cast<WORD>(speed);
                break;
            case Gamepad::Motor::RIGHT:
                vibration.wRightMotorSpeed = static_cast<WORD>(speed);
                break;
            default:
                return;
            }

            XInputSetState(playerIndex, &vibration);
        }

        float GamepadDeviceXI::getVibration(Gamepad::Motor motor)
        {
            switch (motor)
            {
            case Gamepad::Motor::ALL:
            case Gamepad::Motor::LEFT:
                return vibration.wLeftMotorSpeed;
                break;
            case Gamepad::Motor::RIGHT:
                return vibration.wRightMotorSpeed;
            default:
                return 0.0f;
            }
        }
    } // namespace input
} // namespace ouzel
