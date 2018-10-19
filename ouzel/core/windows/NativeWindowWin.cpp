// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <unordered_map>
#include <Windows.h>
#include <windowsx.h>
#include "NativeWindowWin.hpp"
#include "EngineWin.hpp"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "input/windows/InputSystemWin.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/RenderDevice.hpp"
#include "graphics/direct3d11/RenderDeviceD3D11.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

static const LONG_PTR SIGNATURE_MASK = 0x0FFFFFF00;
static const LONG_PTR MOUSEEVENTF_FROMTOUCH = 0x0FF515700;

static const std::unordered_map<UINT, ouzel::input::Keyboard::Key> keyMap = {
    {VK_CANCEL, ouzel::input::Keyboard::Key::CANCEL},
    {VK_BACK, ouzel::input::Keyboard::Key::BACKSPACE},
    {VK_TAB, ouzel::input::Keyboard::Key::TAB},
    {VK_CLEAR, ouzel::input::Keyboard::Key::CLEAR},
    {VK_RETURN, ouzel::input::Keyboard::Key::ENTER},

    {VK_MENU, ouzel::input::Keyboard::Key::LEFT_ALT},
    {VK_PAUSE, ouzel::input::Keyboard::Key::PAUSE},
    {VK_CAPITAL, ouzel::input::Keyboard::Key::CAPS_LOCK},

    {VK_ESCAPE, ouzel::input::Keyboard::Key::ESCAPE},
    {VK_SPACE, ouzel::input::Keyboard::Key::SPACE},
    {VK_PRIOR, ouzel::input::Keyboard::Key::PAGE_UP},
    {VK_NEXT, ouzel::input::Keyboard::Key::PAGE_DOWN},
    {VK_END, ouzel::input::Keyboard::Key::END},
    {VK_HOME, ouzel::input::Keyboard::Key::HOME},
    {VK_LEFT, ouzel::input::Keyboard::Key::LEFT},
    {VK_UP, ouzel::input::Keyboard::Key::UP},
    {VK_RIGHT, ouzel::input::Keyboard::Key::RIGHT},
    {VK_DOWN, ouzel::input::Keyboard::Key::DOWN},

    {VK_SELECT, ouzel::input::Keyboard::Key::SELECT},
    {VK_PRINT, ouzel::input::Keyboard::Key::PRINT},
    {VK_EXECUTE, ouzel::input::Keyboard::Key::EXECUTE},

    {VK_SNAPSHOT, ouzel::input::Keyboard::Key::PRINT_SCREEN},
    {VK_INSERT, ouzel::input::Keyboard::Key::INSERT},
    {VK_DELETE, ouzel::input::Keyboard::Key::DEL},
    {VK_HELP, ouzel::input::Keyboard::Key::HELP},

    {'0', ouzel::input::Keyboard::Key::NUM_0},
    {'1', ouzel::input::Keyboard::Key::NUM_1},
    {'2', ouzel::input::Keyboard::Key::NUM_2},
    {'3', ouzel::input::Keyboard::Key::NUM_3},
    {'4', ouzel::input::Keyboard::Key::NUM_4},
    {'5', ouzel::input::Keyboard::Key::NUM_5},
    {'6', ouzel::input::Keyboard::Key::NUM_6},
    {'7', ouzel::input::Keyboard::Key::NUM_7},
    {'8', ouzel::input::Keyboard::Key::NUM_8},
    {'9', ouzel::input::Keyboard::Key::NUM_9},

    {'A', ouzel::input::Keyboard::Key::A},
    {'B', ouzel::input::Keyboard::Key::B},
    {'C', ouzel::input::Keyboard::Key::C},
    {'D', ouzel::input::Keyboard::Key::D},
    {'E', ouzel::input::Keyboard::Key::E},
    {'F', ouzel::input::Keyboard::Key::F},
    {'G', ouzel::input::Keyboard::Key::G},
    {'H', ouzel::input::Keyboard::Key::H},
    {'I', ouzel::input::Keyboard::Key::I},
    {'J', ouzel::input::Keyboard::Key::J},
    {'K', ouzel::input::Keyboard::Key::K},
    {'L', ouzel::input::Keyboard::Key::L},
    {'M', ouzel::input::Keyboard::Key::M},
    {'N', ouzel::input::Keyboard::Key::N},
    {'O', ouzel::input::Keyboard::Key::O},
    {'P', ouzel::input::Keyboard::Key::P},
    {'Q', ouzel::input::Keyboard::Key::Q},
    {'R', ouzel::input::Keyboard::Key::R},
    {'S', ouzel::input::Keyboard::Key::S},
    {'T', ouzel::input::Keyboard::Key::T},
    {'U', ouzel::input::Keyboard::Key::U},
    {'V', ouzel::input::Keyboard::Key::V},
    {'W', ouzel::input::Keyboard::Key::W},
    {'X', ouzel::input::Keyboard::Key::X},
    {'Y', ouzel::input::Keyboard::Key::Y},
    {'Z', ouzel::input::Keyboard::Key::Z},

    {VK_LWIN, ouzel::input::Keyboard::Key::LEFT_SUPER},
    {VK_RWIN, ouzel::input::Keyboard::Key::RIGHT_SUPER},
    {VK_APPS, ouzel::input::Keyboard::Key::MENU},
    {VK_SLEEP, ouzel::input::Keyboard::Key::SLEEP},

    {VK_NUMPAD0, ouzel::input::Keyboard::Key::NUMPAD_0},
    {VK_NUMPAD1, ouzel::input::Keyboard::Key::NUMPAD_1},
    {VK_NUMPAD2, ouzel::input::Keyboard::Key::NUMPAD_2},
    {VK_NUMPAD3, ouzel::input::Keyboard::Key::NUMPAD_3},
    {VK_NUMPAD4, ouzel::input::Keyboard::Key::NUMPAD_4},
    {VK_NUMPAD5, ouzel::input::Keyboard::Key::NUMPAD_5},
    {VK_NUMPAD6, ouzel::input::Keyboard::Key::NUMPAD_6},
    {VK_NUMPAD7, ouzel::input::Keyboard::Key::NUMPAD_7},
    {VK_NUMPAD8, ouzel::input::Keyboard::Key::NUMPAD_8},
    {VK_NUMPAD9, ouzel::input::Keyboard::Key::NUMPAD_9},

    {VK_MULTIPLY, ouzel::input::Keyboard::Key::NUMPAD_MULTIPLY},
    {VK_ADD, ouzel::input::Keyboard::Key::NUMPAD_PLUS},
    {VK_SEPARATOR, ouzel::input::Keyboard::Key::NUMPAD_SEPARATOR},
    {VK_SUBTRACT, ouzel::input::Keyboard::Key::NUMPAD_MINUS},
    {VK_DECIMAL, ouzel::input::Keyboard::Key::NUMPAD_DECIMAL},
    {VK_DIVIDE, ouzel::input::Keyboard::Key::NUMPAD_DIVIDE},

    {VK_F1, ouzel::input::Keyboard::Key::F1},
    {VK_F2, ouzel::input::Keyboard::Key::F2},
    {VK_F3, ouzel::input::Keyboard::Key::F3},
    {VK_F4, ouzel::input::Keyboard::Key::F4},
    {VK_F5, ouzel::input::Keyboard::Key::F5},
    {VK_F6, ouzel::input::Keyboard::Key::F6},
    {VK_F7, ouzel::input::Keyboard::Key::F7},
    {VK_F8, ouzel::input::Keyboard::Key::F8},
    {VK_F9, ouzel::input::Keyboard::Key::F9},
    {VK_F10, ouzel::input::Keyboard::Key::F10},
    {VK_F11, ouzel::input::Keyboard::Key::F11},
    {VK_F12, ouzel::input::Keyboard::Key::F12},
    {VK_F13, ouzel::input::Keyboard::Key::F13},
    {VK_F14, ouzel::input::Keyboard::Key::F14},
    {VK_F15, ouzel::input::Keyboard::Key::F15},
    {VK_F16, ouzel::input::Keyboard::Key::F16},
    {VK_F17, ouzel::input::Keyboard::Key::F17},
    {VK_F18, ouzel::input::Keyboard::Key::F18},
    {VK_F19, ouzel::input::Keyboard::Key::F19},
    {VK_F20, ouzel::input::Keyboard::Key::F20},
    {VK_F21, ouzel::input::Keyboard::Key::F21},
    {VK_F22, ouzel::input::Keyboard::Key::F22},
    {VK_F23, ouzel::input::Keyboard::Key::F23},
    {VK_F24, ouzel::input::Keyboard::Key::F24},

    {VK_NUMLOCK, ouzel::input::Keyboard::Key::NUM_LOCK},
    {VK_SCROLL, ouzel::input::Keyboard::Key::SCROLL_LOCK},
    {VK_LSHIFT, ouzel::input::Keyboard::Key::LEFT_SHIFT},
    {VK_RSHIFT, ouzel::input::Keyboard::Key::RIGHT_SHIFT},
    {VK_LCONTROL, ouzel::input::Keyboard::Key::LEFT_CONTROL},
    {VK_RCONTROL, ouzel::input::Keyboard::Key::RIGHT_CONTROL},
    {VK_LMENU, ouzel::input::Keyboard::Key::LEFT_ALT},
    {VK_RMENU, ouzel::input::Keyboard::Key::RIGHT_ALT},

    {VK_OEM_1, ouzel::input::Keyboard::Key::SEMICOLON},
    {VK_OEM_PLUS, ouzel::input::Keyboard::Key::PLUS},
    {VK_OEM_COMMA, ouzel::input::Keyboard::Key::COMMA},
    {VK_OEM_MINUS, ouzel::input::Keyboard::Key::MINUS},
    {VK_OEM_PERIOD, ouzel::input::Keyboard::Key::PERIOD},
    {VK_OEM_2, ouzel::input::Keyboard::Key::SLASH},
    {VK_OEM_3, ouzel::input::Keyboard::Key::GRAVE},
    {VK_OEM_4, ouzel::input::Keyboard::Key::LEFT_BRACKET},
    {VK_OEM_5, ouzel::input::Keyboard::Key::BACKSLASH},
    {VK_OEM_6, ouzel::input::Keyboard::Key::RIGHT_BRACKET},
    {VK_OEM_7, ouzel::input::Keyboard::Key::QUOTE},
    {VK_OEM_8, ouzel::input::Keyboard::Key::GRAVE},
    {VK_OEM_AX, ouzel::input::Keyboard::Key::OEM_AX},
    {VK_OEM_102, ouzel::input::Keyboard::Key::LESS}
};

