// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Touchpad.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"

namespace ouzel
{
    namespace input
    {
        Touchpad::Touchpad(InputManager& initInputManager, uint32_t initDeviceId, bool initScreen):
            Controller(initInputManager, Controller::Type::TOUCHPAD, initDeviceId),
            screen(initScreen)
        {
        }

        bool Touchpad::handleTouchBegin(uint64_t touchId, const Vector2F& position, float force)
        {
            auto event = std::make_unique<TouchEvent>();
            event->type = Event::Type::TOUCH_BEGIN;
            event->touchpad = this;
            event->touchId = touchId;
            event->position = position;
            event->force = force;

            touchPositions[touchId] = position;

            return engine->getEventDispatcher().dispatchEvent(std::move(event));
        }

        bool Touchpad::handleTouchEnd(uint64_t touchId, const Vector2F& position, float force)
        {
            auto event = std::make_unique<TouchEvent>();
            event->type = Event::Type::TOUCH_END;
            event->touchpad = this;
            event->touchId = touchId;
            event->position = position;
            event->force = force;

            auto i = touchPositions.find(touchId);

            if (i != touchPositions.end())
                touchPositions.erase(i);

            return engine->getEventDispatcher().dispatchEvent(std::move(event));
        }

        bool Touchpad::handleTouchMove(uint64_t touchId, const Vector2F& position, float force)
        {
            auto event = std::make_unique<TouchEvent>();
            event->type = Event::Type::TOUCH_MOVE;
            event->touchpad = this;
            event->touchId = touchId;
            event->difference = position - touchPositions[touchId];
            event->position = position;
            event->force = force;

            touchPositions[touchId] = position;

            return engine->getEventDispatcher().dispatchEvent(std::move(event));
        }

        bool Touchpad::handleTouchCancel(uint64_t touchId, const Vector2F& position, float force)
        {
            auto event = std::make_unique<TouchEvent>();
            event->type = Event::Type::TOUCH_CANCEL;
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
