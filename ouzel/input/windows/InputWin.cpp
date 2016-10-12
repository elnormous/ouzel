// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#define NOMINMAX
#include <windows.h>
#include "InputWin.h"
#include "core/Engine.h"
#include "events/EventDispatcher.h"
#include "GamepadWin.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace input
    {
        KeyboardKey InputWin::convertKeyCode(WPARAM wParam)
        {
            switch (wParam)
            {
            case VK_CANCEL: return KeyboardKey::CANCEL;
            case VK_BACK: return KeyboardKey::BACKSPACE;
            case VK_TAB: return KeyboardKey::TAB;
            case VK_CLEAR: return KeyboardKey::CLEAR;
            case VK_RETURN: return KeyboardKey::RETURN;
            case VK_SHIFT: return KeyboardKey::SHIFT;
            case VK_CONTROL: return KeyboardKey::CONTROL;
            case VK_MENU: return KeyboardKey::LALT;
            case VK_PAUSE: return KeyboardKey::PAUSE;
            case VK_CAPITAL: return KeyboardKey::CAPITAL;

                // ... Japanese ...

            case VK_ESCAPE: return KeyboardKey::ESCAPE;

                // ... IME ...

            case VK_SPACE: return KeyboardKey::SPACE;
            case VK_PRIOR: return KeyboardKey::PRIOR;
            case VK_NEXT: return KeyboardKey::NEXT;
            case VK_END: return KeyboardKey::END;
            case VK_HOME: return KeyboardKey::HOME;
            case VK_LEFT: return KeyboardKey::LEFT;
            case VK_UP: return KeyboardKey::UP;
            case VK_RIGHT: return KeyboardKey::RIGHT;
            case VK_DOWN: return KeyboardKey::DOWN;

            case VK_SELECT: return KeyboardKey::SELECT;
            case VK_PRINT: return KeyboardKey::PRINT;
            case VK_EXECUTE: return KeyboardKey::EXECUTE;

            case VK_SNAPSHOT: return KeyboardKey::SNAPSHOT;
            case VK_INSERT: return KeyboardKey::INSERT;
            case VK_DELETE: return KeyboardKey::DEL;
            case VK_HELP: return KeyboardKey::HELP;

            case '0': return KeyboardKey::KEY_0;
            case '1': return KeyboardKey::KEY_1;
            case '2': return KeyboardKey::KEY_2;
            case '3': return KeyboardKey::KEY_3;
            case '4': return KeyboardKey::KEY_4;
            case '5': return KeyboardKey::KEY_5;
            case '6': return KeyboardKey::KEY_6;
            case '7': return KeyboardKey::KEY_7;
            case '8': return KeyboardKey::KEY_8;
            case '9': return KeyboardKey::KEY_9;

            case 'A': return KeyboardKey::KEY_A;
            case 'B': return KeyboardKey::KEY_B;
            case 'C': return KeyboardKey::KEY_C;
            case 'D': return KeyboardKey::KEY_D;
            case 'E': return KeyboardKey::KEY_E;
            case 'F': return KeyboardKey::KEY_F;
            case 'G': return KeyboardKey::KEY_G;
            case 'H': return KeyboardKey::KEY_H;
            case 'I': return KeyboardKey::KEY_I;
            case 'J': return KeyboardKey::KEY_J;
            case 'K': return KeyboardKey::KEY_K;
            case 'L': return KeyboardKey::KEY_L;
            case 'M': return KeyboardKey::KEY_M;
            case 'N': return KeyboardKey::KEY_N;
            case 'O': return KeyboardKey::KEY_O;
            case 'P': return KeyboardKey::KEY_P;
            case 'Q': return KeyboardKey::KEY_Q;
            case 'R': return KeyboardKey::KEY_R;
            case 'S': return KeyboardKey::KEY_S;
            case 'T': return KeyboardKey::KEY_T;
            case 'U': return KeyboardKey::KEY_U;
            case 'V': return KeyboardKey::KEY_V;
            case 'W': return KeyboardKey::KEY_W;
            case 'X': return KeyboardKey::KEY_X;
            case 'Y': return KeyboardKey::KEY_Y;
            case 'Z': return KeyboardKey::KEY_Z;

            case VK_LWIN: return KeyboardKey::LSUPER;
            case VK_RWIN: return KeyboardKey::RSUPER;
            case VK_APPS: return KeyboardKey::APPS;
            case VK_SLEEP: return KeyboardKey::SLEEP;

            case VK_NUMPAD0: return KeyboardKey::NUMPAD0;
            case VK_NUMPAD1: return KeyboardKey::NUMPAD1;
            case VK_NUMPAD2: return KeyboardKey::NUMPAD2;
            case VK_NUMPAD3: return KeyboardKey::NUMPAD3;
            case VK_NUMPAD4: return KeyboardKey::NUMPAD4;
            case VK_NUMPAD5: return KeyboardKey::NUMPAD5;
            case VK_NUMPAD6: return KeyboardKey::NUMPAD6;
            case VK_NUMPAD7: return KeyboardKey::NUMPAD7;
            case VK_NUMPAD8: return KeyboardKey::NUMPAD8;
            case VK_NUMPAD9: return KeyboardKey::NUMPAD9;

            case VK_MULTIPLY: return KeyboardKey::MULTIPLY;
            case VK_ADD: return KeyboardKey::ADD;
            case VK_SEPARATOR: return KeyboardKey::SEPARATOR;
            case VK_SUBTRACT: return KeyboardKey::SUBTRACT;
            case VK_DECIMAL: return KeyboardKey::DECIMAL;
            case VK_DIVIDE: return KeyboardKey::DIVIDE;

            case VK_F1: return KeyboardKey::F1;
            case VK_F2: return KeyboardKey::F2;
            case VK_F3: return KeyboardKey::F3;
            case VK_F4: return KeyboardKey::F4;
            case VK_F5: return KeyboardKey::F5;
            case VK_F6: return KeyboardKey::F6;
            case VK_F7: return KeyboardKey::F7;
            case VK_F8: return KeyboardKey::F8;
            case VK_F9: return KeyboardKey::F9;
            case VK_F10: return KeyboardKey::F10;
            case VK_F11: return KeyboardKey::F11;
            case VK_F12: return KeyboardKey::F12;
            case VK_F13: return KeyboardKey::F13;
            case VK_F14: return KeyboardKey::F14;
            case VK_F15: return KeyboardKey::F15;
            case VK_F16: return KeyboardKey::F16;
            case VK_F17: return KeyboardKey::F17;
            case VK_F18: return KeyboardKey::F18;
            case VK_F19: return KeyboardKey::F19;
            case VK_F20: return KeyboardKey::F20;
            case VK_F21: return KeyboardKey::F21;
            case VK_F22: return KeyboardKey::F22;
            case VK_F23: return KeyboardKey::F23;
            case VK_F24: return KeyboardKey::F24;

            case VK_NUMLOCK: return KeyboardKey::NUMLOCK;
            case VK_SCROLL: return KeyboardKey::SCROLL;
            case VK_LSHIFT: return KeyboardKey::LSHIFT;
            case VK_RSHIFT: return KeyboardKey::RSHIFT;
            case VK_LCONTROL: return KeyboardKey::LCONTROL;
            case VK_RCONTROL: return KeyboardKey::RCONTROL;
            case VK_LMENU: return KeyboardKey::LALT;
            case VK_RMENU: return KeyboardKey::RALT;

            case VK_OEM_1: return KeyboardKey::SEMICOLON;
            case VK_OEM_PLUS: return KeyboardKey::PLUS;
            case VK_OEM_COMMA: return KeyboardKey::COMMA;
            case VK_OEM_MINUS: return KeyboardKey::MINUS;
            case VK_OEM_PERIOD: return KeyboardKey::PERIOD;
            case VK_OEM_2: return KeyboardKey::SLASH;
            case VK_OEM_3: return KeyboardKey::GRAVE;
            case VK_OEM_4: return KeyboardKey::BRACKET_LEFT;
            case VK_OEM_5: return KeyboardKey::BACKSLASH;
            case VK_OEM_6: return KeyboardKey::BRACKET_RIGHT;
            case VK_OEM_7: return KeyboardKey::QUOTE;
            case VK_OEM_8: return KeyboardKey::GRAVE;
            case VK_OEM_AX: return KeyboardKey::OEM_AX;
            case VK_OEM_102: return KeyboardKey::LESS;

                // ... misc keys ...
            }
            return KeyboardKey::NONE;
        }

        uint32_t InputWin::getKeyboardModifiers(WPARAM wParam)
        {
            uint32_t modifiers = 0;

            if (wParam & MK_SHIFT) modifiers |= SHIFT_DOWN;
            if (wParam & MK_ALT) modifiers |= ALT_DOWN;
            if (wParam & MK_CONTROL) modifiers |= CONTROL_DOWN;

            return modifiers;
        }

        uint32_t InputWin::getMouseModifiers(WPARAM wParam)
        {
            uint32_t modifiers = 0;

            if (wParam & MK_SHIFT) modifiers |= SHIFT_DOWN;
            if (GetKeyState(VK_MENU) & 0x8000) modifiers |= ALT_DOWN;
            if (wParam & MK_CONTROL) modifiers |= CONTROL_DOWN;
            if (wParam & MK_LBUTTON) modifiers |= LEFT_MOUSE_DOWN;
            if (wParam & MK_RBUTTON) modifiers |= RIGHT_MOUSE_DOWN;
            if (wParam & MK_MBUTTON) modifiers |= MIDDLE_MOUSE_DOWN;

            return modifiers;
        }

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
                        event.type = Event::Type::GAMEPAD_CONNECT;

                        event.gamepadEvent.gamepad = gamepads[i];

                        sharedEngine->getEventDispatcher()->postEvent(event);
                    }

                    gamepads[i]->update(state);
                }
                else if (result == ERROR_DEVICE_NOT_CONNECTED)
                {
                    if (gamepads[i])
                    {
                        Event event;
                        event.type = Event::Type::GAMEPAD_DISCONNECT;

                        event.gamepadEvent.gamepad = gamepads[i];

                        sharedEngine->getEventDispatcher()->postEvent(event);

                        gamepads[i].reset();
                    }
                }
                else
                {
                    Log(Log::Level::WARN) << "Failed to get state for gamepad " << i;
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
