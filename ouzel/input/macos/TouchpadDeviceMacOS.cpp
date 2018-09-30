// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "TouchpadDeviceMacOS.hpp"
#include "InputSystemMacOS.hpp"

namespace ouzel
{
    namespace input
    {
        void TouchpadDeviceMacOS::handleTouchBegin(uint64_t touchId, const Vector2& position, float force)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::TOUCH_BEGIN;
            event.deviceId = id;
            event.touchId = touchId;
            event.position = position;
            event.force = force;
            inputSystemMacOS.addEvent(event);
        }

        void TouchpadDeviceMacOS::handleTouchEnd(uint64_t touchId, const Vector2& position, float force)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::TOUCH_END;
            event.deviceId = id;
            event.touchId = touchId;
            event.position = position;
            event.force = force;
            inputSystemMacOS.addEvent(event);
        }

        void TouchpadDeviceMacOS::handleTouchMove(uint64_t touchId, const Vector2& position, float force)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::TOUCH_MOVE;
            event.deviceId = id;
            event.touchId = touchId;
            event.position = position;
            event.force = force;
            inputSystemMacOS.addEvent(event);
        }

        void TouchpadDeviceMacOS::handleTouchCancel(uint64_t touchId, const Vector2& position, float force)
        {
            InputSystem::Event event;
            event.type = InputSystem::Event::Type::TOUCH_CANCEL;
            event.deviceId = id;
            event.touchId = touchId;
            event.position = position;
            event.force = force;
            inputSystemMacOS.addEvent(event);
        }
    } // namespace input
} // namespace ouzel
