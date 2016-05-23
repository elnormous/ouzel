// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <windowsx.h>
#include "WindowWin.h"
#include "Engine.h"
#include "InputWin.h"
#include "Renderer.h"
#include "Utils.h"

using namespace ouzel;

static input::KeyboardKey winKeyToEngineCode(WPARAM wParam)
{
    switch (wParam)
    {
        case VK_CANCEL: return input::KeyboardKey::CANCEL;
        case VK_BACK: return input::KeyboardKey::BACKSPACE;
        case VK_TAB: return input::KeyboardKey::TAB;
        case VK_CLEAR: return input::KeyboardKey::CLEAR;
        case VK_RETURN: return input::KeyboardKey::RETURN;
        case VK_SHIFT: return input::KeyboardKey::SHIFT;
        case VK_CONTROL: return input::KeyboardKey::CONTROL;
        case VK_MENU: return input::KeyboardKey::MENU;
        case VK_PAUSE: return input::KeyboardKey::PAUSE;
        case VK_CAPITAL: return input::KeyboardKey::CAPITAL;

            // ... Japanese ...

        case VK_ESCAPE: return input::KeyboardKey::ESCAPE;

            // ... IME ...

        case VK_SPACE: return input::KeyboardKey::SPACE;
        case VK_PRIOR: return input::KeyboardKey::PRIOR;
        case VK_NEXT: return input::KeyboardKey::NEXT;
        case VK_END: return input::KeyboardKey::END;
        case VK_HOME: return input::KeyboardKey::HOME;
        case VK_LEFT: return input::KeyboardKey::LEFT;
        case VK_UP: return input::KeyboardKey::UP;
        case VK_RIGHT: return input::KeyboardKey::RIGHT;
        case VK_DOWN: return input::KeyboardKey::DOWN;

        case VK_SELECT: return input::KeyboardKey::SELECT;
        case VK_PRINT: return input::KeyboardKey::PRINT;
        case VK_EXECUTE: return input::KeyboardKey::EXECUT;

        case VK_SNAPSHOT: return input::KeyboardKey::SNAPSHOT;
        case VK_INSERT: return input::KeyboardKey::INSERT;
        case VK_DELETE: return input::KeyboardKey::DEL;
        case VK_HELP: return input::KeyboardKey::HELP;

        case '0': return input::KeyboardKey::KEY_0;
        case '1': return input::KeyboardKey::KEY_1;
        case '2': return input::KeyboardKey::KEY_2;
        case '3': return input::KeyboardKey::KEY_3;
        case '4': return input::KeyboardKey::KEY_4;
        case '5': return input::KeyboardKey::KEY_5;
        case '6': return input::KeyboardKey::KEY_6;
        case '7': return input::KeyboardKey::KEY_7;
        case '8': return input::KeyboardKey::KEY_8;
        case '9': return input::KeyboardKey::KEY_9;

        case 'A': return input::KeyboardKey::KEY_A;
        case 'B': return input::KeyboardKey::KEY_B;
        case 'C': return input::KeyboardKey::KEY_C;
        case 'D': return input::KeyboardKey::KEY_D;
        case 'E': return input::KeyboardKey::KEY_E;
        case 'F': return input::KeyboardKey::KEY_F;
        case 'G': return input::KeyboardKey::KEY_G;
        case 'H': return input::KeyboardKey::KEY_H;
        case 'I': return input::KeyboardKey::KEY_I;
        case 'J': return input::KeyboardKey::KEY_J;
        case 'K': return input::KeyboardKey::KEY_K;
        case 'L': return input::KeyboardKey::KEY_L;
        case 'M': return input::KeyboardKey::KEY_M;
        case 'N': return input::KeyboardKey::KEY_N;
        case 'O': return input::KeyboardKey::KEY_O;
        case 'P': return input::KeyboardKey::KEY_P;
        case 'Q': return input::KeyboardKey::KEY_Q;
        case 'R': return input::KeyboardKey::KEY_R;
        case 'S': return input::KeyboardKey::KEY_S;
        case 'T': return input::KeyboardKey::KEY_T;
        case 'U': return input::KeyboardKey::KEY_U;
        case 'V': return input::KeyboardKey::KEY_V;
        case 'W': return input::KeyboardKey::KEY_W;
        case 'X': return input::KeyboardKey::KEY_X;
        case 'Y': return input::KeyboardKey::KEY_Y;
        case 'Z': return input::KeyboardKey::KEY_Z;

        case VK_LWIN: return input::KeyboardKey::LWIN;
        case VK_RWIN: return input::KeyboardKey::RWIN;
        case VK_APPS: return input::KeyboardKey::MENU;
        case VK_SLEEP: return input::KeyboardKey::SLEEP;

        case VK_NUMPAD0: return input::KeyboardKey::NUMPAD0;
        case VK_NUMPAD1: return input::KeyboardKey::NUMPAD1;
        case VK_NUMPAD2: return input::KeyboardKey::NUMPAD2;
        case VK_NUMPAD3: return input::KeyboardKey::NUMPAD3;
        case VK_NUMPAD4: return input::KeyboardKey::NUMPAD4;
        case VK_NUMPAD5: return input::KeyboardKey::NUMPAD5;
        case VK_NUMPAD6: return input::KeyboardKey::NUMPAD6;
        case VK_NUMPAD7: return input::KeyboardKey::NUMPAD7;
        case VK_NUMPAD8: return input::KeyboardKey::NUMPAD8;
        case VK_NUMPAD9: return input::KeyboardKey::NUMPAD9;

        case VK_MULTIPLY: return input::KeyboardKey::MULTIPLY;
        case VK_ADD: return input::KeyboardKey::ADD;
        case VK_SEPARATOR: return input::KeyboardKey::SEPARATOR;
        case VK_SUBTRACT: return input::KeyboardKey::SUBTRACT;
        case VK_DECIMAL: return input::KeyboardKey::DECIMAL;
        case VK_DIVIDE: return input::KeyboardKey::DIVIDE;

        case VK_F1: return input::KeyboardKey::F1;
        case VK_F2: return input::KeyboardKey::F2;
        case VK_F3: return input::KeyboardKey::F3;
        case VK_F4: return input::KeyboardKey::F4;
        case VK_F5: return input::KeyboardKey::F5;
        case VK_F6: return input::KeyboardKey::F6;
        case VK_F7: return input::KeyboardKey::F7;
        case VK_F8: return input::KeyboardKey::F8;
        case VK_F9: return input::KeyboardKey::F9;
        case VK_F10: return input::KeyboardKey::F10;
        case VK_F11: return input::KeyboardKey::F11;
        case VK_F12: return input::KeyboardKey::F12;
        case VK_F13: return input::KeyboardKey::F13;
        case VK_F14: return input::KeyboardKey::F14;
        case VK_F15: return input::KeyboardKey::F15;
        case VK_F16: return input::KeyboardKey::F16;
        case VK_F17: return input::KeyboardKey::F17;
        case VK_F18: return input::KeyboardKey::F18;
        case VK_F19: return input::KeyboardKey::F19;
        case VK_F20: return input::KeyboardKey::F20;
        case VK_F21: return input::KeyboardKey::F21;
        case VK_F22: return input::KeyboardKey::F22;
        case VK_F23: return input::KeyboardKey::F23;
        case VK_F24: return input::KeyboardKey::F24;

        case VK_NUMLOCK: return input::KeyboardKey::NUMLOCK;
        case VK_SCROLL: return input::KeyboardKey::SCROLL;
        case VK_LSHIFT: return input::KeyboardKey::LSHIFT;
        case VK_RSHIFT: return input::KeyboardKey::RSHIFT;
        case VK_LCONTROL: return input::KeyboardKey::LCONTROL;
        case VK_RCONTROL: return input::KeyboardKey::RCONTROL;
        case VK_LMENU: return input::KeyboardKey::LMENU;
        case VK_RMENU: return input::KeyboardKey::RMENU;

        case VK_OEM_1: return input::KeyboardKey::OEM_1;
        case VK_OEM_PLUS: return input::KeyboardKey::PLUS;
        case VK_OEM_COMMA: return input::KeyboardKey::COMMA;
        case VK_OEM_MINUS: return input::KeyboardKey::MINUS;
        case VK_OEM_PERIOD: return input::KeyboardKey::PERIOD;
        case VK_OEM_2: return input::KeyboardKey::OEM_2;
        case VK_OEM_3: return input::KeyboardKey::OEM_3;
        case VK_OEM_4: return input::KeyboardKey::OEM_4;
        case VK_OEM_5: return input::KeyboardKey::OEM_5;
        case VK_OEM_6: return input::KeyboardKey::OEM_6;
        case VK_OEM_7: return input::KeyboardKey::OEM_7;
        case VK_OEM_8: return input::KeyboardKey::OEM_8;
        case VK_OEM_AX: return input::KeyboardKey::OEM_AX;
        case VK_OEM_102: return input::KeyboardKey::OEM_102;

            // ... misc keys ...
    }
    return input::KeyboardKey::NONE;
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
    OUZEL_UNUSED(lParam);

    if (msg == WM_KEYDOWN)
    {
        sharedEngine->getInput()->keyDown(winKeyToEngineCode(wParam), getKeyboardModifiers(wParam));
    }
    else if (msg == WM_KEYUP)
    {
        sharedEngine->getInput()->keyUp(winKeyToEngineCode(wParam), getKeyboardModifiers(wParam));
    }
}

