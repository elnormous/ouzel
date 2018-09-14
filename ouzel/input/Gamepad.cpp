// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "Gamepad.hpp"
#include "core/Engine.hpp"
#include "events/EventDispatcher.hpp"

namespace ouzel
{
    namespace input
    {
        Gamepad::Gamepad():
            InputDevice(InputDevice::Type::GAMEPAD)
        {
        }

        Gamepad::~Gamepad()
        {
        }

        void Gamepad::setAbsoluteDpadValues(bool)
        {
        }

        bool Gamepad::isAbsoluteDpadValues() const
        {
            return false;
        }

        int32_t Gamepad::getPlayerIndex() const
        {
            return -1;
        }

        void Gamepad::setPlayerIndex(int32_t)
        {
        }

        const Gamepad::ButtonState& Gamepad::getButtonState(GamepadButton button) const
        {
            return buttonStates[static_cast<uint32_t>(button)];
        }

        void Gamepad::handleButtonValueChange(GamepadButton button, bool pressed, float value)
        {
            Event event;
            event.type = Event::Type::GAMEPAD_BUTTON_CHANGE;

            event.gamepadEvent.gamepad = this;
            event.gamepadEvent.button = button;
            event.gamepadEvent.previousPressed = buttonStates[static_cast<uint32_t>(button)].pressed;
            event.gamepadEvent.pressed = pressed;
            event.gamepadEvent.value = value;
            event.gamepadEvent.previousValue = buttonStates[static_cast<uint32_t>(button)].value;

            engine->getEventDispatcher().postEvent(event);

            buttonStates[static_cast<uint32_t>(button)].pressed = pressed;
            buttonStates[static_cast<uint32_t>(button)].value = value;
        }

        void Gamepad::setVibration(Motor, float)
        {
        }

        float Gamepad::getVibration(Motor)
        {
            return 0.0F;
        }
    } // namespace input
} // namespace ouzel
