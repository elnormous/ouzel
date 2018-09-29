// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "MouseMacOS.hpp"
#include "InputSystemMacOS.hpp"

namespace ouzel
{
    namespace input
    {
        void MouseMacOS::handleButtonPress(Mouse::Button button, const Vector2& position, uint32_t modifiers)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::MOUSE_PRESS;
            event.deviceId = deviceId;
            event.mouseButton = button;
            event.position = position;
            event.modifiers = modifiers;
            inputSystemMacOS.addEvent(event);
        }

        void MouseMacOS::handleButtonRelease(Mouse::Button button, const Vector2& position, uint32_t modifiers)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::MOUSE_RELEASE;
            event.deviceId = deviceId;
            event.mouseButton = button;
            event.position = position;
            event.modifiers = modifiers;
            inputSystemMacOS.addEvent(event);
        }

        void MouseMacOS::handleMove(const Vector2& position, uint32_t modifiers)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::MOUSE_MOVE;
            event.deviceId = deviceId;
            event.position = position;
            event.modifiers = modifiers;
            inputSystemMacOS.addEvent(event);
        }

        void MouseMacOS::handleScroll(const Vector2& scroll, const Vector2& position, uint32_t modifiers)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::MOUSE_SCROLL;
            event.deviceId = deviceId;
            event.position = position;
            event.scroll = scroll;
            event.modifiers = modifiers;
            inputSystemMacOS.addEvent(event);
        }
    } // namespace input
} // namespace ouzel