static void handleMouseMoveEvent(UINT msg, WPARAM wParam, LPARAM lParam)
{
    OUZEL_UNUSED(msg);

    Vector2 pos(static_cast<float>(GET_X_LPARAM(lParam)),
                static_cast<float>(GET_Y_LPARAM(lParam)));

    sharedEngine->getInput()->mouseMove(sharedEngine->getRenderer()->viewToScreenLocation(pos),
                                        getMouseModifiers(wParam));
}

static void handleMouseButtonEvent(UINT msg, WPARAM wParam, LPARAM lParam)
{
    Vector2 pos(static_cast<float>(GET_X_LPARAM(lParam)),
                static_cast<float>(GET_Y_LPARAM(lParam)));

    input::MouseButton button;

    if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)
    {
        button = input::MouseButton::LEFT;
    }
    else if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)
    {
        button = input::MouseButton::RIGHT;
    }
    else if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)
    {
        button = input::MouseButton::MIDDLE;
    }
    else if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONUP)
    {
        if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
            button = input::MouseButton::X1;
        else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON2)
            button = input::MouseButton::X2;
        else
            return;
    }
    else
    {
        return;
    }

    if (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN || msg == WM_XBUTTONDOWN)
    {
        sharedEngine->getInput()->mouseDown(button,
                                                     sharedEngine->getRenderer()->viewToScreenLocation(pos),
                                                     getMouseModifiers(wParam));
    }
    else if (msg == WM_LBUTTONUP || msg == WM_RBUTTONUP || msg == WM_MBUTTONUP || msg == WM_XBUTTONUP)
    {
        sharedEngine->getInput()->mouseUp(button,
                                                   sharedEngine->getRenderer()->viewToScreenLocation(pos),
                                                   getMouseModifiers(wParam));
    }
}

