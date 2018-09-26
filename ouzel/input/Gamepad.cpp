// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <iterator>
#include "Gamepad.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"

namespace ouzel
{
    namespace input
    {
        Gamepad::Gamepad(uint32_t initDeviceId):
            Controller(Controller::Type::GAMEPAD, initDeviceId)
        {
            std::fill(std::begin(vibration), std::end(vibration), 0.0F);
        }

        void Gamepad::setAbsoluteDPadValues(bool newAbsoluteDPadValues)
        {
            absoluteDPadValues = newAbsoluteDPadValues;
            // TODO: send command to InputSystem
        }

        void Gamepad::setPlayerIndex(int32_t newPlayerIndex)
        {
            playerIndex = newPlayerIndex;
            // TODO: send command to InputSystem
        }

        void Gamepad::handleButtonValueChange(Gamepad::Button button, bool pressed, float value)
        {
            Event event;
            event.type = Event::Type::GAMEPAD_BUTTON_CHANGE;

            event.gamepadEvent.gamepad = this;
            event.gamepadEvent.button = button;
            event.gamepadEvent.previousPressed = buttonStates[static_cast<uint32_t>(button)].pressed;
            event.gamepadEvent.pressed = pressed;
            event.gamepadEvent.value = value;
            event.gamepadEvent.previousValue = buttonStates[static_cast<uint32_t>(button)].value;

            buttonStates[static_cast<uint32_t>(button)].pressed = pressed;
            buttonStates[static_cast<uint32_t>(button)].value = value;

            engine->getEventDispatcher().postEvent(event, true);
        }

        void Gamepad::setVibration(Motor motor, float speed)
        {
            vibration[static_cast<uint32_t>(motor)] = speed;
            // TODO: send command to InputSystem
        }
    } // namespace input
} // namespace ouzel
