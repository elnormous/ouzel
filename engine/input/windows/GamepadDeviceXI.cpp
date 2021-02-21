// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include <system_error>
#include "GamepadDeviceXI.hpp"

namespace ouzel::input::windows
{
    namespace
    {
        constexpr std::int32_t minThumbValue = -32768;
        constexpr std::int32_t maxThumbValue = 32767;
    }

    GamepadDeviceXI::GamepadDeviceXI(InputSystem& initInputSystem,
                                     DeviceId initId,
                                     DWORD initPlayerIndex):
        GamepadDevice(initInputSystem, initId),
        playerIndex(initPlayerIndex)
    {
    }

    void GamepadDeviceXI::update()
    {
        XINPUT_STATE newState = {};

        if (const auto result = XInputGetState(playerIndex, &newState); result != ERROR_SUCCESS)
        {
            if (result == ERROR_DEVICE_NOT_CONNECTED)
                throw std::runtime_error("Gamepad " + std::to_string(playerIndex) + " disconnected");
            else
                throw std::system_error(result, std::system_category(), "Failed to get state for gamepad " + std::to_string(playerIndex));
        }

        if (newState.dwPacketNumber > state.dwPacketNumber)
        {
            // buttons
            checkButton(newState, XINPUT_GAMEPAD_DPAD_UP, Gamepad::Button::dPadUp);
            checkButton(newState, XINPUT_GAMEPAD_DPAD_DOWN, Gamepad::Button::dPadDown);
            checkButton(newState, XINPUT_GAMEPAD_DPAD_LEFT, Gamepad::Button::dPadLeft);
            checkButton(newState, XINPUT_GAMEPAD_DPAD_RIGHT, Gamepad::Button::dPadRight);
            checkButton(newState, XINPUT_GAMEPAD_START, Gamepad::Button::start);
            checkButton(newState, XINPUT_GAMEPAD_BACK, Gamepad::Button::back);
            checkButton(newState, XINPUT_GAMEPAD_LEFT_THUMB, Gamepad::Button::leftThumb);
            checkButton(newState, XINPUT_GAMEPAD_RIGHT_THUMB, Gamepad::Button::rightThumb);
            checkButton(newState, XINPUT_GAMEPAD_LEFT_SHOULDER, Gamepad::Button::leftShoulder);
            checkButton(newState, XINPUT_GAMEPAD_RIGHT_SHOULDER, Gamepad::Button::rightShoulder);
            checkButton(newState, XINPUT_GAMEPAD_A, Gamepad::Button::faceBottom);
            checkButton(newState, XINPUT_GAMEPAD_B, Gamepad::Button::faceRight);
            checkButton(newState, XINPUT_GAMEPAD_X, Gamepad::Button::faceLeft);
            checkButton(newState, XINPUT_GAMEPAD_Y, Gamepad::Button::faceTop);

            // triggers
            if (newState.Gamepad.bLeftTrigger != state.Gamepad.bLeftTrigger)
            {
                handleButtonValueChange(Gamepad::Button::leftTrigger,
                                        newState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD,
                                        static_cast<float>(newState.Gamepad.bLeftTrigger) / 255.0F);
            }

            if (newState.Gamepad.bRightTrigger != state.Gamepad.bRightTrigger)
            {
                handleButtonValueChange(Gamepad::Button::rightTrigger,
                                        newState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD,
                                        static_cast<float>(newState.Gamepad.bRightTrigger) / 255.0F);
            }

            // left thumbstick
            checkThumbAxis(state.Gamepad.sThumbLX, newState.Gamepad.sThumbLX, Gamepad::Button::leftThumbLeft, Gamepad::Button::leftThumbRight);
            checkThumbAxis(state.Gamepad.sThumbLY, newState.Gamepad.sThumbLY, Gamepad::Button::leftThumbDown, Gamepad::Button::leftThumbUp);

            // right thumbstick
            checkThumbAxis(state.Gamepad.sThumbRX, newState.Gamepad.sThumbRX, Gamepad::Button::rightThumbLeft, Gamepad::Button::rightThumbRight);
            checkThumbAxis(state.Gamepad.sThumbRY, newState.Gamepad.sThumbRY, Gamepad::Button::rightThumbDown, Gamepad::Button::rightThumbUp);

            state = newState;
        }
    }

    std::int32_t GamepadDeviceXI::getPlayerIndex() const
    {
        return static_cast<std::int32_t>(playerIndex);
    }

    void GamepadDeviceXI::checkThumbAxis(SHORT oldValue, SHORT newValue, Gamepad::Button negativeButton, Gamepad::Button positiveButton)
    {
        if (newValue != oldValue)
        {
            if (newValue > 0)
            {
                handleButtonValueChange(positiveButton,
                                        newValue > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
                                        static_cast<float>(newValue) / static_cast<float>(maxThumbValue));
            }
            else if (newValue < 0)
            {
                handleButtonValueChange(negativeButton,
                                        newValue < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
                                        static_cast<float>(newValue) / static_cast<float>(minThumbValue));
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

    void GamepadDeviceXI::checkButton(const XINPUT_STATE& newState, WORD mask, Gamepad::Button button)
    {
        if ((newState.Gamepad.wButtons & mask) != (state.Gamepad.wButtons & mask))
        {
            const bool pressed = ((newState.Gamepad.wButtons & mask) == mask);
            handleButtonValueChange(button, pressed, pressed ? 1.0F : 0.0F);
        }
    }

    void GamepadDeviceXI::setVibration(Gamepad::Motor motor, float speed)
    {
        switch (motor)
        {
        case Gamepad::Motor::all:
            vibration.wLeftMotorSpeed = static_cast<WORD>(speed);
            vibration.wRightMotorSpeed = static_cast<WORD>(speed);
            break;
        case Gamepad::Motor::left:
            vibration.wLeftMotorSpeed = static_cast<WORD>(speed);
            break;
        case Gamepad::Motor::right:
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
        case Gamepad::Motor::all:
        case Gamepad::Motor::left:
            return vibration.wLeftMotorSpeed;
        case Gamepad::Motor::right:
            return vibration.wRightMotorSpeed;
        default:
            return 0.0F;
        }
    }
}
