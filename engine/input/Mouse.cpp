// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <iterator>
#include "Mouse.hpp"
#include "InputManager.hpp"
#include "Cursor.hpp"
#include "../core/Engine.hpp"
#include "../events/EventDispatcher.hpp"

namespace ouzel::input
{
    Mouse::Mouse(InputManager& initInputManager, DeviceId initDeviceId):
        Controller(initInputManager, Controller::Type::mouse, initDeviceId)
    {
    }

    void Mouse::setPosition(const Vector2F& newPosition)
    {
        position = newPosition;

        InputSystem::Command command(InputSystem::Command::Type::setPosition);
        command.deviceId = deviceId;
        command.position = position;
        inputManager.getInputSystem()->addCommand(command);
    }

    void Mouse::setCursorVisible(bool visible)
    {
        cursorVisible = visible;

        InputSystem::Command command(InputSystem::Command::Type::setCursorVisible);
        command.deviceId = deviceId;
        command.visible = cursorVisible;
        inputManager.getInputSystem()->addCommand(command);
    }

    void Mouse::setCursorLocked(bool locked)
    {
        cursorLocked = locked;

        InputSystem::Command command(InputSystem::Command::Type::setCursorLocked);
        command.deviceId = deviceId;
        command.locked = cursorLocked;
        inputManager.getInputSystem()->addCommand(command);
    }

    void Mouse::setCursor(const Cursor* newCursor)
    {
        cursor = newCursor;

        InputSystem::Command command(InputSystem::Command::Type::setCursor);
        command.deviceId = deviceId;
        command.cursorResource = cursor->getCursorResource();
        inputManager.getInputSystem()->addCommand(command);
    }

    bool Mouse::handleButtonPress(Mouse::Button button, const Vector2F& pos)
    {
        buttonStates[static_cast<std::uint32_t>(button)] = true;

        auto event = std::make_unique<MouseEvent>();
        event->type = Event::Type::mousePress;
        event->mouse = this;
        event->button = button;
        event->position = pos;

        return engine->getEventDispatcher().dispatchEvent(std::move(event));
    }

    bool Mouse::handleButtonRelease(Mouse::Button button, const Vector2F& pos)
    {
        buttonStates[static_cast<std::uint32_t>(button)] = false;

        auto event = std::make_unique<MouseEvent>();
        event->type = Event::Type::mouseRelease;
        event->mouse = this;
        event->button = button;
        event->position = pos;

        return engine->getEventDispatcher().dispatchEvent(std::move(event));
    }

    bool Mouse::handleMove(const Vector2F& pos)
    {
        auto event = std::make_unique<MouseEvent>();
        event->type = Event::Type::mouseMove;
        event->mouse = this;
        event->difference = pos - position;
        event->position = pos;

        position = pos;

        return engine->getEventDispatcher().dispatchEvent(std::move(event));
    }

    bool Mouse::handleRelativeMove(const Vector2F& pos)
    {
        Vector2F newPosition = position + pos;

        newPosition.v[0] = std::clamp(newPosition.v[0], 0.0F, 1.0F);
        newPosition.v[1] = std::clamp(newPosition.v[1], 0.0F, 1.0F);

        return handleMove(newPosition);
    }

    bool Mouse::handleScroll(const Vector2F& scroll, const Vector2F& pos)
    {
        auto event = std::make_unique<MouseEvent>();
        event->type = Event::Type::mouseScroll;
        event->mouse = this;
        event->position = pos;
        event->scroll = scroll;

        return engine->getEventDispatcher().dispatchEvent(std::move(event));
    }

    bool Mouse::handleCursorLockChange(bool locked)
    {
        cursorLocked = locked;

        auto event = std::make_unique<MouseEvent>();
        event->type = Event::Type::mouseCursorLockChange;
        event->mouse = this;
        event->locked = cursorLocked;

        return engine->getEventDispatcher().dispatchEvent(std::move(event));
    }
}
