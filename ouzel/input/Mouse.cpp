// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <iterator>
#include "Mouse.hpp"
#include "InputManager.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"

namespace ouzel
{
    namespace input
    {
        Mouse::Mouse(InputManager& initInputManager, uint32_t initDeviceId):
            Controller(initInputManager, Controller::Type::MOUSE, initDeviceId)
        {
            std::fill(std::begin(buttonStates), std::end(buttonStates), false);
        }

        void Mouse::setPosition(const Vector2& newPosition)
        {
            position = newPosition;

            InputSystem::Command command;
            command.type = InputSystem::Command::Type::SET_POSITION;
            command.deviceId = deviceId;
            command.position = position;
            inputManager.getInputSystem()->addCommand(command);
        }

        void Mouse::setCursorVisible(bool visible)
        {
            cursorVisible = visible;

            InputSystem::Command command;
            command.type = InputSystem::Command::Type::SET_CURSOR_VISIBLE;
            command.deviceId = deviceId;
            command.visible = cursorVisible;
            inputManager.getInputSystem()->addCommand(command);
        }

        void Mouse::setCursorLocked(bool locked)
        {
            cursorLocked = locked;

            InputSystem::Command command;
            command.type = InputSystem::Command::Type::SET_CURSOR_LOCKED;
            command.deviceId = deviceId;
            command.locked = cursorLocked;
            inputManager.getInputSystem()->addCommand(command);
        }

        void Mouse::setCursor(const Cursor* newCursor)
        {
            cursor = newCursor;
        }

        bool Mouse::handleButtonPress(Mouse::Button button, const Vector2& pos)
        {
            buttonStates[static_cast<uint32_t>(button)] = true;

            Event event;
            event.type = Event::Type::MOUSE_PRESS;
            event.mouseEvent.mouse = this;
            event.mouseEvent.button = button;
            event.mouseEvent.position = pos;

            return engine->getEventDispatcher().dispatchEvent(event);
        }

        bool Mouse::handleButtonRelease(Mouse::Button button, const Vector2& pos)
        {
            buttonStates[static_cast<uint32_t>(button)] = false;

            Event event;
            event.type = Event::Type::MOUSE_RELEASE;
            event.mouseEvent.mouse = this;
            event.mouseEvent.button = button;
            event.mouseEvent.position = pos;

            return engine->getEventDispatcher().dispatchEvent(event);
        }

        bool Mouse::handleMove(const Vector2& pos)
        {
            Event event;
            event.type = Event::Type::MOUSE_MOVE;
            event.mouseEvent.mouse = this;
            event.mouseEvent.difference = pos - position;
            event.mouseEvent.position = pos;

            position = pos;

            return engine->getEventDispatcher().dispatchEvent(event);
        }

        bool Mouse::handleRelativeMove(const Vector2& pos)
        {
            Vector2 newPosition = position + pos;

            newPosition.x = clamp(newPosition.x, 0.0F, 1.0F);
            newPosition.y = clamp(newPosition.y, 0.0F, 1.0F);

            return handleMove(newPosition);
        }

        bool Mouse::handleScroll(const Vector2& scroll, const Vector2& pos)
        {
            Event event;
            event.type = Event::Type::MOUSE_SCROLL;
            event.mouseEvent.mouse = this;
            event.mouseEvent.position = pos;
            event.mouseEvent.scroll = scroll;

            return engine->getEventDispatcher().dispatchEvent(event);
        }

        bool Mouse::handleCursorLockChange(bool locked)
        {
            cursorLocked = locked;

            Event event;
            event.type = Event::Type::MOUSE_CURSOR_LOCK_CHANGE;
            event.mouseEvent.mouse = this;
            event.mouseEvent.locked = cursorLocked;

            return engine->getEventDispatcher().dispatchEvent(event);
        }
    } // namespace input
} // namespace ouzel
