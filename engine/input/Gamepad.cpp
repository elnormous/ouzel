// Ouzel by Elviss Strazdins

#include <iterator>
#include "Gamepad.hpp"
#include "../core/Engine.hpp"
#include "../events/EventDispatcher.hpp"

namespace ouzel::input
{
    Gamepad::Gamepad(InputManager& initInputManager, DeviceId initDeviceId):
        Controller{initInputManager, Controller::Type::gamepad, initDeviceId}
    {
    }

    void Gamepad::setAbsoluteDpadValues(bool newAbsoluteDpadValues)
    {
        absoluteDpadValues = newAbsoluteDpadValues;

        InputSystem::Command command(InputSystem::Command::Type::setAbsoluteDpadValues);
        command.deviceId = deviceId;
        command.absoluteDpadValues = absoluteDpadValues;
        inputManager.getInputSystem().addCommand(command);
    }

    void Gamepad::setPlayerIndex(std::int32_t newPlayerIndex)
    {
        playerIndex = newPlayerIndex;

        InputSystem::Command command(InputSystem::Command::Type::setPlayerIndex);
        command.deviceId = deviceId;
        command.playerIndex = playerIndex;
        inputManager.getInputSystem().addCommand(command);
    }

    bool Gamepad::handleButtonValueChange(Gamepad::Button button, bool pressed, float value)
    {
        auto event = std::make_unique<GamepadEvent>();
        event->type = Event::Type::gamepadButtonChange;
        event->gamepad = this;
        event->button = button;
        event->previousPressed = buttonStates[static_cast<std::uint32_t>(button)].pressed;
        event->pressed = pressed;
        event->value = value;
        event->previousValue = buttonStates[static_cast<std::uint32_t>(button)].value;

        buttonStates[static_cast<std::uint32_t>(button)].pressed = pressed;
        buttonStates[static_cast<std::uint32_t>(button)].value = value;

        return engine->getEventDispatcher().dispatchEvent(std::move(event));
    }

    void Gamepad::setVibration(Motor motor, float speed)
    {
        vibration[static_cast<std::uint32_t>(motor)] = speed;

        InputSystem::Command command(InputSystem::Command::Type::setVibration);
        command.deviceId = deviceId;
        command.motor = motor;
        command.speed = speed;
        inputManager.getInputSystem().addCommand(command);
    }
}
