// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "MouseDevice.hpp"
#include "InputSystem.hpp"

namespace ouzel
{
    namespace input
    {
        std::future<bool> MouseDevice::handleButtonPress(Mouse::Button button, const Vector2& position)
        {
            InputSystem::Event event(InputSystem::Event::Type::MOUSE_PRESS);
            event.deviceId = id;
            event.mouseButton = button;
            event.position = position;
            return inputSystem.postEvent(event);
        }

        std::future<bool> MouseDevice::handleButtonRelease(Mouse::Button button, const Vector2& position)
        {
            InputSystem::Event event(InputSystem::Event::Type::MOUSE_RELEASE);
            event.deviceId = id;
            event.mouseButton = button;
            event.position = position;
            return inputSystem.postEvent(event);
        }

        std::future<bool> MouseDevice::handleMove(const Vector2& position)
        {
            InputSystem::Event event(InputSystem::Event::Type::MOUSE_MOVE);
            event.deviceId = id;
            event.position = position;
            return inputSystem.postEvent(event);
        }

        std::future<bool> MouseDevice::handleRelativeMove(const Vector2& position)
        {
            InputSystem::Event event(InputSystem::Event::Type::MOUSE_RELATIVE_MOVE);
            event.deviceId = id;
            event.position = position;
            return inputSystem.postEvent(event);
        }

        std::future<bool> MouseDevice::handleScroll(const Vector2& scroll, const Vector2& position)
        {
            InputSystem::Event event(InputSystem::Event::Type::MOUSE_SCROLL);
            event.deviceId = id;
            event.position = position;
            event.scroll = scroll;
            return inputSystem.postEvent(event);
        }

        std::future<bool> MouseDevice::handleCursorLockChange(bool locked)
        {
            InputSystem::Event event(InputSystem::Event::Type::MOUSE_LOCK_CHANGED);
            event.deviceId = id;
            event.locked = locked;
            return inputSystem.postEvent(event);
        }
    } // namespace input
} // namespace ouzel