static void handleMouseWheelEvent(UINT msg, WPARAM wParam, LPARAM lParam)
{
    Vector2 pos(static_cast<float>(GET_X_LPARAM(lParam)),
                static_cast<float>(GET_Y_LPARAM(lParam)));

    if (msg == WM_MOUSEWHEEL)
    {
        sharedEngine->getInput()->mouseScroll(Vector2(0.0f, static_cast<float>(HIWORD(wParam)) / static_cast<float>(WHEEL_DELTA)),
                                                       sharedEngine->getRenderer()->viewToScreenLocation(pos),
                                                       getMouseModifiers(wParam));
    }
    else if (msg == WM_MOUSEHWHEEL)
    {
        sharedEngine->getInput()->mouseScroll(Vector2(-static_cast<float>(HIWORD(wParam)) / static_cast<float>(WHEEL_DELTA), 0.0f),
                                                       sharedEngine->getRenderer()->viewToScreenLocation(pos),
                                                       getMouseModifiers(wParam));
    }
}

static LRESULT CALLBACK windowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    WindowWin* windowWin = reinterpret_cast<WindowWin*>(GetWindowLongPtr(window, GWLP_USERDATA));

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
                if (!sharedEngine->getInput()->isCursorVisible())
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
                windowWin->handleResize(LOWORD(lParam), HIWORD(lParam));
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
                if (windowWin->isFullscreen())
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
            return 0;
        }
    }

    return DefWindowProcW(window, msg, wParam, lParam);
}

