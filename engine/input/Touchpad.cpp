// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "Touchpad.hpp"
#include "../core/Engine.hpp"
#include "../events/EventDispatcher.hpp"

namespace ouzel
{
    namespace input
    {
        Touchpad::Touchpad(InputManager& initInputManager, DeviceId initDeviceId, bool initScreen):
            Controller(initInputManager, Controller::Type::Touchpad, initDeviceId),
            screen(initScreen)
        {
        }

        bool Touchpad::handleTouchBegin(std::uint64_t touchId, const Vector2F& position, float force)
        {
            auto event = std::make_unique<TouchEvent>();
            event->type = Event::Type::TouchBegin;
            event->touchpad = this;
            event->touchId = touchId;
            event->position = position;
            event->force = force;

            touchPositions[touchId] = position;

            return engine->getEventDispatcher().dispatchEvent(std::move(event));
        }

        bool Touchpad::handleTouchEnd(std::uint64_t touchId, const Vector2F& position, float force)
        {
            auto event = std::make_unique<TouchEvent>();
            event->type = Event::Type::TouchEnd;
            event->touchpad = this;
            event->touchId = touchId;
            event->position = position;
            event->force = force;

            auto i = touchPositions.find(touchId);

            if (i != touchPositions.end())
                touchPositions.erase(i);

            return engine->getEventDispatcher().dispatchEvent(std::move(event));
        }

        bool Touchpad::handleTouchMove(std::uint64_t touchId, const Vector2F& position, float force)
        {
            auto event = std::make_unique<TouchEvent>();
            event->type = Event::Type::TouchMove;
            event->touchpad = this;
            event->touchId = touchId;
            event->difference = position - touchPositions[touchId];
            event->position = position;
            event->force = force;

            touchPositions[touchId] = position;

            return engine->getEventDispatcher().dispatchEvent(std::move(event));
        }

        bool Touchpad::handleTouchCancel(std::uint64_t touchId, const Vector2F& position, float force)
        {
            auto event = std::make_unique<TouchEvent>();
            event->type = Event::Type::TouchCancel;
            event->touchpad = this;
            event->touchId = touchId;
            event->position = position;
            event->force = force;

            auto i = touchPositions.find(touchId);

            if (i != touchPositions.end())
                touchPositions.erase(i);

            return engine->getEventDispatcher().dispatchEvent(std::move(event));
        }
    } // namespace input
} // namespace ouzel
