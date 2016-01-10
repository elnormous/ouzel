// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "GamepadWin.h"
#include "Engine.h"
#include "Utils.h"

namespace ouzel
{
    GamepadWin::GamepadWin(int32_t playerIndex):
        _playerIndex(playerIndex)
    {
        memset(&_state, 0, sizeof(XINPUT_STATE));
    }

    void GamepadWin::update(XINPUT_STATE const& state)
    {
        if (state.dwPacketNumber > _state.dwPacketNumber)
        {
            checkButton(state, XINPUT_GAMEPAD_DPAD_UP, GamepadButton::DPAD_UP);
            checkButton(state, XINPUT_GAMEPAD_DPAD_DOWN, GamepadButton::DPAD_DOWN);
            checkButton(state, XINPUT_GAMEPAD_DPAD_LEFT, GamepadButton::DPAD_LEFT);
            checkButton(state, XINPUT_GAMEPAD_DPAD_RIGHT, GamepadButton::DPAD_RIGHT);
            checkButton(state, XINPUT_GAMEPAD_START, GamepadButton::START);
            checkButton(state, XINPUT_GAMEPAD_BACK, GamepadButton::BACK);
            checkButton(state, XINPUT_GAMEPAD_LEFT_THUMB, GamepadButton::LEFT_THUMB);
            checkButton(state, XINPUT_GAMEPAD_RIGHT_THUMB, GamepadButton::RIGHT_THUMB);
            checkButton(state, XINPUT_GAMEPAD_LEFT_SHOULDER, GamepadButton::LEFT_SHOULDER);
            checkButton(state, XINPUT_GAMEPAD_RIGHT_SHOULDER, GamepadButton::RIGHT_SHOULDER);
            checkButton(state, XINPUT_GAMEPAD_A, GamepadButton::A);
            checkButton(state, XINPUT_GAMEPAD_B, GamepadButton::B);
            checkButton(state, XINPUT_GAMEPAD_X, GamepadButton::X);
            checkButton(state, XINPUT_GAMEPAD_Y, GamepadButton::Y);

            if (state.Gamepad.bLeftTrigger != _state.Gamepad.bLeftTrigger)
            {
                handleButtonValueChange(GamepadButton::LEFT_TRIGGER, state.Gamepad.bLeftTrigger != 0, static_cast<float>(state.Gamepad.bLeftTrigger) / 255.0f);
            }

            if (state.Gamepad.bLeftTrigger != _state.Gamepad.bLeftTrigger)
            {
                handleButtonValueChange(GamepadButton::RIGHT_TRIGGER, state.Gamepad.bRightTrigger != 0, static_cast<float>(state.Gamepad.bRightTrigger) / 255.0f);
            }

            _state = state;
        }
    }
    
    int32_t GamepadWin::getPlayerIndex() const
    {
        return _playerIndex;
    }

    void GamepadWin::checkButton(XINPUT_STATE const& state, WORD mask, GamepadButton button)
    {
        if ((state.Gamepad.wButtons & mask) != (_state.Gamepad.wButtons & mask))
        {
            bool pressed = (state.Gamepad.wButtons & mask);
            handleButtonValueChange(button, pressed, pressed ? 1.0f : 0.0f);
        }
    }

    void GamepadWin::handleButtonValueChange(GamepadButton button, bool pressed, float value)
    {
        GamepadEvent event;
        event.type = Event::Type::GAMEPAD_BUTTON_CHANGE;
        event.gamepad = shared_from_this();
        event.button = button;
        event.pressed = pressed;
        event.value = value;

        Engine::getInstance()->getEventDispatcher()->dispatchGamepadButtonChangeEvent(event, Engine::getInstance()->getInput());
    }
}
