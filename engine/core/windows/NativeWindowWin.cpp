// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <system_error>
#include <unordered_map>
#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <Windows.h>
#include <windowsx.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

#include "NativeWindowWin.hpp"
#include "EngineWin.hpp"
#include "Library.hpp"
#include "core/Engine.hpp"
#include "core/Window.hpp"
#include "input/windows/InputSystemWin.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/RenderDevice.hpp"
#include "graphics/direct3d11/D3D11RenderDevice.hpp"
#include "utils/Log.hpp"

namespace
{
    constexpr LONG_PTR SIGNATURE_MASK = 0x0FFFFFF00;
    constexpr LONG_PTR MOUSEEVENTF_FROMTOUCH = 0x0FF515700;

    ouzel::input::Keyboard::Key convertKeyCode(LPARAM lParam, WPARAM wParam) noexcept
    {
        switch (wParam)
        {
            // virtual key codes
            case VK_CLEAR: return ouzel::input::Keyboard::Key::Clear;
            case VK_MODECHANGE: return ouzel::input::Keyboard::Key::ModeChange;
            case VK_SELECT: return ouzel::input::Keyboard::Key::Select;
            case VK_EXECUTE: return ouzel::input::Keyboard::Key::Execute;
            case VK_HELP: return ouzel::input::Keyboard::Key::Help;
            case VK_PAUSE: return ouzel::input::Keyboard::Key::Pause;
            case VK_NUMLOCK: return ouzel::input::Keyboard::Key::NumLock;

            case VK_F13: return ouzel::input::Keyboard::Key::F13;
            case VK_F14: return ouzel::input::Keyboard::Key::F14;
            case VK_F15: return ouzel::input::Keyboard::Key::F15;
            case VK_F16: return ouzel::input::Keyboard::Key::F16;
            case VK_F17: return ouzel::input::Keyboard::Key::F17;
            case VK_F18: return ouzel::input::Keyboard::Key::F18;
            case VK_F19: return ouzel::input::Keyboard::Key::F19;
            case VK_F20: return ouzel::input::Keyboard::Key::F20;
            case VK_F21: return ouzel::input::Keyboard::Key::F21;
            case VK_F22: return ouzel::input::Keyboard::Key::F22;
            case VK_F23: return ouzel::input::Keyboard::Key::F23;
            case VK_F24: return ouzel::input::Keyboard::Key::F24;

            case VK_OEM_NEC_EQUAL: return ouzel::input::Keyboard::Key::NumpadEqual;
            case VK_BROWSER_BACK: return ouzel::input::Keyboard::Key::Back;
            case VK_BROWSER_FORWARD: return ouzel::input::Keyboard::Key::Forward;
            case VK_BROWSER_REFRESH: return ouzel::input::Keyboard::Key::Refresh;
            case VK_BROWSER_STOP: return ouzel::input::Keyboard::Key::Stop;
            case VK_BROWSER_SEARCH: return ouzel::input::Keyboard::Key::Search;
            case VK_BROWSER_FAVORITES: return ouzel::input::Keyboard::Key::Bookmarks;
            case VK_BROWSER_HOME: return ouzel::input::Keyboard::Key::Home;
            case VK_VOLUME_MUTE: return ouzel::input::Keyboard::Key::Mute;
            case VK_VOLUME_DOWN: return ouzel::input::Keyboard::Key::VolumeDown;
            case VK_VOLUME_UP: return ouzel::input::Keyboard::Key::VolumeUp;

            case VK_MEDIA_NEXT_TRACK: return ouzel::input::Keyboard::Key::AudioNext;
            case VK_MEDIA_PREV_TRACK: return ouzel::input::Keyboard::Key::AudioPrevious;
            case VK_MEDIA_STOP: return ouzel::input::Keyboard::Key::AudioStop;
            case VK_MEDIA_PLAY_PAUSE: return ouzel::input::Keyboard::Key::AudioPlay;
            case VK_LAUNCH_MAIL: return ouzel::input::Keyboard::Key::Mail;
            case VK_LAUNCH_MEDIA_SELECT: return ouzel::input::Keyboard::Key::MediaSelect;

            case VK_OEM_102: return ouzel::input::Keyboard::Key::IntlBackslash;

            case VK_ATTN: return ouzel::input::Keyboard::Key::PrintScreen;
            case VK_CRSEL: return ouzel::input::Keyboard::Key::Crsel;
            case VK_EXSEL: return ouzel::input::Keyboard::Key::Exsel;
            case VK_OEM_CLEAR: return ouzel::input::Keyboard::Key::Clear;

            case VK_LAUNCH_APP1: return ouzel::input::Keyboard::Key::App1;
            case VK_LAUNCH_APP2: return ouzel::input::Keyboard::Key::App2;

            // scan codes
            default:
            {
                const uint8_t scanCode = (lParam >> 16) & 0xFF;
                if (scanCode <= 127)
                {
                    const bool isExtended = (lParam & (1 << 24)) != 0;

                    switch (scanCode)
                    {
                        case 0x01: return ouzel::input::Keyboard::Key::Escape;
                        case 0x02: return ouzel::input::Keyboard::Key::Num1;
                        case 0x03: return ouzel::input::Keyboard::Key::Num2;
                        case 0x04: return ouzel::input::Keyboard::Key::Num3;
                        case 0x05: return ouzel::input::Keyboard::Key::Num4;
                        case 0x06: return ouzel::input::Keyboard::Key::Num5;
                        case 0x07: return ouzel::input::Keyboard::Key::Num6;
                        case 0x08: return ouzel::input::Keyboard::Key::Num7;
                        case 0x09: return ouzel::input::Keyboard::Key::Num8;
                        case 0x0A: return ouzel::input::Keyboard::Key::Num9;
                        case 0x0B: return ouzel::input::Keyboard::Key::Num0;
                        case 0x0C: return ouzel::input::Keyboard::Key::Minus;
                        case 0x0D: return ouzel::input::Keyboard::Key::Equal;
                        case 0x0E: return ouzel::input::Keyboard::Key::Backspace;
                        case 0x0F: return ouzel::input::Keyboard::Key::Tab;
                        case 0x10: return ouzel::input::Keyboard::Key::Q;
                        case 0x11: return ouzel::input::Keyboard::Key::W;
                        case 0x12: return ouzel::input::Keyboard::Key::E;
                        case 0x13: return ouzel::input::Keyboard::Key::R;
                        case 0x14: return ouzel::input::Keyboard::Key::T;
                        case 0x15: return ouzel::input::Keyboard::Key::Y;
                        case 0x16: return ouzel::input::Keyboard::Key::U;
                        case 0x17: return ouzel::input::Keyboard::Key::I;
                        case 0x18: return ouzel::input::Keyboard::Key::O;
                        case 0x19: return ouzel::input::Keyboard::Key::P;
                        case 0x1A: return ouzel::input::Keyboard::Key::LeftBracket;
                        case 0x1B: return ouzel::input::Keyboard::Key::RightBracket;
                        case 0x1C: return isExtended ? ouzel::input::Keyboard::Key::NumpadEnter : ouzel::input::Keyboard::Key::Enter;
                        case 0x1D: return isExtended ? ouzel::input::Keyboard::Key::RightControl : ouzel::input::Keyboard::Key::LeftControl;
                        case 0x1E: return ouzel::input::Keyboard::Key::A;
                        case 0x1F: return ouzel::input::Keyboard::Key::S;
                        case 0x20: return ouzel::input::Keyboard::Key::D;
                        case 0x21: return ouzel::input::Keyboard::Key::F;
                        case 0x22: return ouzel::input::Keyboard::Key::G;
                        case 0x23: return ouzel::input::Keyboard::Key::H;
                        case 0x24: return ouzel::input::Keyboard::Key::J;
                        case 0x25: return ouzel::input::Keyboard::Key::K;
                        case 0x26: return ouzel::input::Keyboard::Key::L;
                        case 0x27: return ouzel::input::Keyboard::Key::Semicolon;
                        case 0x28: return ouzel::input::Keyboard::Key::Quote;
                        case 0x29: return ouzel::input::Keyboard::Key::Grave;
                        case 0x2A: return ouzel::input::Keyboard::Key::LeftShift;
                        case 0x2B: return ouzel::input::Keyboard::Key::Backslash;
                        case 0x2C: return ouzel::input::Keyboard::Key::Z;
                        case 0x2D: return ouzel::input::Keyboard::Key::X;
                        case 0x2E: return ouzel::input::Keyboard::Key::C;
                        case 0x2F: return ouzel::input::Keyboard::Key::V;
                        case 0x30: return ouzel::input::Keyboard::Key::B;
                        case 0x31: return ouzel::input::Keyboard::Key::N;
                        case 0x32: return ouzel::input::Keyboard::Key::M;
                        case 0x33: return ouzel::input::Keyboard::Key::Comma;
                        case 0x34: return ouzel::input::Keyboard::Key::Period;
                        case 0x35: return isExtended ? ouzel::input::Keyboard::Key::NumpadDivide : ouzel::input::Keyboard::Key::Slash;
                        case 0x36: return ouzel::input::Keyboard::Key::RightShift;
                        case 0x37: return isExtended ? ouzel::input::Keyboard::Key::PrintScreen : ouzel::input::Keyboard::Key::NumpadMultiply;
                        case 0x38: return isExtended ? ouzel::input::Keyboard::Key::RightAlt : ouzel::input::Keyboard::Key::LeftAlt;
                        case 0x39: return ouzel::input::Keyboard::Key::Space;
                        case 0x3A: return isExtended ? ouzel::input::Keyboard::Key::NumpadPlus : ouzel::input::Keyboard::Key::CapsLock;
                        case 0x3B: return ouzel::input::Keyboard::Key::F1;
                        case 0x3C: return ouzel::input::Keyboard::Key::F2;
                        case 0x3D: return ouzel::input::Keyboard::Key::F3;
                        case 0x3E: return ouzel::input::Keyboard::Key::F4;
                        case 0x3F: return ouzel::input::Keyboard::Key::F5;
                        case 0x40: return ouzel::input::Keyboard::Key::F6;
                        case 0x41: return ouzel::input::Keyboard::Key::F7;
                        case 0x42: return ouzel::input::Keyboard::Key::F8;
                        case 0x43: return ouzel::input::Keyboard::Key::F9;
                        case 0x44: return ouzel::input::Keyboard::Key::F10;
                        case 0x45: return ouzel::input::Keyboard::Key::NumLock;
                        case 0x46: return ouzel::input::Keyboard::Key::ScrollLock;
                        case 0x47: return isExtended ? ouzel::input::Keyboard::Key::Home : ouzel::input::Keyboard::Key::Numpad7;
                        case 0x48: return isExtended ? ouzel::input::Keyboard::Key::Up : ouzel::input::Keyboard::Key::Numpad8;
                        case 0x49: return isExtended ? ouzel::input::Keyboard::Key::PageUp : ouzel::input::Keyboard::Key::Numpad9;
                        case 0x4A: return ouzel::input::Keyboard::Key::NumpadMinus;
                        case 0x4B: return isExtended ? ouzel::input::Keyboard::Key::Left : ouzel::input::Keyboard::Key::Numpad4;
                        case 0x4C: return ouzel::input::Keyboard::Key::Numpad5;
                        case 0x4D: return isExtended ? ouzel::input::Keyboard::Key::Right : ouzel::input::Keyboard::Key::Numpad6;
                        case 0x4E: return ouzel::input::Keyboard::Key::NumpadPlus;
                        case 0x4F: return isExtended ? ouzel::input::Keyboard::Key::End : ouzel::input::Keyboard::Key::Numpad1;
                        case 0x50: return isExtended ? ouzel::input::Keyboard::Key::Down : ouzel::input::Keyboard::Key::Numpad2;
                        case 0x51: return isExtended ? ouzel::input::Keyboard::Key::PageDown : ouzel::input::Keyboard::Key::Numpad3;
                        case 0x52: return isExtended ? ouzel::input::Keyboard::Key::Insert : ouzel::input::Keyboard::Key::Numpad0;
                        case 0x53: return isExtended ? ouzel::input::Keyboard::Key::Del : ouzel::input::Keyboard::Key::NumpadDecimal;
                        case 0x54: return ouzel::input::Keyboard::Key::Unknown;
                        case 0x55: return ouzel::input::Keyboard::Key::Unknown;
                        case 0x56: return ouzel::input::Keyboard::Key::IntlBackslash;
                        case 0x57: return ouzel::input::Keyboard::Key::F11;
                        case 0x58: return ouzel::input::Keyboard::Key::F12;
                        case 0x59: return ouzel::input::Keyboard::Key::Pause;
                        case 0x5A: return ouzel::input::Keyboard::Key::Unknown;
                        case 0x5B: return ouzel::input::Keyboard::Key::LeftSuper;
                        case 0x5C: return ouzel::input::Keyboard::Key::RightSuper;
                        case 0x5D: return ouzel::input::Keyboard::Key::Menu;
                        case 0x5E: return ouzel::input::Keyboard::Key::Unknown;
                        case 0x5F: return ouzel::input::Keyboard::Key::Unknown;
                        case 0x60: return ouzel::input::Keyboard::Key::Unknown;
                        case 0x61: return ouzel::input::Keyboard::Key::Unknown;
                        case 0x62: return ouzel::input::Keyboard::Key::Unknown;
                        case 0x63: return ouzel::input::Keyboard::Key::Unknown;
                        case 0x64: return ouzel::input::Keyboard::Key::F13;
                        case 0x65: return ouzel::input::Keyboard::Key::F14;
                        case 0x66: return ouzel::input::Keyboard::Key::F15;
                        case 0x67: return ouzel::input::Keyboard::Key::F16;
                        case 0x68: return ouzel::input::Keyboard::Key::F17;
                        case 0x69: return ouzel::input::Keyboard::Key::F18;
                        case 0x6A: return ouzel::input::Keyboard::Key::F19;
                        case 0x6B: return ouzel::input::Keyboard::Key::Unknown;
                        case 0x6C: return ouzel::input::Keyboard::Key::Unknown;
                        case 0x6D: return ouzel::input::Keyboard::Key::Unknown;
                        case 0x6E: return ouzel::input::Keyboard::Key::Unknown;
                        case 0x6F: return ouzel::input::Keyboard::Key::Unknown;
                        case 0x70: return ouzel::input::Keyboard::Key::KatakanaHiragana;
                        case 0x71: return ouzel::input::Keyboard::Key::Unknown;
                        case 0x72: return ouzel::input::Keyboard::Key::Unknown;
                        case 0x73: return ouzel::input::Keyboard::Key::Ro;
                        case 0x74: return ouzel::input::Keyboard::Key::Unknown;
                        case 0x75: return ouzel::input::Keyboard::Key::Unknown;
                        case 0x76: return ouzel::input::Keyboard::Key::Unknown;
                        case 0x77: return ouzel::input::Keyboard::Key::Unknown;
                        case 0x78: return ouzel::input::Keyboard::Key::Unknown;
                        case 0x79: return ouzel::input::Keyboard::Key::Henkan;
                        case 0x7A: return ouzel::input::Keyboard::Key::Unknown;
                        case 0x7B: return ouzel::input::Keyboard::Key::Muhenkan;
                        case 0x7C: return ouzel::input::Keyboard::Key::Unknown;
                        case 0x7D: return ouzel::input::Keyboard::Key::Yen;
                        case 0x7E: return ouzel::input::Keyboard::Key::Unknown;
                        case 0x7F: return ouzel::input::Keyboard::Key::Unknown;
                        default: return ouzel::input::Keyboard::Key::Unknown;
                    }
                }
                else
                    return ouzel::input::Keyboard::Key::Unknown;
            }
        }
    }

