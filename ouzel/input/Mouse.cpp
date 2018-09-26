// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <iterator>
#include "Mouse.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"

namespace ouzel
{
    namespace input
    {
        Mouse::Mouse():
            InputDevice(InputDevice::Type::MOUSE)
        {
            std::fill(std::begin(buttonStates), std::end(buttonStates), false);
        }

        void Mouse::setPosition(const Vector2& newPosition)
        {
            position = newPosition;
            // TODO: send command to InputSystem
        }

        void Mouse::handleMouseButtonPress(Mouse::Button button, const Vector2& pos, uint32_t modifiers)
        {
            buttonStates[static_cast<uint32_t>(button)] = true;

            Event event;
            event.type = Event::Type::MOUSE_PRESS;

            event.mouseEvent.button = button;
            event.mouseEvent.position = pos;
            event.mouseEvent.modifiers = modifiers;

            engine->getEventDispatcher().postEvent(event);
        }

        void Mouse::handleMouseButtonRelease(Mouse::Button button, const Vector2& pos, uint32_t modifiers)
        {
            buttonStates[static_cast<uint32_t>(button)] = false;

            Event event;
            event.type = Event::Type::MOUSE_RELEASE;

            event.mouseEvent.button = button;
            event.mouseEvent.position = pos;
            event.mouseEvent.modifiers = modifiers;

            engine->getEventDispatcher().postEvent(event);
        }

        void Mouse::handleMouseMove(const Vector2& pos, uint32_t modifiers)
        {
            Event event;
            event.type = Event::Type::MOUSE_MOVE;

            event.mouseEvent.difference = pos - position;
            event.mouseEvent.position = pos;
            event.mouseEvent.modifiers = modifiers;

            position = pos;

            engine->getEventDispatcher().postEvent(event);
        }

        void Mouse::handleMouseRelativeMove(const Vector2& pos, uint32_t modifiers)
        {
            Vector2 newPosition = position + pos;

            newPosition.x = clamp(newPosition.x, 0.0F, 1.0F);
            newPosition.y = clamp(newPosition.y, 0.0F, 1.0F);

            handleMouseMove(newPosition, modifiers);
        }

        void Mouse::handleMouseScroll(const Vector2& scroll, const Vector2& pos, uint32_t modifiers)
        {
            Event event;
            event.type = Event::Type::MOUSE_SCROLL;

            event.mouseEvent.position = pos;
            event.mouseEvent.scroll = scroll;
            event.mouseEvent.modifiers = modifiers;

            engine->getEventDispatcher().postEvent(event);
        }
    } // namespace input
} // namespace ouzel
