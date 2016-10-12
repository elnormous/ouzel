// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <windowsx.h>
#include "WindowWin.h"
#include "core/Application.h"
#include "core/Engine.h"
#include "core/Window.h"
#include "input/windows/InputWin.h"
#include "graphics/Renderer.h"
#include "graphics/direct3d11/RendererD3D11.h"
#include "utils/Log.h"

static void handleKeyEvent(UINT msg, WPARAM wParam, LPARAM)
{
    if (msg == WM_KEYDOWN)
    {
        ouzel::sharedEngine->getInput()->keyDown(ouzel::input::InputWin::convertKeyCode(wParam),
                                                 ouzel::input::InputWin::getKeyboardModifiers(wParam));
    }
    else if (msg == WM_KEYUP)
    {
        ouzel::sharedEngine->getInput()->keyUp(ouzel::input::InputWin::convertKeyCode(wParam),
                                               ouzel::input::InputWin::getKeyboardModifiers(wParam));
    }
}

static void handleMouseMoveEvent(UINT, WPARAM wParam, LPARAM lParam)
{
    ouzel::Vector2 position(static_cast<float>(GET_X_LPARAM(lParam)),
                            static_cast<float>(GET_Y_LPARAM(lParam)));

    ouzel::sharedEngine->getInput()->mouseMove(ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(position),
                                               ouzel::input::InputWin::getMouseModifiers(wParam));
}

static void handleMouseButtonEvent(UINT msg, WPARAM wParam, LPARAM lParam)
{
    ouzel::Vector2 position(static_cast<float>(GET_X_LPARAM(lParam)),
                            static_cast<float>(GET_Y_LPARAM(lParam)));

    ouzel::input::MouseButton button;

    if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)
    {
        button = ouzel::input::MouseButton::LEFT;
    }
    else if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)
    {
        button = ouzel::input::MouseButton::RIGHT;
    }
    else if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)
    {
        button = ouzel::input::MouseButton::MIDDLE;
    }
    else if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONUP)
    {
        if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
            button = ouzel::input::MouseButton::X1;
        else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON2)
            button = ouzel::input::MouseButton::X2;
        else
            return;
    }
    else
    {
        return;
    }

    if (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN || msg == WM_XBUTTONDOWN)
    {
        ouzel::sharedEngine->getInput()->mouseDown(button,
                                                   ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(position),
                                                   ouzel::input::InputWin::getMouseModifiers(wParam));
    }
    else if (msg == WM_LBUTTONUP || msg == WM_RBUTTONUP || msg == WM_MBUTTONUP || msg == WM_XBUTTONUP)
    {
        ouzel::sharedEngine->getInput()->mouseUp(button,
                                                 ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(position),
                                                 ouzel::input::InputWin::getMouseModifiers(wParam));
    }
}

static void handleMouseWheelEvent(UINT msg, WPARAM wParam, LPARAM lParam)
{
    ouzel::Vector2 position(static_cast<float>(GET_X_LPARAM(lParam)),
                            static_cast<float>(GET_Y_LPARAM(lParam)));

    if (msg == WM_MOUSEWHEEL)
    {
        short param = static_cast<short>(HIWORD(wParam));
        ouzel::sharedEngine->getInput()->mouseScroll(ouzel::Vector2(0.0f, -static_cast<float>(param) / static_cast<float>(WHEEL_DELTA)),
                                                     ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(position),
                                                     ouzel::input::InputWin::getMouseModifiers(wParam));
    }
    else if (msg == WM_MOUSEHWHEEL)
    {
        short param = static_cast<short>(HIWORD(wParam));
        ouzel::sharedEngine->getInput()->mouseScroll(ouzel::Vector2(static_cast<float>(param) / static_cast<float>(WHEEL_DELTA), 0.0f),
                                                     ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(position),
                                                     ouzel::input::InputWin::getMouseModifiers(wParam));
    }
}

static void handleTouchEvent(WPARAM wParam, LPARAM lParam)
{
    UINT inputCount = LOWORD(wParam);
    std::vector<TOUCHINPUT> touches(inputCount);

    if (GetTouchInputInfo(reinterpret_cast<HTOUCHINPUT>(lParam), inputCount, touches.data(), sizeof(TOUCHINPUT)))
    {
        ouzel::Vector2 position;

        for (const TOUCHINPUT& touch : touches)
        {
            position.x = static_cast<float>(touch.x);
            position.y = static_cast<float>(touch.y);

            if (touch.dwFlags & TOUCHEVENTF_DOWN)
            {}
                ouzel::sharedEngine->getInput()->touchBegin(touch.dwID,
                                                            ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(position));
            

            if (touch.dwFlags & TOUCHEVENTF_UP)
            {
                ouzel::sharedEngine->getInput()->touchEnd(touch.dwID,
                                                          ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(position));
            }

            if (touch.dwFlags & TOUCHEVENTF_MOVE)
            {
                ouzel::sharedEngine->getInput()->touchMove(touch.dwID,
                                                           ouzel::sharedEngine->getWindow()->convertWindowToNormalizedLocation(position));
            }
        }

        if (!CloseTouchInputHandle(reinterpret_cast<HTOUCHINPUT>(lParam)))
        {
            ouzel::Log(ouzel::Log::Level::ERR) << "Failed to close touch input handle";
        }
    }
    else
    {
        ouzel::Log(ouzel::Log::Level::ERR) << "Failed to get touch info";
    }
}

