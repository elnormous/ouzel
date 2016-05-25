// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#define NOMINMAX
#include <windows.h>
#include "InputWin.h"
#include "Engine.h"
#include "GamepadWin.h"
#include "Utils.h"

namespace ouzel
{
    namespace input
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
                    if (!gamepads[i])
                    {
                        gamepads[i].reset(new GamepadWin(static_cast<int32_t>(i)));

                        Event event;
                        event.sender = shared_from_this();
                        event.type = Event::Type::GAMEPAD_CONNECT;
                        
                        event.gamepadEvent.gamepad = gamepads[i];

                        sharedEngine->getEventDispatcher()->dispatchEvent(event);
                    }

                    gamepads[i]->update(state);
                }
                else if (result == ERROR_DEVICE_NOT_CONNECTED)
                {
                    if (gamepads[i])
                    {
                        Event event;
                        event.sender = shared_from_this();
                        event.type = Event::Type::GAMEPAD_DISCONNECT;
                        
                        event.gamepadEvent.gamepad = gamepads[i];

                        sharedEngine->getEventDispatcher()->dispatchEvent(event);

                        gamepads[i].reset();
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
            cursorVisible = visible;

            if (cursorVisible)
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
            return cursorVisible;
        }
    } // namespace input
} // namespace ouzel
