// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstring>
#include "Gamepad.h"
#include "Engine.h"
#include "Utils.h"

namespace ouzel
{
    namespace input
    {
        Gamepad::Gamepad()
        {
            memset(_buttonStates, 0, sizeof(_buttonStates));
        }

        bool Gamepad::isAttached() const
        {
            return false;
        }

        void Gamepad::setAbsoluteDpadValues(bool absoluteDpadValues)
        {
            OUZEL_UNUSED(absoluteDpadValues);
        }

        bool Gamepad::isAbsoluteDpadValues() const
        {
            return false;
        }

        int32_t Gamepad::getPlayerIndex() const
        {
            return -1;
        }

        bool Gamepad::setPlayerIndex(int32_t playerIndex)
        {
            OUZEL_UNUSED(playerIndex);

            return false;
        }

        const GamepadButtonState& Gamepad::getButtonState(GamepadButton button) const
        {
            return _buttonStates[static_cast<uint32_t>(button)];
        }

        void Gamepad::handleButtonValueChange(GamepadButton button, bool pressed, float value)
        {
            GamepadEventPtr event = std::make_shared<GamepadEvent>();
            event->type = Event::Type::GAMEPAD_BUTTON_CHANGE;
            event->gamepad = shared_from_this();
            event->button = button;
            event->pressed = pressed;
            event->value = value;

            Engine::getInstance()->getEventDispatcher()->dispatchEvent(event, Engine::getInstance()->getInput());

            _buttonStates[static_cast<uint32_t>(button)].pressed = pressed;
            _buttonStates[static_cast<uint32_t>(button)].value = value;
        }

        void Gamepad::setVibration(Motor motor, float speed)
        {
            OUZEL_UNUSED(motor);
            OUZEL_UNUSED(speed);
        }

        float Gamepad::getVibration(Motor motor)
        {
            OUZEL_UNUSED(motor);

            return 0.0f;
        }
    } // namespace input
} // namespace ouzel