const LPCWSTR WINDOW_CLASS_NAME = L"OuzelWindow";

namespace ouzel
{
    WindowWin::WindowWin(const Size2& pSize, bool pResizable, bool pFullscreen, const std::string& pTitle):
        Window(pSize, pResizable, pFullscreen, pTitle)
    {

    }

    WindowWin::~WindowWin()
    {
        if (window) DestroyWindow(window);
        if (windowClass)
        {
            UnregisterClassW(WINDOW_CLASS_NAME, GetModuleHandle(nullptr));
            windowClass = 0;
        }
    }

    bool WindowWin::init()
    {
        HINSTANCE hInstance = GetModuleHandleW(nullptr);

        WNDCLASSEXW wc;
        memset(&wc, 0, sizeof(wc));
        wc.cbSize = sizeof(wc);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = windowProc;
        wc.hInstance = hInstance;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.lpszClassName = WINDOW_CLASS_NAME;
        // Application icon should be the first resource
        wc.hIcon = LoadIconW(hInstance, MAKEINTRESOURCE(101));

        windowClass = RegisterClassExW(&wc);
        if (!windowClass)
        {
            log("Failed to register window class");
            return false;
        }

        windowStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

        if (resizable)
        {
            windowStyle |= WS_SIZEBOX | WS_MAXIMIZEBOX;
        }

        int x = CW_USEDEFAULT;
        int y = CW_USEDEFAULT;
        if (fullscreen)
        {
            windowStyle = WS_POPUP;
            x = 0;
            y = 0;
        }
        RECT windowRect = { 0, 0, static_cast<LONG>(size.width), static_cast<LONG>(size.height) };
        AdjustWindowRect(&windowRect, windowStyle, FALSE);

        wchar_t titleBuffer[256];
        MultiByteToWideChar(CP_UTF8, 0, title.c_str(), -1, titleBuffer, 256);

        window = CreateWindowExW(
            0,
            WINDOW_CLASS_NAME,
            titleBuffer,
            windowStyle,
            x,
            y,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            nullptr,
            nullptr,
            hInstance,
            nullptr);

        if (!window)
        {
            log("Failed to create window");
            return false;
        }

        SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        ShowWindow(window, SW_SHOW);

        return Window::init();
    }

    void WindowWin::close()
    {
        SendMessage(window, WM_CLOSE, 0, 0);
    }

    void WindowWin::setSize(const Size2& newSize)
    {
        UINT width = static_cast<UINT>(newSize.width);
        UINT height = static_cast<UINT>(newSize.height);

        UINT swpFlags = SWP_NOMOVE | SWP_NOZORDER;

        RECT rect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
        AdjustWindowRect(&rect, windowStyle, FALSE);

        SetWindowPos(window, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, swpFlags);

        Window::setSize(newSize);
    }

    void WindowWin::setTitle(const std::string& newTitle)
    {
        if (title != newTitle)
        {
            wchar_t titleBuffer[256];
            MultiByteToWideChar(CP_UTF8, 0, newTitle.c_str(), -1, titleBuffer, 256);

            SetWindowTextW(window, titleBuffer);
        }

        Window::setTitle(newTitle);
    }

    void WindowWin::handleResize(INT width, INT height)
    {
        setSize(Size2(static_cast<float>(width), static_cast<float>(height)));
    }

    HMONITOR WindowWin::getMonitor() const
    {
        return MonitorFromWindow(window, MONITOR_DEFAULTTONULL);
    }
}
