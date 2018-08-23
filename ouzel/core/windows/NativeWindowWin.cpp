// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <windowsx.h>
#include "NativeWindowWin.hpp"
#include "EngineWin.hpp"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "input/windows/InputManagerWin.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/RenderDevice.hpp"
#include "graphics/direct3d11/RenderDeviceD3D11.hpp"
#include "thread/Lock.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

static void handleKeyEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
    UINT key = static_cast<UINT>(wParam);
    switch (key)
    {
        case VK_MENU:
            if ((lParam & 0x1000000) == 0)
                key = VK_LMENU;
            else
                key = VK_RMENU;
            break;
        case VK_CONTROL:
            if ((lParam & 0x1000000) == 0)
                key = VK_LCONTROL;
            else
                key = VK_RCONTROL;
            break;
        case VK_SHIFT:
            key = MapVirtualKey((lParam & 0x00ff0000) >> 16, MAPVK_VSC_TO_VK_EX);
            break;
    }

    if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN)
        ouzel::engine->getInputManager()->keyPress(ouzel::input::InputManagerWin::convertKeyCode(key), 0);
    else if (message == WM_KEYUP || message == WM_SYSKEYUP)
        ouzel::engine->getInputManager()->keyRelease(ouzel::input::InputManagerWin::convertKeyCode(key), 0);
}

static void handleMouseMoveEvent(UINT, WPARAM wParam, LPARAM lParam)
{
    ouzel::Vector2 position(static_cast<float>(GET_X_LPARAM(lParam)),
                            static_cast<float>(GET_Y_LPARAM(lParam)));

    ouzel::engine->getInputManager()->mouseMove(ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position),
                                                ouzel::input::InputManagerWin::getModifiers(wParam));
}

static void handleMouseButtonEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
    ouzel::Vector2 position(static_cast<float>(GET_X_LPARAM(lParam)),
                            static_cast<float>(GET_Y_LPARAM(lParam)));

    ouzel::input::MouseButton button;

    if (message == WM_LBUTTONDOWN || message == WM_LBUTTONUP)
        button = ouzel::input::MouseButton::LEFT;
    else if (message == WM_RBUTTONDOWN || message == WM_RBUTTONUP)
        button = ouzel::input::MouseButton::RIGHT;
    else if (message == WM_MBUTTONDOWN || message == WM_MBUTTONUP)
        button = ouzel::input::MouseButton::MIDDLE;
    else if (message == WM_XBUTTONDOWN || message == WM_XBUTTONUP)
    {
        if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
            button = ouzel::input::MouseButton::X1;
        else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON2)
            button = ouzel::input::MouseButton::X2;
        else
            return;
    }
    else
        return;

    if (message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN || message == WM_MBUTTONDOWN || message == WM_XBUTTONDOWN)
        ouzel::engine->getInputManager()->mouseButtonPress(button,
                                                           ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position),
                                                           ouzel::input::InputManagerWin::getModifiers(wParam));
    else if (message == WM_LBUTTONUP || message == WM_RBUTTONUP || message == WM_MBUTTONUP || message == WM_XBUTTONUP)
        ouzel::engine->getInputManager()->mouseButtonRelease(button,
                                                             ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position),
                                                             ouzel::input::InputManagerWin::getModifiers(wParam));
}

static void handleMouseWheelEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
    ouzel::Vector2 position(static_cast<float>(GET_X_LPARAM(lParam)),
                            static_cast<float>(GET_Y_LPARAM(lParam)));

    if (message == WM_MOUSEWHEEL)
    {
        short param = static_cast<short>(HIWORD(wParam));
        ouzel::engine->getInputManager()->mouseScroll(ouzel::Vector2(0.0F, -static_cast<float>(param) / static_cast<float>(WHEEL_DELTA)),
                                                      ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position),
                                                      ouzel::input::InputManagerWin::getModifiers(wParam));
    }
    else if (message == WM_MOUSEHWHEEL)
    {
        short param = static_cast<short>(HIWORD(wParam));
        ouzel::engine->getInputManager()->mouseScroll(ouzel::Vector2(static_cast<float>(param) / static_cast<float>(WHEEL_DELTA), 0.0F),
                                                      ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position),
                                                      ouzel::input::InputManagerWin::getModifiers(wParam));
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
            position.x = static_cast<float>(touch.x / 100);
            position.y = static_cast<float>(touch.y / 100);

            if (touch.dwFlags & TOUCHEVENTF_DOWN)
            {
                ouzel::engine->getInputManager()->touchBegin(touch.dwID,
                                                             ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position));
            }

            if (touch.dwFlags & TOUCHEVENTF_UP)
            {
                ouzel::engine->getInputManager()->touchEnd(touch.dwID,
                                                           ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position));
            }

            if (touch.dwFlags & TOUCHEVENTF_MOVE)
            {
                ouzel::engine->getInputManager()->touchMove(touch.dwID,
                                                            ouzel::engine->getWindow()->convertWindowToNormalizedLocation(position));
            }
        }

        if (!CloseTouchInputHandle(reinterpret_cast<HTOUCHINPUT>(lParam)))
            throw ouzel::SystemError("Failed to close touch input handle");
    }
    else
        throw ouzel::SystemError("Failed to get touch info");
}

