// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "TouchpadDevice.hpp"
#include "InputSystem.hpp"

namespace ouzel
{
    namespace input
    {
        std::future<bool> TouchpadDevice::handleTouchBegin(uint64_t touchId, const Vector2& position, float force)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::TOUCH_BEGIN;
            event.deviceId = id;
            event.touchId = touchId;
            event.position = position;
            event.force = force;
            return inputSystem.postEvent(event);
        }

        std::future<bool> TouchpadDevice::handleTouchEnd(uint64_t touchId, const Vector2& position, float force)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::TOUCH_END;
            event.deviceId = id;
            event.touchId = touchId;
            event.position = position;
            event.force = force;
            return inputSystem.postEvent(event);
        }

        std::future<bool> TouchpadDevice::handleTouchMove(uint64_t touchId, const Vector2& position, float force)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::TOUCH_MOVE;
            event.deviceId = id;
            event.touchId = touchId;
            event.position = position;
            event.force = force;
            return inputSystem.postEvent(event);
        }

        std::future<bool> TouchpadDevice::handleTouchCancel(uint64_t touchId, const Vector2& position, float force)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::TOUCH_CANCEL;
            event.deviceId = id;
            event.touchId = touchId;
            event.position = position;
            event.force = force;
            return inputSystem.postEvent(event);
        }
    } // namespace input
} // namespace ouzel