static LRESULT CALLBACK windowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    ouzel::WindowWin* windowWin = reinterpret_cast<ouzel::WindowWin*>(GetWindowLongPtr(window, GWLP_USERDATA));
    if (!windowWin) return DefWindowProcW(window, msg, wParam, lParam);

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
        case WM_TOUCH:
        {
            handleTouchEvent(wParam, lParam);
            return 0;
        }
        case WM_SETCURSOR:
        {
            if (LOWORD(lParam) == HTCLIENT)
            {
                if (!ouzel::sharedEngine->getInput()->isCursorVisible())
                {
                    SetCursor(nullptr);
                    return TRUE;
                }
            }
            break;
        }
        case WM_SIZE:
        {
            switch (wParam)
            {
                case SIZE_MINIMIZED:
                    ouzel::sharedEngine->pause();
                    break;
                case SIZE_RESTORED:
                    ouzel::sharedEngine->resume();
                    // fall through
                case SIZE_MAXIMIZED:
                    windowWin->handleResize(LOWORD(lParam), HIWORD(lParam));
                    break;
            }
            return 0;
        }
        case WM_ERASEBKGND:
        {
            if (ouzel::sharedEngine->getRenderer()->getDriver() != ouzel::graphics::Renderer::Driver::EMPTY)
            {
                return TRUE;
            }
            break;
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
        if (sharedEngine->getRenderer()->getDriver() == graphics::Renderer::Driver::EMPTY)
        {
            wc.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW);
        }
        else
        {
            wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        }
        wc.lpszClassName = WINDOW_CLASS_NAME;
        // Application icon should be the first resource
        wc.hIcon = LoadIconW(hInstance, MAKEINTRESOURCE(101));

        windowClass = RegisterClassExW(&wc);
        if (!windowClass)
        {
            Log(Log::Level::ERR) << "Failed to register window class";
            return false;
        }

        windowWindowedStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPSIBLINGS | WS_BORDER | WS_DLGFRAME | WS_THICKFRAME | WS_GROUP | WS_TABSTOP;

        if (resizable)
        {
            windowWindowedStyle |= WS_SIZEBOX | WS_MAXIMIZEBOX;
        }

        windowFullscreenStyle = WS_CLIPSIBLINGS | WS_GROUP | WS_TABSTOP;

        int x = CW_USEDEFAULT;
        int y = CW_USEDEFAULT;

        windowStyle = windowWindowedStyle;

        RECT windowRect = { 0, 0, static_cast<LONG>(size.width), static_cast<LONG>(size.height) };
        AdjustWindowRect(&windowRect, windowStyle, FALSE);

        int width = CW_USEDEFAULT;
        int height = CW_USEDEFAULT;
        if (size.width > 0.0f) width = windowRect.right - windowRect.left;
        if (size.height > 0.0f) height = windowRect.bottom - windowRect.top;

        wchar_t titleBuffer[256];
        MultiByteToWideChar(CP_UTF8, 0, title.c_str(), -1, titleBuffer, 256);

        window = CreateWindowExW(WS_EX_APPWINDOW, WINDOW_CLASS_NAME, titleBuffer, windowStyle,
                                 x, y, width, height, nullptr, nullptr, hInstance, nullptr);

        if (!window)
        {
            Log(Log::Level::ERR) << "Failed to create window";
            return false;
        }

        if (fullscreen)
        {
            switchFullscreen(fullscreen);
        }

        GetClientRect(window, &windowRect);
        size.width = static_cast<float>(windowRect.right - windowRect.left);
        size.height = static_cast<float>(windowRect.bottom - windowRect.top);

        if (!RegisterTouchWindow(window, 0))
        {
            Log(Log::Level::WARN) << "Failed to enable touch for window";
        }

        ShowWindow(window, SW_SHOW);
        SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

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
        sharedEngine->getRenderer()->setSize(size);
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

    void WindowWin::setFullscreen(bool newFullscreen)
    {
        Window::setFullscreen(newFullscreen);

        sharedApplication->execute([this, newFullscreen] {
            switchFullscreen(newFullscreen);
        });
    }

    void WindowWin::switchFullscreen(bool newFullscreen)
    {
        windowStyle = (newFullscreen ? windowFullscreenStyle : windowWindowedStyle) | WS_VISIBLE;
        SetWindowLong(window, GWL_STYLE, windowStyle);

        if (newFullscreen)
        {
            RECT windowRect;
            GetWindowRect(window, &windowRect);

            windowX = windowRect.left;
            windowY = windowRect.top;
            windowWidth = windowRect.right - windowRect.left;
            windowHeight = windowRect.bottom - windowRect.top;

            HMONITOR monitor = getMonitor();

            MONITORINFO info;
            info.cbSize = sizeof(MONITORINFO);
            GetMonitorInfo(monitor, &info);

            SetWindowPos(window, nullptr, info.rcMonitor.left, info.rcMonitor.top,
                         info.rcMonitor.right - info.rcMonitor.left,
                         info.rcMonitor.bottom - info.rcMonitor.top,
                         SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER);
        }
        else
        {
            SetWindowPos(window, nullptr, windowX, windowY, windowWidth, windowHeight,
                         SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER);
        }
    }

    void WindowWin::handleResize(INT width, INT height)
    {
        Window::setSize(Size2(static_cast<float>(width), static_cast<float>(height)));
        sharedEngine->getRenderer()->setSize(size);
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
