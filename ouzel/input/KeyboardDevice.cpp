// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "KeyboardDevice.hpp"
#include "InputSystem.hpp"

namespace ouzel
{
    namespace input
    {
        void KeyboardDevice::handleKeyPress(Keyboard::Key key, uint32_t modifiers)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::KEY_PRESS;
            event.deviceId = id;
            event.keyboardKey = key;
            event.modifiers = modifiers;
            inputSystem.addEvent(event);
        }

        void KeyboardDevice::handleKeyRelease(Keyboard::Key key, uint32_t modifiers)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::KEY_RELEASE;
            event.deviceId = id;
            event.keyboardKey = key;
            event.modifiers = modifiers;
            inputSystem.addEvent(event);
        }
    } // namespace input
} // namespace ouzel
