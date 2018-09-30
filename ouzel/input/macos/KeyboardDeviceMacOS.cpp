// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "KeyboardDeviceMacOS.hpp"
#include "InputSystemMacOS.hpp"

namespace ouzel
{
    namespace input
    {
        void KeyboardDeviceMacOS::handleKeyPress(Keyboard::Key key, uint32_t modifiers)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::KEY_PRESS;
            event.deviceId = id;
            event.keyboardKey = key;
            event.modifiers = modifiers;
            inputSystemMacOS.addEvent(event);
        }

        void KeyboardDeviceMacOS::handleKeyRelease(Keyboard::Key key, uint32_t modifiers)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::KEY_RELEASE;
            event.deviceId = id;
            event.keyboardKey = key;
            event.modifiers = modifiers;
            inputSystemMacOS.addEvent(event);
        }
    } // namespace input
} // namespace ouzel
