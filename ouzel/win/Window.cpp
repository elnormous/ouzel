// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <windowsx.h>
#include "Window.h"
#include "Engine.h"
#include "InputWin.h"

using namespace ouzel;

static KeyboardKey winKeyToEngineCode(WPARAM wParam, LPARAM lParam)
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
        case VK_MENU: return KeyboardKey::MENU;
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
        case VK_EXECUTE: return KeyboardKey::EXECUT;

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

        case VK_LWIN: return KeyboardKey::LWIN;
        case VK_RWIN: return KeyboardKey::RWIN;
        case VK_APPS: return KeyboardKey::MENU;
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
        case VK_LMENU: return KeyboardKey::LMENU;
        case VK_RMENU: return KeyboardKey::RMENU;

        case VK_OEM_1: return KeyboardKey::OEM_1;
        case VK_OEM_PLUS: return KeyboardKey::PLUS;
        case VK_OEM_COMMA: return KeyboardKey::COMMA;
        case VK_OEM_MINUS: return KeyboardKey::MINUS;
        case VK_OEM_PERIOD: return KeyboardKey::PERIOD;
        case VK_OEM_2: return KeyboardKey::OEM_2;
        case VK_OEM_3: return KeyboardKey::OEM_3;
        case VK_OEM_4: return KeyboardKey::OEM_4;
        case VK_OEM_5: return KeyboardKey::OEM_5;
        case VK_OEM_6: return KeyboardKey::OEM_6;
        case VK_OEM_7: return KeyboardKey::OEM_7;
        case VK_OEM_8: return KeyboardKey::OEM_8;
        case VK_OEM_AX: return KeyboardKey::OEM_AX;
        case VK_OEM_102: return KeyboardKey::OEM_102;

            // ... misc keys ...
    }
    return KeyboardKey::NONE;
}

static uint32_t getKeyboardModifiers(WPARAM wParam)
{
    uint32_t modifiers = 0;
    
    if (wParam & MK_SHIFT) modifiers |= Event::SHIFT_DOWN;
    if (wParam & MK_ALT) modifiers |= Event::ALT_DOWN;
    if (wParam & MK_CONTROL) modifiers |= Event::CONTROL_DOWN;
    
    return modifiers;
}

static uint32_t getMouseModifiers(WPARAM wParam)
{
    uint32_t modifiers = 0;
    
    if (wParam & MK_SHIFT) modifiers |= Event::SHIFT_DOWN;
    if (wParam & MK_CONTROL) modifiers |= Event::CONTROL_DOWN;
    if (wParam & MK_LBUTTON) modifiers |= Event::LEFT_MOUSE_DOWN;
    if (wParam & MK_RBUTTON) modifiers |= Event::RIGHT_MOUSE_DOWN;
    if (wParam & MK_MBUTTON) modifiers |= Event::MIDDLE_MOUSE_DOWN;
    
    return modifiers;
}

static void handleKeyEvent(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_KEYDOWN)
    {
        Engine::getInstance()->getInput()->keyDown(winKeyToEngineCode(wParam, lParam), getKeyboardModifiers(wParam));
    }
    else if (msg == WM_KEYUP)
    {
        Engine::getInstance()->getInput()->keyUp(winKeyToEngineCode(wParam, lParam), getKeyboardModifiers(wParam));
    }
}

static void handleMouseMoveEvent(UINT msg, WPARAM wParam, LPARAM lParam)
{
    Vector2 pos(static_cast<float>(GET_X_LPARAM(lParam)),
                static_cast<float>(GET_Y_LPARAM(lParam)));

    Engine::getInstance()->getInput()->mouseMove(Engine::getInstance()->getRenderer()->viewToScreenLocation(pos),
                                                 getMouseModifiers(wParam));
}

