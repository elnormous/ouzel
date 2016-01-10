// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "GamepadWin.h"
#include "Engine.h"
#include "Utils.h"

namespace ouzel
{
    const int32_t MAX_THUMB_VALUE = 32767;
    const int32_t MIN_THUMB_VALUE = -32768;

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
                handleButtonValueChange(GamepadButton::LEFT_TRIGGER,
                                        state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD,
                                        static_cast<float>(state.Gamepad.bLeftTrigger) / 255.0f);
            }

            if (state.Gamepad.bRightTrigger != _state.Gamepad.bRightTrigger)
            {
                handleButtonValueChange(GamepadButton::RIGHT_TRIGGER,
                                        state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD,
                                        static_cast<float>(state.Gamepad.bRightTrigger) / 255.0f);
            }

            if (state.Gamepad.sThumbLX != _state.Gamepad.sThumbLX)
            {
                if (state.Gamepad.sThumbLX > 0)
                {
                    handleButtonValueChange(GamepadButton::LEFT_THUMB_RIGHT,
                                            state.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE,
                                            static_cast<float>(state.Gamepad.sThumbLX) / static_cast<float>(MAX_THUMB_VALUE));
                }
                else if (state.Gamepad.sThumbLX < 0)
                {
                    handleButtonValueChange(GamepadButton::LEFT_THUMB_LEFT,
                                            state.Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE,
                                            static_cast<float>(state.Gamepad.sThumbLX) / static_cast<float>(MIN_THUMB_VALUE));
                }
                else // thumbstick is 0
                {
                    if (_state.Gamepad.sThumbLX > state.Gamepad.sThumbLX)
                    {
                        handleButtonValueChange(GamepadButton::LEFT_THUMB_RIGHT, false, 0.0f);
                    }
                    else
                    {
                        handleButtonValueChange(GamepadButton::LEFT_THUMB_LEFT, false, 0.0f);
                    }
                }
            }

            if (state.Gamepad.sThumbLY != _state.Gamepad.sThumbLY)
            {
                if (state.Gamepad.sThumbLY > 0)
                {
                    handleButtonValueChange(GamepadButton::LEFT_THUMB_UP,
                                            state.Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE,
                                            static_cast<float>(state.Gamepad.sThumbLY) / static_cast<float>(MAX_THUMB_VALUE));
                }
                else if (state.Gamepad.sThumbLY < 0)
                {
                    handleButtonValueChange(GamepadButton::LEFT_THUMB_DOWN,
                                            state.Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE,
                                            static_cast<float>(state.Gamepad.sThumbLY) / static_cast<float>(MIN_THUMB_VALUE));
                }
                else // thumbstick is 0
                {
                    if (_state.Gamepad.sThumbLY > state.Gamepad.sThumbLY)
                    {
                        handleButtonValueChange(GamepadButton::LEFT_THUMB_UP, false, 0.0f);
                    }
                    else
                    {
                        handleButtonValueChange(GamepadButton::LEFT_THUMB_DOWN, false, 0.0f);
                    }
                }
            }

            if (state.Gamepad.sThumbRX != _state.Gamepad.sThumbRX)
            {
                if (state.Gamepad.sThumbRX > 0)
                {
                    handleButtonValueChange(GamepadButton::RIGHT_THUMB_RIGHT,
                                            state.Gamepad.sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
                                            static_cast<float>(state.Gamepad.sThumbRX) / static_cast<float>(MAX_THUMB_VALUE));
                }
                else if (state.Gamepad.sThumbRX < 0)
                {
                    handleButtonValueChange(GamepadButton::RIGHT_THUMB_LEFT,
                                            state.Gamepad.sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
                                            static_cast<float>(state.Gamepad.sThumbRX) / static_cast<float>(MIN_THUMB_VALUE));
                }
                else // thumbstick is 0
                {
                    if (_state.Gamepad.sThumbRX > state.Gamepad.sThumbRX)
                    {
                        handleButtonValueChange(GamepadButton::RIGHT_THUMB_RIGHT, false, 0.0f);
                    }
                    else
                    {
                        handleButtonValueChange(GamepadButton::RIGHT_THUMB_LEFT, false, 0.0f);
                    }
                }
            }

            if (state.Gamepad.sThumbRY != _state.Gamepad.sThumbRY)
            {
                if (state.Gamepad.sThumbRY > 0)
                {
                    handleButtonValueChange(GamepadButton::RIGHT_THUMB_UP,
                                            state.Gamepad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
                                            static_cast<float>(state.Gamepad.sThumbRY) / static_cast<float>(MAX_THUMB_VALUE));
                }
                else if (state.Gamepad.sThumbRY < 0)
                {
                    handleButtonValueChange(GamepadButton::RIGHT_THUMB_DOWN,
                                            state.Gamepad.sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE,
                                            static_cast<float>(state.Gamepad.sThumbRY) / static_cast<float>(MIN_THUMB_VALUE));
                }
                else // thumbstick is 0
                {
                    if (_state.Gamepad.sThumbRY > state.Gamepad.sThumbRY)
                    {
                        handleButtonValueChange(GamepadButton::RIGHT_THUMB_UP, false, 0.0f);
                    }
                    else
                    {
                        handleButtonValueChange(GamepadButton::RIGHT_THUMB_DOWN, false, 0.0f);
                    }
                }
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
