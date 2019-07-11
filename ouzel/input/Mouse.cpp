// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <iterator>
#include "Mouse.hpp"
#include "InputManager.hpp"
#include "Cursor.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"

namespace ouzel
{
    namespace input
    {
        Mouse::Mouse(InputManager& initInputManager, uint32_t initDeviceId):
            Controller(initInputManager, Controller::Type::MOUSE, initDeviceId)
        {
        }

        void Mouse::setPosition(const Vector2F& newPosition)
        {
            position = newPosition;

            InputSystem::Command command(InputSystem::Command::Type::SetPosition);
            command.deviceId = deviceId;
            command.position = position;
            inputManager.getInputSystem()->addCommand(command);
        }

        void Mouse::setCursorVisible(bool visible)
        {
            cursorVisible = visible;

            InputSystem::Command command(InputSystem::Command::Type::SetCursorVisible);
            command.deviceId = deviceId;
            command.visible = cursorVisible;
            inputManager.getInputSystem()->addCommand(command);
        }

        void Mouse::setCursorLocked(bool locked)
        {
            cursorLocked = locked;

            InputSystem::Command command(InputSystem::Command::Type::SetCursorLocked);
            command.deviceId = deviceId;
            command.locked = cursorLocked;
            inputManager.getInputSystem()->addCommand(command);
        }

        void Mouse::setCursor(const Cursor* newCursor)
        {
            cursor = newCursor;

            InputSystem::Command command(InputSystem::Command::Type::SetCursor);
            command.deviceId = deviceId;
            command.cursorResource = cursor->getCursorResource();
            inputManager.getInputSystem()->addCommand(command);
        }

        bool Mouse::handleButtonPress(Mouse::Button button, const Vector2F& pos)
        {
            buttonStates[static_cast<uint32_t>(button)] = true;

            auto event = std::make_unique<MouseEvent>();
            event->type = Event::Type::MOUSE_PRESS;
            event->mouse = this;
            event->button = button;
            event->position = pos;

            return engine->getEventDispatcher().dispatchEvent(std::move(event));
        }

        bool Mouse::handleButtonRelease(Mouse::Button button, const Vector2F& pos)
        {
            buttonStates[static_cast<uint32_t>(button)] = false;

            auto event = std::make_unique<MouseEvent>();
            event->type = Event::Type::MOUSE_RELEASE;
            event->mouse = this;
            event->button = button;
            event->position = pos;

            return engine->getEventDispatcher().dispatchEvent(std::move(event));
        }

        bool Mouse::handleMove(const Vector2F& pos)
        {
            auto event = std::make_unique<MouseEvent>();
            event->type = Event::Type::MOUSE_MOVE;
            event->mouse = this;
            event->difference = pos - position;
            event->position = pos;

            position = pos;

            return engine->getEventDispatcher().dispatchEvent(std::move(event));
        }

        bool Mouse::handleRelativeMove(const Vector2F& pos)
        {
            Vector2F newPosition = position + pos;

            newPosition.v[0] = clamp(newPosition.v[0], 0.0F, 1.0F);
            newPosition.v[1] = clamp(newPosition.v[1], 0.0F, 1.0F);

            return handleMove(newPosition);
        }

        bool Mouse::handleScroll(const Vector2F& scroll, const Vector2F& pos)
        {
            auto event = std::make_unique<MouseEvent>();
            event->type = Event::Type::MOUSE_SCROLL;
            event->mouse = this;
            event->position = pos;
            event->scroll = scroll;

            return engine->getEventDispatcher().dispatchEvent(std::move(event));
        }

        bool Mouse::handleCursorLockChange(bool locked)
        {
            cursorLocked = locked;

            auto event = std::make_unique<MouseEvent>();
            event->type = Event::Type::MOUSE_CURSOR_LOCK_CHANGE;
            event->mouse = this;
            event->locked = cursorLocked;

            return engine->getEventDispatcher().dispatchEvent(std::move(event));
        }
    } // namespace input
} // namespace ouzel
