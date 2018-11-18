// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "GamepadDevice.hpp"
#include "InputSystem.hpp"

namespace ouzel
{
    namespace input
    {
        std::future<bool> GamepadDevice::handleButtonValueChange(Gamepad::Button button, bool pressed, float value)
        {
            InputSystem::Event event(InputSystem::Event::Type::GAMEPAD_BUTTON_CHANGE);
            event.deviceId = id;
            event.gamepadButton = button;
            event.pressed = pressed;
            event.value = value;

            return inputSystem.postEvent(event);
        }
    } // namespace input
} // namespace ouzel
