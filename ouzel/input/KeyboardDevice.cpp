// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "KeyboardDevice.hpp"
#include "InputSystem.hpp"

namespace ouzel
{
    namespace input
    {
        void KeyboardDevice::handleKeyPress(Keyboard::Key key)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::KEY_PRESS;
            event.deviceId = id;
            event.keyboardKey = key;
            inputSystem.addEvent(event);
        }

        void KeyboardDevice::handleKeyRelease(Keyboard::Key key)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::KEY_RELEASE;
            event.deviceId = id;
            event.keyboardKey = key;
            inputSystem.addEvent(event);
        }
    } // namespace input
} // namespace ouzel
