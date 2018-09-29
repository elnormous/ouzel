// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "KeyboardMacOS.hpp"
#include "InputSystemMacOS.hpp"

namespace ouzel
{
    namespace input
    {
        void KeyboardMacOS::handleKeyPress(Keyboard::Key key, uint32_t modifiers)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::KEY_PRESS;
            event.deviceId = deviceId;
            event.keyboardKey = key;
            event.modifiers = modifiers;
            inputSystemMacOS.addEvent(event);
        }

        void KeyboardMacOS::handleKeyRelease(Keyboard::Key key, uint32_t modifiers)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::KEY_RELEASE;
            event.deviceId = deviceId;
            event.keyboardKey = key;
            event.modifiers = modifiers;
            inputSystemMacOS.addEvent(event);
        }
    } // namespace input
} // namespace ouzel