static ouzel::input::Keyboard::Key convertKeyCode(UINT keyCode)
{
    auto i = keyMap.find(keyCode);

    if (i != keyMap.end())
        return i->second;
    else
        return ouzel::input::Keyboard::Key::NONE;
}

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
            windowWin->handleKeyEvent(message, wParam, lParam);
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
                windowWin->handleMouseButtonEvent(message, wParam, lParam);

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
                windowWin->handleMouseMoveEvent(message, wParam, lParam);
                return 0;
            }
            break;
        }
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
        {
            windowWin->handleMouseWheelEvent(message, wParam, lParam);
            return 0;
        }
        case WM_TOUCH:
        {
            windowWin->handleTouchEvent(wParam, lParam);
            return 0;
        }
        case WM_SETCURSOR:
        {
            if (LOWORD(lParam) == HTCLIENT)
            {
                /*ouzel::input::InputManagerWin* inputWin = static_cast<ouzel::input::InputManagerWin*>(ouzel::engine->getInputManager());
                inputWin->updateCursor();*/
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
    NativeWindowWin::NativeWindowWin(EventHandler& initEventHandler,
                                     const Size2& newSize,
                                     bool newResizable,
                                     bool newFullscreen,
                                     bool newExclusiveFullscreen,
                                     const std::string& newTitle,
                                     bool newHighDpi):
        NativeWindow(initEventHandler,
                     newSize,
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

        postEvent(Event(Event::Type::RESOLUTION_CHANGE, resolution));
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

        postEvent(Event(Event::Type::SIZE_CHANGE, size));
        postEvent(Event(Event::Type::RESOLUTION_CHANGE, resolution));
    }

    void NativeWindowWin::handleMove()
    {
        monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
    }

    void NativeWindowWin::handleKeyEvent(UINT message, WPARAM wParam, LPARAM lParam)
    {
        input::InputSystemWin* inputSystemWin = static_cast<input::InputSystemWin*>(engine->getInputManager()->getInputSystem());
        input::KeyboardDeviceWin* keyboardDevice = inputSystemWin->getKeyboardDevice();

        UINT key = static_cast<UINT>(wParam);
        switch (key)
        {
            case VK_MENU:
                key = (lParam & 0x1000000) ? VK_RMENU : VK_LMENU;
                break;
            case VK_CONTROL:
                key = (lParam & 0x1000000) ? VK_RCONTROL : VK_LCONTROL;
                break;
            case VK_SHIFT:
                key = MapVirtualKey((lParam & 0x00ff0000) >> 16, MAPVK_VSC_TO_VK_EX);
                break;
        }

        if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN)
            keyboardDevice->handleKeyPress(convertKeyCode(key));
        else if (message == WM_KEYUP || message == WM_SYSKEYUP)
            keyboardDevice->handleKeyRelease(convertKeyCode(key));
    }

    void NativeWindowWin::handleMouseMoveEvent(UINT, WPARAM, LPARAM lParam)
    {
        input::InputSystemWin* inputSystemWin = static_cast<input::InputSystemWin*>(engine->getInputManager()->getInputSystem());
        input::MouseDeviceWin* mouseDevice = inputSystemWin->getMouseDevice();

        Vector2 position(static_cast<float>(GET_X_LPARAM(lParam)),
                         static_cast<float>(GET_Y_LPARAM(lParam)));

        mouseDevice->handleMove(engine->getWindow()->convertWindowToNormalizedLocation(position));
    }

    void NativeWindowWin::handleMouseButtonEvent(UINT message, WPARAM wParam, LPARAM lParam)
    {
        input::InputSystemWin* inputSystemWin = static_cast<input::InputSystemWin*>(engine->getInputManager()->getInputSystem());
        input::MouseDeviceWin* mouseDevice = inputSystemWin->getMouseDevice();

        Vector2 position(static_cast<float>(GET_X_LPARAM(lParam)),
                         static_cast<float>(GET_Y_LPARAM(lParam)));

        input::Mouse::Button button;

        if (message == WM_LBUTTONDOWN || message == WM_LBUTTONUP)
            button = input::Mouse::Button::LEFT;
        else if (message == WM_RBUTTONDOWN || message == WM_RBUTTONUP)
            button = input::Mouse::Button::RIGHT;
        else if (message == WM_MBUTTONDOWN || message == WM_MBUTTONUP)
            button = input::Mouse::Button::MIDDLE;
        else if (message == WM_XBUTTONDOWN || message == WM_XBUTTONUP)
        {
            if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
                button = input::Mouse::Button::X1;
            else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON2)
                button = input::Mouse::Button::X2;
            else
                return;
        }
        else
            return;

        if (message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN || message == WM_MBUTTONDOWN || message == WM_XBUTTONDOWN)
            mouseDevice->handleButtonPress(button,
                                           engine->getWindow()->convertWindowToNormalizedLocation(position));
        else if (message == WM_LBUTTONUP || message == WM_RBUTTONUP || message == WM_MBUTTONUP || message == WM_XBUTTONUP)
            mouseDevice->handleButtonRelease(button,
                                             engine->getWindow()->convertWindowToNormalizedLocation(position));
    }

    void NativeWindowWin::handleMouseWheelEvent(UINT message, WPARAM wParam, LPARAM lParam)
    {
        input::InputSystemWin* inputSystemWin = static_cast<input::InputSystemWin*>(engine->getInputManager()->getInputSystem());
        input::MouseDeviceWin* mouseDevice = inputSystemWin->getMouseDevice();

        Vector2 position(static_cast<float>(GET_X_LPARAM(lParam)),
                         static_cast<float>(GET_Y_LPARAM(lParam)));

        if (message == WM_MOUSEWHEEL)
        {
            short param = static_cast<short>(HIWORD(wParam));
            mouseDevice->handleScroll(Vector2(0.0F, -static_cast<float>(param) / static_cast<float>(WHEEL_DELTA)),
                                      engine->getWindow()->convertWindowToNormalizedLocation(position));
        }
        else if (message == WM_MOUSEHWHEEL)
        {
            short param = static_cast<short>(HIWORD(wParam));
            mouseDevice->handleScroll(Vector2(static_cast<float>(param) / static_cast<float>(WHEEL_DELTA), 0.0F),
                                      engine->getWindow()->convertWindowToNormalizedLocation(position));
        }
    }

    void NativeWindowWin::handleTouchEvent(WPARAM wParam, LPARAM lParam)
    {
        input::InputSystemWin* inputSystemWin = static_cast<input::InputSystemWin*>(engine->getInputManager()->getInputSystem());
        input::TouchpadDevice* touchpadDevice = inputSystemWin->getTouchpadDevice();

        UINT inputCount = LOWORD(wParam);
        std::vector<TOUCHINPUT> touches(inputCount);

        if (GetTouchInputInfo(reinterpret_cast<HTOUCHINPUT>(lParam), inputCount, touches.data(), sizeof(TOUCHINPUT)))
        {
            Vector2 position;

            for (const TOUCHINPUT& touch : touches)
            {
                position.x = static_cast<float>(touch.x / 100);
                position.y = static_cast<float>(touch.y / 100);

                if (touch.dwFlags & TOUCHEVENTF_DOWN)
                {
                    touchpadDevice->handleTouchBegin(touch.dwID,
                                                     engine->getWindow()->convertWindowToNormalizedLocation(position));
                }

                if (touch.dwFlags & TOUCHEVENTF_UP)
                {
                    touchpadDevice->handleTouchEnd(touch.dwID,
                                                   engine->getWindow()->convertWindowToNormalizedLocation(position));
                }

                if (touch.dwFlags & TOUCHEVENTF_MOVE)
                {
                    touchpadDevice->handleTouchMove(touch.dwID,
                                                    engine->getWindow()->convertWindowToNormalizedLocation(position));
                }
            }

            if (!CloseTouchInputHandle(reinterpret_cast<HTOUCHINPUT>(lParam)))
                throw SystemError("Failed to close touch input handle");
        }
        else
            throw SystemError("Failed to get touch info");
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