static const LONG_PTR SIGNATURE_MASK = 0x0FFFFFF00;
static const LONG_PTR MOUSEEVENTF_FROMTOUCH = 0x0FF515700;

static LRESULT CALLBACK windowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    ouzel::NativeWindowWin* windowWin = reinterpret_cast<ouzel::NativeWindowWin*>(GetWindowLongPtr(window, GWLP_USERDATA));
    if (!windowWin) return DefWindowProcW(window, message, wParam, lParam);

    switch (message)
    {
        case WM_ACTIVATEAPP:
        {
            if (ouzel::engine)
            {
                if (wParam)
                    ouzel::engine->resume();
                else
                    ouzel::engine->pause();
            }
            break;
        }
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        {
            handleKeyEvent(message, wParam, lParam);
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
            LONG_PTR extraInfo = GetMessageExtraInfo();

            // don't handle mouse event that came from touch
            if ((extraInfo & SIGNATURE_MASK) != MOUSEEVENTF_FROMTOUCH)
            {
                handleMouseButtonEvent(message, wParam, lParam);

                // must return TRUE if WM_XBUTTONDOWN or WM_XBUTTONUP is handled
                if (message == WM_XBUTTONDOWN || message == WM_XBUTTONUP)
                    return TRUE;

                return 0;
            }
            break;
        }
        case WM_MOUSEMOVE:
        {
            LONG_PTR extraInfo = GetMessageExtraInfo();

            // don't handle mouse event that came from touch
            if ((extraInfo & SIGNATURE_MASK) != MOUSEEVENTF_FROMTOUCH)
            {
                handleMouseMoveEvent(message, wParam, lParam);
                return 0;
            }
            break;
        }
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
        {
            handleMouseWheelEvent(message, wParam, lParam);
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
                ouzel::input::InputManagerWin* inputWin = static_cast<ouzel::input::InputManagerWin*>(ouzel::engine->getInputManager());
                inputWin->updateCursor();
                return TRUE;
            }
            break;
        }
        case WM_SIZE:
        {
            switch (wParam)
            {
                case SIZE_MINIMIZED:
                    ouzel::engine->pause();
                    break;
                case SIZE_RESTORED:
                    ouzel::engine->resume();
                    windowWin->handleResize(ouzel::Size2(static_cast<float>(LOWORD(lParam)),
                                                         static_cast<float>(HIWORD(lParam))));
                    break;
                case SIZE_MAXIMIZED:
                    windowWin->handleResize(ouzel::Size2(static_cast<float>(LOWORD(lParam)),
                                                         static_cast<float>(HIWORD(lParam))));
                    break;
            }
            return 0;
        }
        case WM_MOVE:
        {
            windowWin->handleMove();
            break;
        }
        case WM_ERASEBKGND:
        {
            // Erase background only for the Empty renderer
            //if (ouzel::engine->getRenderer()->getDevice()->getDriver() != ouzel::graphics::Renderer::Driver::EMPTY)
            //    return TRUE;

            break;
        }
        case WM_SYSCOMMAND:
        {
            switch (wParam)
            {
                case SC_SCREENSAVE:
                case SC_MONITORPOWER:
                {
                    if (!ouzel::engine->isScreenSaverEnabled())
                    {
                        // Disable screensaver
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
        case WM_USER:
        {
            ouzel::EngineWin* engineWin = static_cast<ouzel::EngineWin*>(ouzel::engine);
            engineWin->executeAll();
            break;
        }
    }

    return DefWindowProcW(window, message, wParam, lParam);
}

static const LPCWSTR WINDOW_CLASS_NAME = L"OuzelWindow";

namespace ouzel
{
    NativeWindowWin::NativeWindowWin(const Size2& newSize,
                                     bool newResizable,
                                     bool newFullscreen,
                                     bool newExclusiveFullscreen,
                                     const std::string& newTitle,
                                     bool newHighDpi):
        ouzel::NativeWindow(newSize,
                            newResizable,
                            newFullscreen,
                            newExclusiveFullscreen,
                            newTitle,
                            newHighDpi)
    {
        if (highDpi)
        {
            HMODULE shcore = LoadLibraryW(L"shcore.dll");

            if (shcore)
            {
                typedef HRESULT(STDAPICALLTYPE *SetProcessDpiAwarenessProc)(int value);
                SetProcessDpiAwarenessProc setProcessDpiAwareness = reinterpret_cast<SetProcessDpiAwarenessProc>(GetProcAddress(shcore, "SetProcessDpiAwareness"));

                if (setProcessDpiAwareness)
                    setProcessDpiAwareness(2); // PROCESS_PER_MONITOR_DPI_AWARE
            }
        }

        HINSTANCE instance = GetModuleHandleW(nullptr);

        WNDCLASSEXW wc;
        wc.cbSize = sizeof(wc);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = windowProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = instance;
        // Application icon should be the first resource
        wc.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(101));
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = nullptr;
        wc.lpszMenuName = nullptr;
        wc.lpszClassName = WINDOW_CLASS_NAME;
        wc.hIconSm = nullptr;

        windowClass = RegisterClassExW(&wc);
        if (!windowClass)
            throw SystemError("Failed to register window class");

        windowWindowedStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPSIBLINGS | WS_BORDER | WS_DLGFRAME | WS_THICKFRAME | WS_GROUP | WS_TABSTOP;

        if (resizable)
            windowWindowedStyle |= WS_SIZEBOX | WS_MAXIMIZEBOX;

        windowFullscreenStyle = WS_CLIPSIBLINGS | WS_GROUP | WS_TABSTOP;

        int x = CW_USEDEFAULT;
        int y = CW_USEDEFAULT;

        windowStyle = windowWindowedStyle;
        windowExStyle = WS_EX_APPWINDOW;

        RECT windowRect = {0, 0, static_cast<LONG>(size.width), static_cast<LONG>(size.height)};
        AdjustWindowRectEx(&windowRect, windowStyle, FALSE, windowExStyle);

        int width = CW_USEDEFAULT;
        int height = CW_USEDEFAULT;
        if (size.width > 0.0F) width = windowRect.right - windowRect.left;
        if (size.height > 0.0F) height = windowRect.bottom - windowRect.top;

        wchar_t titleBuffer[256] = L"";

        if (!title.empty() && MultiByteToWideChar(CP_UTF8, 0, title.c_str(), -1, titleBuffer, 256) == 0)
            throw SystemError("Failed to convert UTF-8 to wide char");

        window = CreateWindowExW(windowExStyle, WINDOW_CLASS_NAME, titleBuffer, windowStyle,
                                 x, y, width, height, nullptr, nullptr, instance, nullptr);

        if (!window)
            throw SystemError("Failed to create window");

        monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);

        if (fullscreen)
            switchFullscreen(fullscreen);

        GetClientRect(window, &windowRect);
        size.width = static_cast<float>(windowRect.right - windowRect.left);
        size.height = static_cast<float>(windowRect.bottom - windowRect.top);
        resolution = size;

        if (!RegisterTouchWindow(window, 0))
            Log(Log::Level::WARN) << "Failed to enable touch for window";

        ShowWindow(window, SW_SHOW);
        SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    }

    NativeWindowWin::~NativeWindowWin()
    {
        if (window)
            DestroyWindow(window);

        if (windowClass)
            UnregisterClassW(WINDOW_CLASS_NAME, GetModuleHandleW(nullptr));
    }

    void NativeWindowWin::close()
    {
        NativeWindow::close();

        SendMessage(window, WM_CLOSE, 0, 0);
    }

    void NativeWindowWin::setSize(const Size2& newSize)
    {
        NativeWindow::setSize(newSize);

        UINT width = static_cast<UINT>(newSize.width);
        UINT height = static_cast<UINT>(newSize.height);

        UINT swpFlags = SWP_NOMOVE | SWP_NOZORDER;

        RECT rect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
        AdjustWindowRectEx(&rect, windowStyle, GetMenu(window) ? TRUE : FALSE, windowExStyle);

        SetWindowPos(window, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, swpFlags);

        resolution = size;

        addEvent(Event(Event::Type::RESOLUTION_CHANGE, resolution));
    }

    void NativeWindowWin::setTitle(const std::string& newTitle)
    {
        if (title != newTitle)
        {
            wchar_t titleBuffer[256] = L"";

            if (!newTitle.empty() && MultiByteToWideChar(CP_UTF8, 0, newTitle.c_str(), -1, titleBuffer, 256) == 0)
                throw SystemError("Failed to convert UTF-8 to wide char");

            SetWindowTextW(window, titleBuffer);
        }

        NativeWindow::setTitle(newTitle);
    }

    void NativeWindowWin::setFullscreen(bool newFullscreen)
    {
        NativeWindow::setFullscreen(newFullscreen);

        switchFullscreen(newFullscreen);
    }

    void NativeWindowWin::switchFullscreen(bool newFullscreen)
    {
        if (exclusiveFullscreen)
        {
            if (engine->getRenderer()->getDevice()->getDriver() == graphics::Renderer::Driver::DIRECT3D11)
            {
                graphics::RenderDeviceD3D11* renderDeviceD3D11 = static_cast<graphics::RenderDeviceD3D11*>(engine->getRenderer()->getDevice());
                renderDeviceD3D11->setFullscreen(newFullscreen);
            }
        }
        else
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
    }

    void NativeWindowWin::handleResize(const Size2& newSize)
    {
        monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);

        size = newSize;
        resolution = size;

        addEvent(Event(Event::Type::SIZE_CHANGE, size));
        addEvent(Event(Event::Type::RESOLUTION_CHANGE, resolution));
    }

    void NativeWindowWin::handleMove()
    {
        monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
    }

    void NativeWindowWin::addAccelerator(HACCEL accelerator)
    {
        engine->executeOnMainThread([this, accelerator]() {
            accelerators.insert(accelerator);
        });
    }

    void NativeWindowWin::removeAccelerator(HACCEL accelerator)
    {
        engine->executeOnMainThread([this, accelerator]() {
            accelerators.erase(accelerator);
        });
    }
}
