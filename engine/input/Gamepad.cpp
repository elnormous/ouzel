// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <iterator>
#include "Gamepad.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"

namespace ouzel
{
    namespace input
    {
        Gamepad::Gamepad(InputManager& initInputManager, DeviceId initDeviceId):
            Controller(initInputManager, Controller::Type::Gamepad, initDeviceId)
        {
        }

        void Gamepad::setAbsoluteDpadValues(bool newAbsoluteDpadValues)
        {
            absoluteDpadValues = newAbsoluteDpadValues;

            InputSystem::Command command(InputSystem::Command::Type::SetAbsoluteDpad_VALUES);
            command.deviceId = deviceId;
            command.absoluteDpadValues = absoluteDpadValues;
            inputManager.getInputSystem()->addCommand(command);
        }

        void Gamepad::setPlayerIndex(int32_t newPlayerIndex)
        {
            playerIndex = newPlayerIndex;

            InputSystem::Command command(InputSystem::Command::Type::SetPlayerIndex);
            command.deviceId = deviceId;
            command.playerIndex = playerIndex;
            inputManager.getInputSystem()->addCommand(command);
        }

        bool Gamepad::handleButtonValueChange(Gamepad::Button button, bool pressed, float value)
        {
            auto event = std::make_unique<GamepadEvent>();
            event->type = Event::Type::GamepadButtonChange;
            event->gamepad = this;
            event->button = button;
            event->previousPressed = buttonStates[static_cast<uint32_t>(button)].pressed;
            event->pressed = pressed;
            event->value = value;
            event->previousValue = buttonStates[static_cast<uint32_t>(button)].value;

            buttonStates[static_cast<uint32_t>(button)].pressed = pressed;
            buttonStates[static_cast<uint32_t>(button)].value = value;

            return engine->getEventDispatcher().dispatchEvent(std::move(event));
        }

        void Gamepad::setVibration(Motor motor, float speed)
        {
            vibration[static_cast<uint32_t>(motor)] = speed;

            InputSystem::Command command(InputSystem::Command::Type::SetVibration);
            command.deviceId = deviceId;
            command.motor = motor;
            command.speed = speed;
            inputManager.getInputSystem()->addCommand(command);
        }
    } // namespace input
} // namespace ouzel
