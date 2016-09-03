// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <windowsx.h>
#include "WindowWin.h"
#include "core/Application.h"
#include "core/Engine.h"
#include "InputWin.h"
#include "graphics/Renderer.h"
#include "utils/Utils.h"

using namespace ouzel;

static void handleKeyEvent(UINT msg, WPARAM wParam, LPARAM)
{
    if (msg == WM_KEYDOWN)
    {
        sharedEngine->getInput()->keyDown(input::InputWin::convertKeyCode(wParam),
                                          input::InputWin::getKeyboardModifiers(wParam));
    }
    else if (msg == WM_KEYUP)
    {
        sharedEngine->getInput()->keyUp(input::InputWin::convertKeyCode(wParam),
                                        input::InputWin::getKeyboardModifiers(wParam));
    }
}

static void handleMouseMoveEvent(UINT, WPARAM wParam, LPARAM lParam)
{
    Vector2 pos(static_cast<float>(GET_X_LPARAM(lParam)),
                static_cast<float>(GET_Y_LPARAM(lParam)));

    sharedEngine->getInput()->mouseMove(sharedEngine->getRenderer()->viewToScreenLocation(pos),
                                        input::InputWin::getMouseModifiers(wParam));
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
                                            input::InputWin::getMouseModifiers(wParam));
    }
    else if (msg == WM_LBUTTONUP || msg == WM_RBUTTONUP || msg == WM_MBUTTONUP || msg == WM_XBUTTONUP)
    {
        sharedEngine->getInput()->mouseUp(button,
                                          sharedEngine->getRenderer()->viewToScreenLocation(pos),
                                          input::InputWin::getMouseModifiers(wParam));
    }
}

static void handleMouseWheelEvent(UINT msg, WPARAM wParam, LPARAM lParam)
{
    Vector2 pos(static_cast<float>(GET_X_LPARAM(lParam)),
                static_cast<float>(GET_Y_LPARAM(lParam)));

    if (msg == WM_MOUSEWHEEL)
    {
        short param = static_cast<short>(HIWORD(wParam));
        sharedEngine->getInput()->mouseScroll(Vector2(0.0f, -static_cast<float>(param) / static_cast<float>(WHEEL_DELTA)),
                                              sharedEngine->getRenderer()->viewToScreenLocation(pos),
                                              input::InputWin::getMouseModifiers(wParam));
    }
    else if (msg == WM_MOUSEHWHEEL)
    {
        short param = static_cast<short>(HIWORD(wParam));
        sharedEngine->getInput()->mouseScroll(Vector2(static_cast<float>(param) / static_cast<float>(WHEEL_DELTA), 0.0f),
                                              sharedEngine->getRenderer()->viewToScreenLocation(pos),
                                              input::InputWin::getMouseModifiers(wParam));
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
                ouzel::sharedEngine->resume();
            }
            else if (state == WA_INACTIVE)
            {
                ouzel::sharedEngine->pause();
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
        if (window)
        {
            DestroyWindow(window);
        }

        if (windowClass)
        {
            UnregisterClassW(WINDOW_CLASS_NAME, GetModuleHandle(nullptr));
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
        sharedApplication->execute([this] {
            SendMessage(window, WM_CLOSE, 0, 0);
        });
    }

    void WindowWin::setSize(const Size2& newSize)
    {
        sharedApplication->execute([this, newSize] {
            UINT width = static_cast<UINT>(newSize.width);
            UINT height = static_cast<UINT>(newSize.height);

            UINT swpFlags = SWP_NOMOVE | SWP_NOZORDER;

            RECT rect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
            AdjustWindowRect(&rect, windowStyle, GetMenu(window) ? TRUE : FALSE);

            SetWindowPos(window, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, swpFlags);
        });

        Window::setSize(newSize);
    }

    void WindowWin::setTitle(const std::string& newTitle)
    {
        if (title != newTitle)
        {
            sharedApplication->execute([this, newTitle] {
                wchar_t titleBuffer[256];
                MultiByteToWideChar(CP_UTF8, 0, newTitle.c_str(), -1, titleBuffer, 256);

                SetWindowTextW(window, titleBuffer);
            });
        }

        Window::setTitle(newTitle);
    }

    void WindowWin::handleResize(INT width, INT height)
    {
        Window::setSize(Size2(static_cast<float>(width), static_cast<float>(height)));
    }

    void WindowWin::addAccelerator(HACCEL accelerator)
    {
        accelerators.insert(accelerator);
    }

    void WindowWin::removeAccelerator(HACCEL accelerator)
    {
        accelerators.erase(accelerator);
    }

    HMONITOR WindowWin::getMonitor() const
    {
        return MonitorFromWindow(window, MONITOR_DEFAULTTONULL);
    }
}
