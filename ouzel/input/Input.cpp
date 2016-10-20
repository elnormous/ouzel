// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <algorithm>
#include "Input.h"
#include "core/Engine.h"
#include "events/EventDispatcher.h"
#include "math/MathUtils.h"

namespace ouzel
{
    namespace input
    {
        Input::Input()
        {
            std::fill(std::begin(keyboardKeyStates), std::end(keyboardKeyStates), false);
            std::fill(std::begin(mouseButtonStates), std::end(mouseButtonStates), false);
        }

        Input::~Input()
        {
        }

        void Input::setCursorVisible(bool)
        {
        }

        bool Input::isCursorVisible() const
        {
            return false;
        }

        void Input::startGamepadDiscovery()
        {
        }

        void Input::stopGamepadDiscovery()
        {
        }

        void Input::keyDown(KeyboardKey key, uint32_t modifiers)
        {
            Event event;

            event.keyboardEvent.key = key;
            event.keyboardEvent.modifiers = modifiers;

            if (!keyboardKeyStates[static_cast<uint32_t>(key)])
            {
                keyboardKeyStates[static_cast<uint32_t>(key)] = true;

                event.type = Event::Type::KEY_DOWN;
                sharedEngine->getEventDispatcher()->postEvent(event);
            }
            else
            {
                event.type = Event::Type::KEY_REPEAT;
                sharedEngine->getEventDispatcher()->postEvent(event);
            }
        }

        void Input::keyUp(KeyboardKey key, uint32_t modifiers)
        {
            keyboardKeyStates[static_cast<uint32_t>(key)] = false;

            Event event;
            event.type = Event::Type::KEY_UP;

            event.keyboardEvent.key = key;
            event.keyboardEvent.modifiers = modifiers;

            sharedEngine->getEventDispatcher()->postEvent(event);
        }

        void Input::mouseDown(MouseButton button, const Vector2& position, uint32_t modifiers)
        {
            mouseButtonStates[static_cast<uint32_t>(button)] = true;

            Event event;
            event.type = Event::Type::MOUSE_DOWN;

            event.mouseEvent.button = button;
            event.mouseEvent.position = position;
            event.mouseEvent.modifiers = modifiers;

            sharedEngine->getEventDispatcher()->postEvent(event);
        }

        void Input::mouseUp(MouseButton button, const Vector2& position, uint32_t modifiers)
        {
            mouseButtonStates[static_cast<uint32_t>(button)] = false;

            Event event;
            event.type = Event::Type::MOUSE_UP;

            event.mouseEvent.button = button;
            event.mouseEvent.position = position;
            event.mouseEvent.modifiers = modifiers;

            sharedEngine->getEventDispatcher()->postEvent(event);
        }

        void Input::mouseMove(const Vector2& position, uint32_t modifiers)
        {
            cursorPosition = position;

            Event event;
            event.type = Event::Type::MOUSE_MOVE;

            event.mouseEvent.position = position;
            event.mouseEvent.modifiers = modifiers;

            sharedEngine->getEventDispatcher()->postEvent(event);
        }

        void Input::mouseRelativeMove(const Vector2& relativePosition, uint32_t modifiers)
        {
            Vector2 newPosition = cursorPosition + relativePosition;

            newPosition.x = clamp(newPosition.x, 0.0f, 1.0f);
            newPosition.y = clamp(newPosition.y, 0.0f, 1.0f);

            mouseMove(newPosition, modifiers);
        }

        void Input::mouseScroll(const Vector2& scroll, const Vector2& position, uint32_t modifiers)
        {
            Event event;
            event.type = Event::Type::MOUSE_SCROLL;

            event.mouseEvent.position = position;
            event.mouseEvent.scroll = scroll;
            event.mouseEvent.modifiers = modifiers;

            sharedEngine->getEventDispatcher()->postEvent(event);
        }

        void Input::touchBegin(uint64_t touchId, const Vector2& position)
        {
            Event event;
            event.type = Event::Type::TOUCH_BEGIN;

            event.touchEvent.touchId = touchId;
            event.touchEvent.position = position;

            sharedEngine->getEventDispatcher()->postEvent(event);
        }

        void Input::touchEnd(uint64_t touchId, const Vector2& position)
        {
            Event event;
            event.type = Event::Type::TOUCH_END;

            event.touchEvent.touchId = touchId;
            event.touchEvent.position = position;

            sharedEngine->getEventDispatcher()->postEvent(event);
        }

        void Input::touchMove(uint64_t touchId, const Vector2& position)
        {
            Event event;
            event.type = Event::Type::TOUCH_MOVE;

            event.touchEvent.touchId = touchId;
            event.touchEvent.position = position;

            sharedEngine->getEventDispatcher()->postEvent(event);
        }

        void Input::touchCancel(uint64_t touchId, const Vector2& position)
        {
            Event event;
            event.type = Event::Type::TOUCH_CANCEL;

            event.touchEvent.touchId = touchId;
            event.touchEvent.position = position;

            sharedEngine->getEventDispatcher()->postEvent(event);
        }

        bool Input::showVirtualKeyboard()
        {
            return false;
        }

        bool Input::hideVirtualKeyboard()
        {
            return false;
        }
    } // namespace input
} // namespace ouzel