    LRESULT CALLBACK windowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
        auto userData = GetWindowLongPtr(window, GWLP_USERDATA);
        if (!userData) return DefWindowProcW(window, message, wParam, lParam);

        ouzel::NativeWindowWin* windowWin;
        memcpy(&windowWin, &userData, sizeof(windowWin));

        switch (message)
        {
            case WM_ACTIVATEAPP:
                windowWin->handleActivate(wParam);
                break;
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
                windowWin->handleKey(message, wParam, lParam);
                break;
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
                    windowWin->handleMouseButton(message, wParam, lParam);

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
                    windowWin->handleMouseMove(lParam);
                    return 0;
                }
                break;
            }
            case WM_MOUSEWHEEL:
            case WM_MOUSEHWHEEL:
                windowWin->handleMouseWheel(message, wParam, lParam);
                return 0;
            case WM_TOUCH:
                windowWin->handleTouch(wParam, lParam);
                return 0;
            case WM_SETCURSOR:
            {
                if (LOWORD(lParam) == HTCLIENT)
                {
                    ouzel::input::InputSystemWin* inputSystemWin = static_cast<ouzel::input::InputSystemWin*>(ouzel::engine->getInputManager()->getInputSystem());
                    inputSystemWin->updateCursor();
                    return TRUE;
                }
                break;
            }
            case WM_SHOWWINDOW:
            {
                windowWin->handleShowWindow(wParam ? TRUE : FALSE);
                break;
            }
            case WM_SIZE:
            {
                switch (wParam)
                {
                    case SIZE_MINIMIZED:
                        windowWin->handleMinimize();
                        break;
                    case SIZE_RESTORED:
                        windowWin->handleResize(ouzel::Size2U(static_cast<uint32_t>(LOWORD(lParam)),
                                                            static_cast<uint32_t>(HIWORD(lParam))));
                        windowWin->handleRestore();
                        break;
                    case SIZE_MAXIMIZED:
                        windowWin->handleResize(ouzel::Size2U(static_cast<uint32_t>(LOWORD(lParam)),
                                                            static_cast<uint32_t>(HIWORD(lParam))));
                        windowWin->handleMaximize();
                        break;
                }
                return 0;
            }
            case WM_MOVE:
                windowWin->handleMove();
                break;
            case WM_ERASEBKGND:
            {
                // Erase background only for the Empty renderer
                //if (ouzel::engine->getRenderer()->getDevice()->getDriver() != ouzel::graphics::Driver::Empty)
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
                PostQuitMessage(0);
                return 0;
            case WM_USER:
            {
                ouzel::EngineWin* engineWin = static_cast<ouzel::EngineWin*>(ouzel::engine);
                engineWin->executeAll();
                break;
            }
        }

        return DefWindowProcW(window, message, wParam, lParam);
    }

    constexpr LPCWSTR WINDOW_CLASS_NAME = L"OuzelWindow";
}