static void handleMouseButtonEvent(UINT msg, WPARAM wParam, LPARAM lParam)
{
    Vector2 pos(static_cast<float>(GET_X_LPARAM(lParam)),
                static_cast<float>(GET_Y_LPARAM(lParam)));

    MouseButton button;

    if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)
    {
        button = MouseButton::LEFT;
    }
    else if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)
    {
        button = MouseButton::RIGHT;
    }
    else if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)
    {
        button = MouseButton::MIDDLE;
    }
    else if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONUP)
    {
        if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
            button = MouseButton::X1;
        else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON2)
            button = MouseButton::X2;
    }
    else
    {
        return;
    }

    if (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN || msg == WM_XBUTTONDOWN)
    {
        Engine::getInstance()->getInput()->mouseDown(button,
                                                     Engine::getInstance()->getRenderer()->viewToScreenLocation(pos),
                                                     getMouseModifiers(wParam));
    }
    else if (msg == WM_LBUTTONUP || msg == WM_RBUTTONUP || msg == WM_MBUTTONUP || msg == WM_XBUTTONUP)
    {
        Engine::getInstance()->getInput()->mouseUp(button,
                                                   Engine::getInstance()->getRenderer()->viewToScreenLocation(pos),
                                                   getMouseModifiers(wParam));
    }
}

static void handleMouseWheelEvent(UINT msg, WPARAM wParam, LPARAM lParam)
{
    Vector2 pos(static_cast<float>(GET_X_LPARAM(lParam)),
                static_cast<float>(GET_Y_LPARAM(lParam)));

    if (msg == WM_MOUSEWHEEL)
    {
        Engine::getInstance()->getInput()->mouseScroll(Vector2(0.0f, static_cast<float>(HIWORD(wParam)) / static_cast<float>(WHEEL_DELTA)),
                                                       Engine::getInstance()->getRenderer()->viewToScreenLocation(pos),
                                                       getMouseModifiers(wParam));
    }
    else if (msg == WM_MOUSEHWHEEL)
    {
        Engine::getInstance()->getInput()->mouseScroll(Vector2(-static_cast<float>(HIWORD(wParam)) / static_cast<float>(WHEEL_DELTA), 0.0f),
                                                       Engine::getInstance()->getRenderer()->viewToScreenLocation(pos),
                                                       getMouseModifiers(wParam));
    }
}

LRESULT CALLBACK windowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_ACTIVATE:
        {
            UINT state = LOWORD(wParam);

            if (state == WA_ACTIVE || state == WA_CLICKACTIVE)
            {
                // TODO: Implement activation of the window
            }
            else if (state == WA_INACTIVE)
            {
                // TODO: Implement activation of the window
            }
            break;
        }
        case WM_KEYUP:
        case WM_KEYDOWN:
        {
            handleKeyEvent(msg, wParam, lParam);
            break;
        }
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
        {
            handleMouseButtonEvent(msg, wParam, lParam);

            if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONUP)
                return TRUE;

            return 0;
        }
        case WM_MOUSEMOVE:
        {
            handleMouseMoveEvent(msg, wParam, lParam);
            return 0;
        }
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
        {
            handleMouseWheelEvent(msg, wParam, lParam);
            return 0;
        }
        case WM_SETCURSOR:
        {
            if (LOWORD(lParam) == HTCLIENT)
            {
                if (!Engine::getInstance()->getInput()->isCursorVisible())
                {
                    SetCursor(nullptr);
                    return TRUE;
                }
            }
            break;
        }
        case WM_SIZE:
        {
            if (wParam == SIZE_MINIMIZED)
            {
                // TODO: stop engine
            }
            else if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
            {
                INT width = LOWORD(lParam);
                INT height = HIWORD(lParam);

                Engine::getInstance()->getRenderer()->resize(Size2(width, height));
            }
            return 0;
        }
        case WM_ERASEBKGND:
        {
            return TRUE;
        }
        case WM_SYSCOMMAND:
        {
            switch (wParam)
            {
            case SC_SCREENSAVE:
            case SC_MONITORPOWER:
            {
                if (Engine::getInstance()->getRenderer()->isFullscreen())
                {
                    // Disable screensaver in fullscreen mode
                    return 0;
                }
                break;
            }
            // Disable accessing menu using alt key
            case SC_KEYMENU:
                return 0;
            }
            break;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            Engine::getInstance()->end();
            return 0;
        }
    }

    return DefWindowProcW(window, msg, wParam, lParam);
}
