// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "MouseDevice.hpp"
#include "InputSystem.hpp"

namespace ouzel
{
    namespace input
    {
        void MouseDevice::handleButtonPress(Mouse::Button button, const Vector2& position, uint32_t modifiers)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::MOUSE_PRESS;
            event.deviceId = id;
            event.mouseButton = button;
            event.position = position;
            event.modifiers = modifiers;
            inputSystem.addEvent(event);
        }

        void MouseDevice::handleButtonRelease(Mouse::Button button, const Vector2& position, uint32_t modifiers)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::MOUSE_RELEASE;
            event.deviceId = id;
            event.mouseButton = button;
            event.position = position;
            event.modifiers = modifiers;
            inputSystem.addEvent(event);
        }

        void MouseDevice::handleMove(const Vector2& position, uint32_t modifiers)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::MOUSE_MOVE;
            event.deviceId = id;
            event.position = position;
            event.modifiers = modifiers;
            inputSystem.addEvent(event);
        }

        void MouseDevice::handleScroll(const Vector2& scroll, const Vector2& position, uint32_t modifiers)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::MOUSE_SCROLL;
            event.deviceId = id;
            event.position = position;
            event.scroll = scroll;
            event.modifiers = modifiers;
            inputSystem.addEvent(event);
        }

        void MouseDevice::handleCursorLockChange(bool locked)
        {
            /*InputSystem::Event event;
            event.type = InputSystem::Event::Type::MOUSE_LOCK_CHANGED;

            event.deviceId = id;
            event.locked = locked;

            inputSystem.addEvent(event);*/
        }
    } // namespace input
} // namespace ouzel
