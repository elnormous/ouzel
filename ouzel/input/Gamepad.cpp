// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstring>
#include "Gamepad.h"
#include "core/Engine.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace input
    {
        Gamepad::Gamepad()
        {
            memset(buttonStates, 0, sizeof(buttonStates));
        }

        bool Gamepad::isAttached() const
        {
            return false;
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

        bool Gamepad::setPlayerIndex(int32_t)
        {
            return false;
        }

        const GamepadButtonState& Gamepad::getButtonState(GamepadButton button) const
        {
            return buttonStates[static_cast<uint32_t>(button)];
        }

        void Gamepad::handleButtonValueChange(GamepadButton button, bool pressed, float value)
        {
            Event event;
            event.sender = shared_from_this();
            event.type = Event::Type::GAMEPAD_BUTTON_CHANGE;

            event.gamepadEvent.gamepad = shared_from_this();
            event.gamepadEvent.button = button;
            event.gamepadEvent.pressed = pressed;
            event.gamepadEvent.value = value;

            sharedEngine->getEventDispatcher()->dispatchEvent(event);

            buttonStates[static_cast<uint32_t>(button)].pressed = pressed;
            buttonStates[static_cast<uint32_t>(button)].value = value;
        }

        void Gamepad::setVibration(Motor, float)
        {
        }

        float Gamepad::getVibration(Motor)
        {
            return 0.0f;
        }
    } // namespace input
} // namespace ouzel