namespace ouzel
{
    NativeWindowWin::NativeWindowWin(const std::function<void(const Event&)>& initCallback,
                                     const Size2U& newSize,
                                     bool newResizable,
                                     bool newFullscreen,
                                     bool newExclusiveFullscreen,
                                     const std::string& newTitle,
                                     bool newHighDpi):
        NativeWindow(initCallback,
                     newSize,
                     newResizable,
                     newFullscreen,
                     newExclusiveFullscreen,
                     newTitle,
                     newHighDpi)
    {
        if (highDpi)
        {
            Library shcoreModule("shcore.dll");

            if (shcoreModule)
            {
                using SetProcessDpiAwarenessProc = HRESULT(STDAPICALLTYPE *)(int value);
                SetProcessDpiAwarenessProc setProcessDpiAwarenessProc = reinterpret_cast<SetProcessDpiAwarenessProc>(GetProcAddress(shcoreModule.get(), "SetProcessDpiAwareness"));

                constexpr int PROCESS_PER_MONITOR_DPI_AWARE = 2;
                if (setProcessDpiAwarenessProc)
                    setProcessDpiAwarenessProc(PROCESS_PER_MONITOR_DPI_AWARE);
            }
        }

        HINSTANCE instance = GetModuleHandleW(nullptr);
        if (!instance)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to get module handle");

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
            throw std::system_error(GetLastError(), std::system_category(), "Failed to register window class");

        windowWindowedStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPSIBLINGS | WS_BORDER | WS_DLGFRAME | WS_THICKFRAME | WS_GROUP | WS_TABSTOP;

        if (resizable)
            windowWindowedStyle |= WS_SIZEBOX | WS_MAXIMIZEBOX;

        windowFullscreenStyle = WS_CLIPSIBLINGS | WS_GROUP | WS_TABSTOP;

        const int x = CW_USEDEFAULT;
        const int y = CW_USEDEFAULT;

        windowStyle = windowWindowedStyle;
        windowExStyle = WS_EX_APPWINDOW;

        RECT windowRect = {0, 0, static_cast<LONG>(size.v[0]), static_cast<LONG>(size.v[1])};
        if (!AdjustWindowRectEx(&windowRect, windowStyle, FALSE, windowExStyle))
            throw std::system_error(GetLastError(), std::system_category(), "Failed to adjust window rectangle");

        const int width = (size.v[0] > 0.0F) ? windowRect.right - windowRect.left : CW_USEDEFAULT;
        const int height = (size.v[1] > 0.0F) ? windowRect.bottom - windowRect.top : CW_USEDEFAULT;

        const int titleBufferSize = MultiByteToWideChar(CP_UTF8, 0, title.c_str(), -1, nullptr, 0);
        if (titleBufferSize == 0)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

        std::vector<WCHAR> titleBuffer(titleBufferSize);
        if (MultiByteToWideChar(CP_UTF8, 0, title.c_str(), -1, titleBuffer.data(), titleBufferSize) == 0)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

        window = CreateWindowExW(windowExStyle, WINDOW_CLASS_NAME, titleBuffer.data(), windowStyle,
                                 x, y, width, height, nullptr, nullptr, instance, nullptr);

        if (!window)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to create window");

        monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);

