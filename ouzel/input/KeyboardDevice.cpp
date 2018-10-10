// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "KeyboardDevice.hpp"
#include "InputSystem.hpp"

namespace ouzel
{
    namespace input
    {
        std::future<bool> KeyboardDevice::handleKeyPress(Keyboard::Key key)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::KEY_PRESS;
            event.deviceId = id;
            event.keyboardKey = key;
            return inputSystem.postEvent(event);
        }

        std::future<bool> KeyboardDevice::handleKeyRelease(Keyboard::Key key)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::KEY_RELEASE;
            event.deviceId = id;
            event.keyboardKey = key;
            return inputSystem.postEvent(event);
        }
    } // namespace input
} // namespace ouzel
