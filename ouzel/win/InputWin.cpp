// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <windows.h>
#include "InputWin.h"
#include "Engine.h"
#include "GamepadWin.h"
#include "Utils.h"

namespace ouzel
{
    InputWin::InputWin()
    {
        
    }
    
    InputWin::~InputWin()
    {
        
    }

    void InputWin::update()
    {
        for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i)
        {
            XINPUT_STATE state;

            memset(&state, 0, sizeof(XINPUT_STATE));

            DWORD result = XInputGetState(i, &state);

            if (result == ERROR_SUCCESS)
            {
                if (!_gamepads[i])
                {
                    _gamepads[i].reset(new GamepadWin(static_cast<int32_t>(i)));

                    GamepadEvent event;
                    event.type = Event::Type::GAMEPAD_CONNECT;
                    event.gamepad = _gamepads[i];

                    Engine::getInstance()->getEventDispatcher()->dispatchGamepadConnectEvent(event, Engine::getInstance()->getInput());
                }
                
                _gamepads[i]->update(state);
            }
            else if (result == ERROR_DEVICE_NOT_CONNECTED)
            {
                if (_gamepads[i])
                {
                    GamepadEvent event;
                    event.type = Event::Type::GAMEPAD_DISCONNECT;
                    event.gamepad = _gamepads[i];

                    Engine::getInstance()->getEventDispatcher()->dispatchGamepadDisconnectEvent(event, Engine::getInstance()->getInput());

                    _gamepads[i].reset();
                }
            }
            else
            {
                log("Failed to get state for gamepad %d", i);
            }
        }
    }

    void InputWin::setCursorVisible(bool visible)
    {
        _cursorVisible = visible;

        if (_cursorVisible)
        {
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
        }
        else
        {
            SetCursor(nullptr);
        }
    }

    bool InputWin::isCursorVisible() const
    {
        return _cursorVisible;
    }
}