        if (fullscreen)
            switchFullscreen(fullscreen);

        if (!GetClientRect(window, &windowRect))
            throw std::system_error(GetLastError(), std::system_category(), "Failed to get client rectangle");

        size.v[0] = static_cast<uint32_t>(windowRect.right - windowRect.left);
        size.v[1] = static_cast<uint32_t>(windowRect.bottom - windowRect.top);
        resolution = size;

        if (!RegisterTouchWindow(window, 0))
            engine->log(Log::Level::Warning) << "Failed to enable touch for window";

        ShowWindow(window, SW_SHOW);

        SetLastError(ERROR_SUCCESS);

        ouzel::NativeWindowWin* windowWin = this;
        LONG_PTR userData;
        memcpy(&userData, &windowWin, sizeof(windowWin));

        if (!SetWindowLongPtr(window, GWLP_USERDATA, userData))
            if (DWORD error = GetLastError())
                throw std::system_error(error, std::system_category(), "Failed to set window pointer");
    }

    NativeWindowWin::~NativeWindowWin()
    {
        if (window)
            DestroyWindow(window);

        if (windowClass)
            UnregisterClassW(WINDOW_CLASS_NAME, GetModuleHandleW(nullptr));
    }

    void NativeWindowWin::executeCommand(const Command& command)
    {
        switch (command.type)
        {
            case Command::Type::ChangeSize:
                setSize(command.size);
                break;
            case Command::Type::ChangeFullscreen:
                setFullscreen(command.fullscreen);
                break;
            case Command::Type::Close:
                close();
                break;
            case Command::Type::SetTitle:
                setTitle(command.title);
                break;
            case Command::Type::BringToFront:
                bringToFront();
                break;
            case Command::Type::Show:
                show();
                break;
            case Command::Type::Hide:
                hide();
                break;
            case Command::Type::Minimize:
                minimize();
                break;
            case Command::Type::Maximize:
                maximize();
                break;
            case Command::Type::Restore:
                restore();
                break;
            default:
                throw std::runtime_error("Invalid command");
        }
    }

    void NativeWindowWin::close()
    {
        SendMessage(window, WM_CLOSE, 0, 0);
    }

    void NativeWindowWin::setSize(const Size2U& newSize)
    {
        size = newSize;

        const auto width = static_cast<UINT>(newSize.v[0]);
        const auto height = static_cast<UINT>(newSize.v[1]);

        const UINT swpFlags = SWP_NOMOVE | SWP_NOZORDER;

        RECT rect = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
        if (!AdjustWindowRectEx(&rect, windowStyle, GetMenu(window) ? TRUE : FALSE, windowExStyle))
            throw std::system_error(GetLastError(), std::system_category(), "Failed to adjust window rectangle");

        if (!SetWindowPos(window, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, swpFlags))
            throw std::system_error(GetLastError(), std::system_category(), "Failed to set window position");

        resolution = size;

        Event resolutionChangeEvent(Event::Type::ResolutionChange);
        resolutionChangeEvent.size = resolution;
        sendEvent(resolutionChangeEvent);
    }

    void NativeWindowWin::setFullscreen(bool newFullscreen)
    {
        fullscreen = newFullscreen;

        switchFullscreen(newFullscreen);
    }

    void NativeWindowWin::setTitle(const std::string& newTitle)
    {
        if (title != newTitle)
        {
            const int titleBufferSize = MultiByteToWideChar(CP_UTF8, 0, newTitle.c_str(), -1, nullptr, 0);
            if (titleBufferSize == 0)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

            std::vector<WCHAR> titleBuffer(titleBufferSize);
            if (MultiByteToWideChar(CP_UTF8, 0, newTitle.c_str(), -1, titleBuffer.data(), titleBufferSize) == 0)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

            if (!SetWindowTextW(window, titleBuffer.data()))
                throw std::system_error(GetLastError(), std::system_category(), "Failed to set window title");

            title = newTitle;
        }
    }

    void NativeWindowWin::bringToFront()
    {
        SetForegroundWindow(window);
    }

    void NativeWindowWin::show()
    {
        ShowWindow(window, SW_SHOW);
    }

    void NativeWindowWin::hide()
    {
        ShowWindow(window, SW_HIDE);
    }

    void NativeWindowWin::minimize()
    {
        ShowWindow(window, SW_MINIMIZE);
    }

    void NativeWindowWin::maximize()
    {
        ShowWindow(window, SW_MAXIMIZE);
    }

    void NativeWindowWin::restore()
    {
        ShowWindow(window, SW_RESTORE);
    }

    void NativeWindowWin::switchFullscreen(bool newFullscreen)
    {
        if (exclusiveFullscreen)
        {
            if (engine->getRenderer()->getDevice()->getDriver() == graphics::Driver::Direct3D11)
            {
                graphics::d3d11::RenderDevice* renderDevice = static_cast<graphics::d3d11::RenderDevice*>(engine->getRenderer()->getDevice());
                renderDevice->setFullscreen(newFullscreen);
            }
        }
        else
        {
            windowStyle = (newFullscreen ? windowFullscreenStyle : windowWindowedStyle) | WS_VISIBLE;
            if (!SetWindowLong(window, GWL_STYLE, windowStyle))
                throw std::system_error(GetLastError(), std::system_category(), "Failed to set window style");

            if (newFullscreen)
            {
                RECT windowRect;
                if (!GetWindowRect(window, &windowRect))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to get window rectangle");

                windowX = windowRect.left;
                windowY = windowRect.top;
                windowWidth = windowRect.right - windowRect.left;
                windowHeight = windowRect.bottom - windowRect.top;

                MONITORINFO info;
                info.cbSize = sizeof(MONITORINFO);
                if (!GetMonitorInfo(monitor, &info))
                    throw std::runtime_error("Failed to get monitor info");

                if (!SetWindowPos(window, nullptr, info.rcMonitor.left, info.rcMonitor.top,
                                  info.rcMonitor.right - info.rcMonitor.left,
                                  info.rcMonitor.bottom - info.rcMonitor.top,
                                  SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to set window position");
            }
            else
            {
                if (!SetWindowPos(window, nullptr, windowX, windowY, windowWidth, windowHeight,
                                  SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOOWNERZORDER))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to set window position");
            }
        }
    }

    void NativeWindowWin::handleResize(const Size2U& newSize)
    {
        monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);

        size = newSize;
        resolution = size;

        Event sizeChangeEvent(Event::Type::SizeChange);
        sizeChangeEvent.size = size;
        sendEvent(sizeChangeEvent);

        Event resolutionChangeEvent(Event::Type::ResolutionChange);
        resolutionChangeEvent.size = resolution;
        sendEvent(resolutionChangeEvent);
    }

    void NativeWindowWin::handleMove()
    {
        monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
    }

    void NativeWindowWin::handleActivate(WPARAM wParam)
    {
        Event focusChangeEvent(Event::Type::FocusChange);
        focusChangeEvent.focus = wParam != 0;
        sendEvent(focusChangeEvent);

        if (wParam)
        {
                input::InputSystemWin* inputSystemWin = static_cast<input::InputSystemWin*>(engine->getInputManager()->getInputSystem());
                input::MouseDeviceWin* mouseDevice = inputSystemWin->getMouseDevice();

                POINT cursorPos;
                if (!GetCursorPos(&cursorPos))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to get cursor position");

                Vector2F position(static_cast<float>(cursorPos.x),
                                  static_cast<float>(cursorPos.y));
                mouseDevice->handleMove(engine->getWindow()->convertWindowToNormalizedLocation(position));
        }
    }

    void NativeWindowWin::handleShowWindow(BOOL shown)
    {
        sendEvent(Event(shown ? Event::Type::Show : Event::Type::Hide));
    }

    void NativeWindowWin::handleMinimize()
    {
        Event focusChangeEvent(Event::Type::FocusChange);
        focusChangeEvent.focus = false;
        sendEvent(focusChangeEvent);

        sendEvent(Event(Event::Type::Minimize));
    }

    void NativeWindowWin::handleMaximize()
    {
        sendEvent(Event(Event::Type::Maximize));
    }

    void NativeWindowWin::handleRestore()
    {
        Event focusChangeEvent(Event::Type::FocusChange);
        focusChangeEvent.focus = true;
        sendEvent(focusChangeEvent);

        input::InputSystemWin* inputSystemWin = static_cast<input::InputSystemWin*>(engine->getInputManager()->getInputSystem());
        input::MouseDeviceWin* mouseDevice = inputSystemWin->getMouseDevice();

        POINT cursorPos;
        if (!GetCursorPos(&cursorPos))
            throw std::system_error(GetLastError(), std::system_category(), "Failed to get cursor position");

        Vector2F position(static_cast<float>(cursorPos.x),
                          static_cast<float>(cursorPos.y));
        mouseDevice->handleMove(engine->getWindow()->convertWindowToNormalizedLocation(position));

        sendEvent(Event(Event::Type::Restore));
    }

    void NativeWindowWin::handleKey(UINT message, WPARAM wParam, LPARAM lParam)
    {
        input::InputSystemWin* inputSystemWin = static_cast<input::InputSystemWin*>(engine->getInputManager()->getInputSystem());
        input::KeyboardDeviceWin* keyboardDevice = inputSystemWin->getKeyboardDevice();

        if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN)
            keyboardDevice->handleKeyPress(convertKeyCode(lParam, wParam));
        else if (message == WM_KEYUP || message == WM_SYSKEYUP)
            keyboardDevice->handleKeyRelease(convertKeyCode(lParam, wParam));
    }

    void NativeWindowWin::handleMouseMove(LPARAM lParam)
    {
        input::InputSystemWin* inputSystemWin = static_cast<input::InputSystemWin*>(engine->getInputManager()->getInputSystem());
        input::MouseDeviceWin* mouseDevice = inputSystemWin->getMouseDevice();

        Vector2F position(static_cast<float>(GET_X_LPARAM(lParam)),
                          static_cast<float>(GET_Y_LPARAM(lParam)));

        mouseDevice->handleMove(engine->getWindow()->convertWindowToNormalizedLocation(position));
    }

    void NativeWindowWin::handleMouseButton(UINT message, WPARAM wParam, LPARAM lParam)
    {
        input::InputSystemWin* inputSystemWin = static_cast<input::InputSystemWin*>(engine->getInputManager()->getInputSystem());
        input::MouseDeviceWin* mouseDevice = inputSystemWin->getMouseDevice();

        Vector2F position(static_cast<float>(GET_X_LPARAM(lParam)),
                          static_cast<float>(GET_Y_LPARAM(lParam)));

        input::Mouse::Button button;

        if (message == WM_LBUTTONDOWN || message == WM_LBUTTONUP)
            button = input::Mouse::Button::Left;
        else if (message == WM_RBUTTONDOWN || message == WM_RBUTTONUP)
            button = input::Mouse::Button::Right;
        else if (message == WM_MBUTTONDOWN || message == WM_MBUTTONUP)
            button = input::Mouse::Button::Middle;
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

    void NativeWindowWin::handleMouseWheel(UINT message, WPARAM wParam, LPARAM lParam)
    {
        input::InputSystemWin* inputSystemWin = static_cast<input::InputSystemWin*>(engine->getInputManager()->getInputSystem());
        input::MouseDeviceWin* mouseDevice = inputSystemWin->getMouseDevice();

        Vector2F position(static_cast<float>(GET_X_LPARAM(lParam)),
                          static_cast<float>(GET_Y_LPARAM(lParam)));

        if (message == WM_MOUSEWHEEL)
        {
            auto param = static_cast<short>(HIWORD(wParam));
            mouseDevice->handleScroll(Vector2F(0.0F, -static_cast<float>(param) / static_cast<float>(WHEEL_DELTA)),
                                      engine->getWindow()->convertWindowToNormalizedLocation(position));
        }
        else if (message == WM_MOUSEHWHEEL)
        {
            auto param = static_cast<short>(HIWORD(wParam));
            mouseDevice->handleScroll(Vector2F(static_cast<float>(param) / static_cast<float>(WHEEL_DELTA), 0.0F),
                                      engine->getWindow()->convertWindowToNormalizedLocation(position));
        }
    }

    void NativeWindowWin::handleTouch(WPARAM wParam, LPARAM lParam)
    {
        input::InputSystemWin* inputSystemWin = static_cast<input::InputSystemWin*>(engine->getInputManager()->getInputSystem());
        input::TouchpadDevice* touchpadDevice = inputSystemWin->getTouchpadDevice();

        const UINT inputCount = LOWORD(wParam);
        std::vector<TOUCHINPUT> touches(inputCount);

        if (!GetTouchInputInfo(reinterpret_cast<HTOUCHINPUT>(lParam), inputCount, touches.data(), sizeof(TOUCHINPUT)))
            throw std::system_error(GetLastError(), std::system_category(), "Failed to get touch info");

        Vector2F position;

        for (const TOUCHINPUT& touch : touches)
        {
            position.v[0] = static_cast<float>(touch.x / 100);
            position.v[1] = static_cast<float>(touch.y / 100);

            if (touch.dwFlags & TOUCHEVENTF_DOWN)
                touchpadDevice->handleTouchBegin(touch.dwID,
                                                 engine->getWindow()->convertWindowToNormalizedLocation(position));

            if (touch.dwFlags & TOUCHEVENTF_UP)
                touchpadDevice->handleTouchEnd(touch.dwID,
                                               engine->getWindow()->convertWindowToNormalizedLocation(position));

            if (touch.dwFlags & TOUCHEVENTF_MOVE)
                touchpadDevice->handleTouchMove(touch.dwID,
                                                engine->getWindow()->convertWindowToNormalizedLocation(position));
        }

        if (!CloseTouchInputHandle(reinterpret_cast<HTOUCHINPUT>(lParam)))
            throw std::system_error(GetLastError(), std::system_category(), "Failed to close touch input handle");
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
