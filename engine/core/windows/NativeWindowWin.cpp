// Ouzel by Elviss Strazdins

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
#include <shellscalingapi.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

#include "NativeWindowWin.hpp"
#include "EngineWin.hpp"
#include "../../platform/winapi/Library.hpp"
#include "../Engine.hpp"
#include "../Window.hpp"
#include "../../input/windows/InputSystemWin.hpp"
#include "../../graphics/Graphics.hpp"
#include "../../graphics/RenderDevice.hpp"
#include "../../graphics/direct3d11/D3D11RenderDevice.hpp"
#include "../../utils/Log.hpp"
#include "../../utils/Utils.hpp"

namespace
{
    constexpr LONG_PTR SIGNATURE_MASK = 0xFFFFFF00;
    constexpr LONG_PTR MI_WP_SIGNATURE = 0xFF515700;

    ouzel::input::Keyboard::Key convertKeyCode(LPARAM lParam, WPARAM wParam) noexcept
    {
        switch (wParam)
        {
            // virtual key codes
            case VK_CLEAR: return ouzel::input::Keyboard::Key::clear;
            case VK_MODECHANGE: return ouzel::input::Keyboard::Key::modeChange;
            case VK_SELECT: return ouzel::input::Keyboard::Key::select;
            case VK_EXECUTE: return ouzel::input::Keyboard::Key::execute;
            case VK_HELP: return ouzel::input::Keyboard::Key::help;
            case VK_PAUSE: return ouzel::input::Keyboard::Key::pause;
            case VK_NUMLOCK: return ouzel::input::Keyboard::Key::numLock;

            case VK_F13: return ouzel::input::Keyboard::Key::f13;
            case VK_F14: return ouzel::input::Keyboard::Key::f14;
            case VK_F15: return ouzel::input::Keyboard::Key::f15;
            case VK_F16: return ouzel::input::Keyboard::Key::f16;
            case VK_F17: return ouzel::input::Keyboard::Key::f17;
            case VK_F18: return ouzel::input::Keyboard::Key::f18;
            case VK_F19: return ouzel::input::Keyboard::Key::f19;
            case VK_F20: return ouzel::input::Keyboard::Key::f20;
            case VK_F21: return ouzel::input::Keyboard::Key::f21;
            case VK_F22: return ouzel::input::Keyboard::Key::f22;
            case VK_F23: return ouzel::input::Keyboard::Key::f23;
            case VK_F24: return ouzel::input::Keyboard::Key::f24;

            case VK_OEM_NEC_EQUAL: return ouzel::input::Keyboard::Key::numpadEqual;
            case VK_BROWSER_BACK: return ouzel::input::Keyboard::Key::back;
            case VK_BROWSER_FORWARD: return ouzel::input::Keyboard::Key::forward;
            case VK_BROWSER_REFRESH: return ouzel::input::Keyboard::Key::refresh;
            case VK_BROWSER_STOP: return ouzel::input::Keyboard::Key::stop;
            case VK_BROWSER_SEARCH: return ouzel::input::Keyboard::Key::search;
            case VK_BROWSER_FAVORITES: return ouzel::input::Keyboard::Key::bookmarks;
            case VK_BROWSER_HOME: return ouzel::input::Keyboard::Key::home;
            case VK_VOLUME_MUTE: return ouzel::input::Keyboard::Key::mute;
            case VK_VOLUME_DOWN: return ouzel::input::Keyboard::Key::volumeDown;
            case VK_VOLUME_UP: return ouzel::input::Keyboard::Key::volumeUp;

            case VK_MEDIA_NEXT_TRACK: return ouzel::input::Keyboard::Key::audioNext;
            case VK_MEDIA_PREV_TRACK: return ouzel::input::Keyboard::Key::audioPrevious;
            case VK_MEDIA_STOP: return ouzel::input::Keyboard::Key::audioStop;
            case VK_MEDIA_PLAY_PAUSE: return ouzel::input::Keyboard::Key::audioPlay;
            case VK_LAUNCH_MAIL: return ouzel::input::Keyboard::Key::mail;
            case VK_LAUNCH_MEDIA_SELECT: return ouzel::input::Keyboard::Key::mediaSelect;

            case VK_OEM_102: return ouzel::input::Keyboard::Key::intlBackslash;

            case VK_ATTN: return ouzel::input::Keyboard::Key::printScreen;
            case VK_CRSEL: return ouzel::input::Keyboard::Key::crsel;
            case VK_EXSEL: return ouzel::input::Keyboard::Key::exsel;
            case VK_OEM_CLEAR: return ouzel::input::Keyboard::Key::clear;

            case VK_LAUNCH_APP1: return ouzel::input::Keyboard::Key::app1;
            case VK_LAUNCH_APP2: return ouzel::input::Keyboard::Key::app2;

            // scan codes
            default:
            {
                const std::uint8_t scanCode = (lParam >> 16) & 0xFF;
                if (scanCode <= 127)
                {
                    const bool isExtended = (lParam & (1 << 24)) != 0;

                    switch (scanCode)
                    {
                        case 0x01: return ouzel::input::Keyboard::Key::escape;
                        case 0x02: return ouzel::input::Keyboard::Key::num1;
                        case 0x03: return ouzel::input::Keyboard::Key::num2;
                        case 0x04: return ouzel::input::Keyboard::Key::num3;
                        case 0x05: return ouzel::input::Keyboard::Key::num4;
                        case 0x06: return ouzel::input::Keyboard::Key::num5;
                        case 0x07: return ouzel::input::Keyboard::Key::num6;
                        case 0x08: return ouzel::input::Keyboard::Key::num7;
                        case 0x09: return ouzel::input::Keyboard::Key::num8;
                        case 0x0A: return ouzel::input::Keyboard::Key::num9;
                        case 0x0B: return ouzel::input::Keyboard::Key::num0;
                        case 0x0C: return ouzel::input::Keyboard::Key::minus;
                        case 0x0D: return ouzel::input::Keyboard::Key::equal;
                        case 0x0E: return ouzel::input::Keyboard::Key::backspace;
                        case 0x0F: return ouzel::input::Keyboard::Key::tab;
                        case 0x10: return ouzel::input::Keyboard::Key::q;
                        case 0x11: return ouzel::input::Keyboard::Key::w;
                        case 0x12: return ouzel::input::Keyboard::Key::e;
                        case 0x13: return ouzel::input::Keyboard::Key::r;
                        case 0x14: return ouzel::input::Keyboard::Key::t;
                        case 0x15: return ouzel::input::Keyboard::Key::y;
                        case 0x16: return ouzel::input::Keyboard::Key::u;
                        case 0x17: return ouzel::input::Keyboard::Key::i;
                        case 0x18: return ouzel::input::Keyboard::Key::o;
                        case 0x19: return ouzel::input::Keyboard::Key::p;
                        case 0x1A: return ouzel::input::Keyboard::Key::leftBracket;
                        case 0x1B: return ouzel::input::Keyboard::Key::rightBracket;
                        case 0x1C: return isExtended ? ouzel::input::Keyboard::Key::numpadEnter : ouzel::input::Keyboard::Key::enter;
                        case 0x1D: return isExtended ? ouzel::input::Keyboard::Key::rightControl : ouzel::input::Keyboard::Key::leftControl;
                        case 0x1E: return ouzel::input::Keyboard::Key::a;
                        case 0x1F: return ouzel::input::Keyboard::Key::s;
                        case 0x20: return ouzel::input::Keyboard::Key::d;
                        case 0x21: return ouzel::input::Keyboard::Key::f;
                        case 0x22: return ouzel::input::Keyboard::Key::g;
                        case 0x23: return ouzel::input::Keyboard::Key::h;
                        case 0x24: return ouzel::input::Keyboard::Key::j;
                        case 0x25: return ouzel::input::Keyboard::Key::k;
                        case 0x26: return ouzel::input::Keyboard::Key::l;
                        case 0x27: return ouzel::input::Keyboard::Key::semicolon;
                        case 0x28: return ouzel::input::Keyboard::Key::quote;
                        case 0x29: return ouzel::input::Keyboard::Key::grave;
                        case 0x2A: return ouzel::input::Keyboard::Key::leftShift;
                        case 0x2B: return ouzel::input::Keyboard::Key::backslash;
                        case 0x2C: return ouzel::input::Keyboard::Key::z;
                        case 0x2D: return ouzel::input::Keyboard::Key::x;
                        case 0x2E: return ouzel::input::Keyboard::Key::c;
                        case 0x2F: return ouzel::input::Keyboard::Key::v;
                        case 0x30: return ouzel::input::Keyboard::Key::b;
                        case 0x31: return ouzel::input::Keyboard::Key::n;
                        case 0x32: return ouzel::input::Keyboard::Key::m;
                        case 0x33: return ouzel::input::Keyboard::Key::comma;
                        case 0x34: return ouzel::input::Keyboard::Key::period;
                        case 0x35: return isExtended ? ouzel::input::Keyboard::Key::numpadDivide : ouzel::input::Keyboard::Key::slash;
                        case 0x36: return ouzel::input::Keyboard::Key::rightShift;
                        case 0x37: return isExtended ? ouzel::input::Keyboard::Key::printScreen : ouzel::input::Keyboard::Key::numpadMultiply;
                        case 0x38: return isExtended ? ouzel::input::Keyboard::Key::rightAlt : ouzel::input::Keyboard::Key::leftAlt;
                        case 0x39: return ouzel::input::Keyboard::Key::space;
                        case 0x3A: return isExtended ? ouzel::input::Keyboard::Key::numpadPlus : ouzel::input::Keyboard::Key::capsLock;
                        case 0x3B: return ouzel::input::Keyboard::Key::f1;
                        case 0x3C: return ouzel::input::Keyboard::Key::f2;
                        case 0x3D: return ouzel::input::Keyboard::Key::f3;
                        case 0x3E: return ouzel::input::Keyboard::Key::f4;
                        case 0x3F: return ouzel::input::Keyboard::Key::f5;
                        case 0x40: return ouzel::input::Keyboard::Key::f6;
                        case 0x41: return ouzel::input::Keyboard::Key::f7;
                        case 0x42: return ouzel::input::Keyboard::Key::f8;
                        case 0x43: return ouzel::input::Keyboard::Key::f9;
                        case 0x44: return ouzel::input::Keyboard::Key::f10;
                        case 0x45: return ouzel::input::Keyboard::Key::numLock;
                        case 0x46: return ouzel::input::Keyboard::Key::scrollLock;
                        case 0x47: return isExtended ? ouzel::input::Keyboard::Key::home : ouzel::input::Keyboard::Key::numpad7;
                        case 0x48: return isExtended ? ouzel::input::Keyboard::Key::up : ouzel::input::Keyboard::Key::numpad8;
                        case 0x49: return isExtended ? ouzel::input::Keyboard::Key::pageUp : ouzel::input::Keyboard::Key::numpad9;
                        case 0x4A: return ouzel::input::Keyboard::Key::numpadMinus;
                        case 0x4B: return isExtended ? ouzel::input::Keyboard::Key::left : ouzel::input::Keyboard::Key::numpad4;
                        case 0x4C: return ouzel::input::Keyboard::Key::numpad5;
                        case 0x4D: return isExtended ? ouzel::input::Keyboard::Key::right : ouzel::input::Keyboard::Key::numpad6;
                        case 0x4E: return ouzel::input::Keyboard::Key::numpadPlus;
                        case 0x4F: return isExtended ? ouzel::input::Keyboard::Key::end : ouzel::input::Keyboard::Key::numpad1;
                        case 0x50: return isExtended ? ouzel::input::Keyboard::Key::down : ouzel::input::Keyboard::Key::numpad2;
                        case 0x51: return isExtended ? ouzel::input::Keyboard::Key::pageDown : ouzel::input::Keyboard::Key::numpad3;
                        case 0x52: return isExtended ? ouzel::input::Keyboard::Key::insert : ouzel::input::Keyboard::Key::numpad0;
                        case 0x53: return isExtended ? ouzel::input::Keyboard::Key::del : ouzel::input::Keyboard::Key::numpadDecimal;
                        case 0x54: return ouzel::input::Keyboard::Key::none;
                        case 0x55: return ouzel::input::Keyboard::Key::none;
                        case 0x56: return ouzel::input::Keyboard::Key::intlBackslash;
                        case 0x57: return ouzel::input::Keyboard::Key::f11;
                        case 0x58: return ouzel::input::Keyboard::Key::f12;
                        case 0x59: return ouzel::input::Keyboard::Key::pause;
                        case 0x5A: return ouzel::input::Keyboard::Key::none;
                        case 0x5B: return ouzel::input::Keyboard::Key::leftSuper;
                        case 0x5C: return ouzel::input::Keyboard::Key::rightSuper;
                        case 0x5D: return ouzel::input::Keyboard::Key::menu;
                        case 0x5E: return ouzel::input::Keyboard::Key::none;
                        case 0x5F: return ouzel::input::Keyboard::Key::none;
                        case 0x60: return ouzel::input::Keyboard::Key::none;
                        case 0x61: return ouzel::input::Keyboard::Key::none;
                        case 0x62: return ouzel::input::Keyboard::Key::none;
                        case 0x63: return ouzel::input::Keyboard::Key::none;
                        case 0x64: return ouzel::input::Keyboard::Key::f13;
                        case 0x65: return ouzel::input::Keyboard::Key::f14;
                        case 0x66: return ouzel::input::Keyboard::Key::f15;
                        case 0x67: return ouzel::input::Keyboard::Key::f16;
                        case 0x68: return ouzel::input::Keyboard::Key::f17;
                        case 0x69: return ouzel::input::Keyboard::Key::f18;
                        case 0x6A: return ouzel::input::Keyboard::Key::f19;
                        case 0x6B: return ouzel::input::Keyboard::Key::none;
                        case 0x6C: return ouzel::input::Keyboard::Key::none;
                        case 0x6D: return ouzel::input::Keyboard::Key::none;
                        case 0x6E: return ouzel::input::Keyboard::Key::none;
                        case 0x6F: return ouzel::input::Keyboard::Key::none;
                        case 0x70: return ouzel::input::Keyboard::Key::katakanaHiragana;
                        case 0x71: return ouzel::input::Keyboard::Key::none;
                        case 0x72: return ouzel::input::Keyboard::Key::none;
                        case 0x73: return ouzel::input::Keyboard::Key::ro;
                        case 0x74: return ouzel::input::Keyboard::Key::none;
                        case 0x75: return ouzel::input::Keyboard::Key::none;
                        case 0x76: return ouzel::input::Keyboard::Key::none;
                        case 0x77: return ouzel::input::Keyboard::Key::none;
                        case 0x78: return ouzel::input::Keyboard::Key::none;
                        case 0x79: return ouzel::input::Keyboard::Key::henkan;
                        case 0x7A: return ouzel::input::Keyboard::Key::none;
                        case 0x7B: return ouzel::input::Keyboard::Key::muhenkan;
                        case 0x7C: return ouzel::input::Keyboard::Key::none;
                        case 0x7D: return ouzel::input::Keyboard::Key::yen;
                        case 0x7E: return ouzel::input::Keyboard::Key::none;
                        case 0x7F: return ouzel::input::Keyboard::Key::none;
                        default: return ouzel::input::Keyboard::Key::none;
                    }
                }
                else
                    return ouzel::input::Keyboard::Key::none;
            }
        }
    }

    LRESULT CALLBACK windowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
        auto userData = GetWindowLongPtr(window, GWLP_USERDATA);
        if (!userData) return DefWindowProcW(window, message, wParam, lParam);

        const auto windowWin = ouzel::bitCast<ouzel::core::windows::NativeWindow*>(userData);

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
                const auto extraInfo = GetMessageExtraInfo();

                // don't handle mouse event that came from pen or touch
                if ((extraInfo & SIGNATURE_MASK) != MI_WP_SIGNATURE)
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
                const auto extraInfo = GetMessageExtraInfo();

                // don't handle mouse event that came from pen or touch
                if ((extraInfo & SIGNATURE_MASK) != MI_WP_SIGNATURE)
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
                    auto& inputSystemWin = ouzel::engine->getInputManager()->getInputSystem();
                    inputSystemWin.updateCursor();
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
                        windowWin->handleResize(ouzel::Size<std::uint32_t, 2>(static_cast<std::uint32_t>(LOWORD(lParam)),
                                                              static_cast<std::uint32_t>(HIWORD(lParam))));
                        windowWin->handleRestore();
                        break;
                    case SIZE_MAXIMIZED:
                        windowWin->handleResize(ouzel::Size<std::uint32_t, 2>(static_cast<std::uint32_t>(LOWORD(lParam)),
                                                              static_cast<std::uint32_t>(HIWORD(lParam))));
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
                //if (ouzel::engine->getGraphics()->getDevice()->getDriver() != ouzel::graphics::Driver::empty)
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
            case WM_CREATE:
                return 0;
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
            case WM_DEVICECHANGE:
            {
                // TODO: handle input device connect/disconnect
                return TRUE;
            }
            case WM_USER:
            {
                const auto engineWin = static_cast<ouzel::core::windows::Engine*>(ouzel::engine);
                engineWin->executeAll();
                break;
            }
        }

        return DefWindowProcW(window, message, wParam, lParam);
    }
}

namespace ouzel::core::windows
{
    NativeWindow::NativeWindow(const std::function<void(const Event&)>& initCallback,
                               const Size<std::uint32_t, 2>& newSize,
                               bool newResizable,
                               bool newFullscreen,
                               bool newExclusiveFullscreen,
                               const std::string& newTitle,
                               bool newHighDpi):
        core::NativeWindow(initCallback,
                           newSize,
                           newResizable,
                           newFullscreen,
                           newExclusiveFullscreen,
                           newTitle,
                           newHighDpi)
    {
        if (highDpi)
        {
            platform::winapi::Library shcoreModule("shcore.dll");

            if (shcoreModule)
            {
                using SetProcessDpiAwarenessProc = HRESULT(STDAPICALLTYPE*)(int value);
                const auto setProcessDpiAwarenessProc = reinterpret_cast<SetProcessDpiAwarenessProc>(GetProcAddress(shcoreModule.get(), "SetProcessDpiAwareness"));

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
        wc.lpszClassName = L"OuzelWindow";
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

        auto titleBuffer = std::make_unique<WCHAR[]>(titleBufferSize);
        if (MultiByteToWideChar(CP_UTF8, 0, title.c_str(), -1, titleBuffer.get(), titleBufferSize) == 0)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

        window = CreateWindowExW(windowExStyle, MAKEINTATOM(windowClass), titleBuffer.get(), windowStyle,
                                 x, y, width, height, nullptr, nullptr, instance, nullptr);

        if (!window)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to create window");

        monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);

        if (fullscreen)
            switchFullscreen(fullscreen);

        if (!GetClientRect(window, &windowRect))
            throw std::system_error(GetLastError(), std::system_category(), "Failed to get client rectangle");

        size.v[0] = static_cast<std::uint32_t>(windowRect.right - windowRect.left);
        size.v[1] = static_cast<std::uint32_t>(windowRect.bottom - windowRect.top);
        resolution = size;

        if (!RegisterTouchWindow(window, 0))
            logger.log(Log::Level::warning) << "Failed to enable touch for window";

        ShowWindow(window, SW_SHOW);

        SetLastError(ERROR_SUCCESS);

        if (!SetWindowLongPtr(window, GWLP_USERDATA, bitCast<LONG_PTR>(this)))
            if (DWORD error = GetLastError())
                throw std::system_error(error, std::system_category(), "Failed to set window pointer");
    }

    NativeWindow::~NativeWindow()
    {
        if (window)
            DestroyWindow(window);

        if (windowClass)
            UnregisterClassW(MAKEINTATOM(windowClass), GetModuleHandleW(nullptr));
    }

    void NativeWindow::executeCommand(const Command& command)
    {
        switch (command.type)
        {
            case Command::Type::changeSize:
                setSize(command.size);
                break;
            case Command::Type::changeFullscreen:
                setFullscreen(command.fullscreen);
                break;
            case Command::Type::close:
                close();
                break;
            case Command::Type::setTitle:
                setTitle(command.title);
                break;
            case Command::Type::bringToFront:
                bringToFront();
                break;
            case Command::Type::show:
                show();
                break;
            case Command::Type::hide:
                hide();
                break;
            case Command::Type::minimize:
                minimize();
                break;
            case Command::Type::maximize:
                maximize();
                break;
            case Command::Type::restore:
                restore();
                break;
            default:
                throw std::runtime_error("Invalid command");
        }
    }

    void NativeWindow::close()
    {
        SendMessage(window, WM_CLOSE, 0, 0);
    }

    void NativeWindow::setSize(const Size<std::uint32_t, 2>& newSize)
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

        Event resolutionChangeEvent(Event::Type::resolutionChange);
        resolutionChangeEvent.size = resolution;
        sendEvent(resolutionChangeEvent);
    }

    void NativeWindow::setFullscreen(bool newFullscreen)
    {
        fullscreen = newFullscreen;

        switchFullscreen(newFullscreen);
    }

    void NativeWindow::setTitle(const std::string& newTitle)
    {
        if (title != newTitle)
        {
            const int titleBufferSize = MultiByteToWideChar(CP_UTF8, 0, newTitle.c_str(), -1, nullptr, 0);
            if (titleBufferSize == 0)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

            auto titleBuffer = std::make_unique<WCHAR[]>(titleBufferSize);
            if (MultiByteToWideChar(CP_UTF8, 0, newTitle.c_str(), -1, titleBuffer.get(), titleBufferSize) == 0)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

            if (!SetWindowTextW(window, titleBuffer.get()))
                throw std::system_error(GetLastError(), std::system_category(), "Failed to set window title");

            title = newTitle;
        }
    }

    void NativeWindow::bringToFront()
    {
        SetForegroundWindow(window);
    }

    void NativeWindow::show()
    {
        ShowWindow(window, SW_SHOW);
    }

    void NativeWindow::hide()
    {
        ShowWindow(window, SW_HIDE);
    }

    void NativeWindow::minimize()
    {
        ShowWindow(window, SW_MINIMIZE);
    }

    void NativeWindow::maximize()
    {
        ShowWindow(window, SW_MAXIMIZE);
    }

    void NativeWindow::restore()
    {
        ShowWindow(window, SW_RESTORE);
    }

    void NativeWindow::switchFullscreen(bool newFullscreen)
    {
        if (exclusiveFullscreen)
        {
            if (engine->getGraphics()->getDevice()->getDriver() == graphics::Driver::direct3D11)
            {
                const auto renderDevice = static_cast<graphics::d3d11::RenderDevice*>(engine->getGraphics()->getDevice());
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

    void NativeWindow::handleResize(const Size<std::uint32_t, 2>& newSize)
    {
        monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);

        size = newSize;
        resolution = size;

        Event sizeChangeEvent(Event::Type::sizeChange);
        sizeChangeEvent.size = size;
        sendEvent(sizeChangeEvent);

        Event resolutionChangeEvent(Event::Type::resolutionChange);
        resolutionChangeEvent.size = resolution;
        sendEvent(resolutionChangeEvent);
    }

    void NativeWindow::handleMove()
    {
        monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
    }

    void NativeWindow::handleActivate(WPARAM wParam)
    {
        Event focusChangeEvent(Event::Type::focusChange);
        focusChangeEvent.focus = wParam != 0;
        sendEvent(focusChangeEvent);

        if (wParam)
        {
                auto& inputSystemWin = engine->getInputManager()->getInputSystem();
                const auto mouseDevice = inputSystemWin.getMouseDevice();

                POINT cursorPos;
                if (!GetCursorPos(&cursorPos))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to get cursor position");

                const Vector<float, 2> position{
                    static_cast<float>(cursorPos.x),
                    static_cast<float>(cursorPos.y)
                };

                mouseDevice->handleMove(engine->getWindow()->convertWindowToNormalizedLocation(position));
        }
    }

    void NativeWindow::handleShowWindow(BOOL shown)
    {
        sendEvent(Event(shown ? Event::Type::show : Event::Type::hide));
    }

    void NativeWindow::handleMinimize()
    {
        Event focusChangeEvent(Event::Type::focusChange);
        focusChangeEvent.focus = false;
        sendEvent(focusChangeEvent);

        sendEvent(Event(Event::Type::minimize));
    }

    void NativeWindow::handleMaximize()
    {
        sendEvent(Event(Event::Type::maximize));
    }

    void NativeWindow::handleRestore()
    {
        Event focusChangeEvent(Event::Type::focusChange);
        focusChangeEvent.focus = true;
        sendEvent(focusChangeEvent);

        auto& inputSystemWin = engine->getInputManager()->getInputSystem();
        const auto mouseDevice = inputSystemWin.getMouseDevice();

        POINT cursorPos;
        if (!GetCursorPos(&cursorPos))
            throw std::system_error(GetLastError(), std::system_category(), "Failed to get cursor position");

        const Vector<float, 2> position{
            static_cast<float>(cursorPos.x),
            static_cast<float>(cursorPos.y)
        };

        mouseDevice->handleMove(engine->getWindow()->convertWindowToNormalizedLocation(position));

        sendEvent(Event(Event::Type::restore));
    }

    void NativeWindow::handleKey(UINT message, WPARAM wParam, LPARAM lParam)
    {
        auto& inputSystemWin = engine->getInputManager()->getInputSystem();
        const auto keyboardDevice = inputSystemWin.getKeyboardDevice();

        if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN)
            keyboardDevice->handleKeyPress(convertKeyCode(lParam, wParam));
        else if (message == WM_KEYUP || message == WM_SYSKEYUP)
            keyboardDevice->handleKeyRelease(convertKeyCode(lParam, wParam));
    }

    void NativeWindow::handleMouseMove(LPARAM lParam)
    {
        auto& inputSystemWin = engine->getInputManager()->getInputSystem();
        const auto mouseDevice = inputSystemWin.getMouseDevice();

        const Vector<float, 2> position{
            static_cast<float>(GET_X_LPARAM(lParam)),
            static_cast<float>(GET_Y_LPARAM(lParam))
        };

        mouseDevice->handleMove(engine->getWindow()->convertWindowToNormalizedLocation(position));
    }

    void NativeWindow::handleMouseButton(UINT message, WPARAM wParam, LPARAM lParam)
    {
        auto& inputSystemWin = engine->getInputManager()->getInputSystem();
        const auto mouseDevice = inputSystemWin.getMouseDevice();

        const Vector<float, 2> position{
            static_cast<float>(GET_X_LPARAM(lParam)),
            static_cast<float>(GET_Y_LPARAM(lParam))
        };

        input::Mouse::Button button;

        if (message == WM_LBUTTONDOWN || message == WM_LBUTTONUP)
            button = input::Mouse::Button::left;
        else if (message == WM_RBUTTONDOWN || message == WM_RBUTTONUP)
            button = input::Mouse::Button::right;
        else if (message == WM_MBUTTONDOWN || message == WM_MBUTTONUP)
            button = input::Mouse::Button::middle;
        else if (message == WM_XBUTTONDOWN || message == WM_XBUTTONUP)
        {
            if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
                button = input::Mouse::Button::x1;
            else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON2)
                button = input::Mouse::Button::x2;
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

    void NativeWindow::handleMouseWheel(UINT message, WPARAM wParam, LPARAM lParam)
    {
        auto& inputSystemWin = engine->getInputManager()->getInputSystem();
        const auto mouseDevice = inputSystemWin.getMouseDevice();

        const Vector<float, 2> position{
            static_cast<float>(GET_X_LPARAM(lParam)),
            static_cast<float>(GET_Y_LPARAM(lParam))
        };

        if (message == WM_MOUSEWHEEL)
        {
            const auto param = static_cast<short>(HIWORD(wParam));
            mouseDevice->handleScroll(Vector<float, 2>{0.0F, -static_cast<float>(param) / static_cast<float>(WHEEL_DELTA)},
                                      engine->getWindow()->convertWindowToNormalizedLocation(position));
        }
        else if (message == WM_MOUSEHWHEEL)
        {
            const auto param = static_cast<short>(HIWORD(wParam));
            mouseDevice->handleScroll(Vector<float, 2>{static_cast<float>(param) / static_cast<float>(WHEEL_DELTA), 0.0F},
                                      engine->getWindow()->convertWindowToNormalizedLocation(position));
        }
    }

    void NativeWindow::handleTouch(WPARAM wParam, LPARAM lParam)
    {
        auto& inputSystemWin = engine->getInputManager()->getInputSystem();
        const auto touchpadDevice = inputSystemWin.getTouchpadDevice();

        const UINT inputCount = LOWORD(wParam);
        std::vector<TOUCHINPUT> touches(inputCount);

        if (!GetTouchInputInfo(reinterpret_cast<HTOUCHINPUT>(lParam), inputCount, touches.data(), sizeof(TOUCHINPUT)))
            throw std::system_error(GetLastError(), std::system_category(), "Failed to get touch info");

        for (const TOUCHINPUT& touch : touches)
        {
            const Vector<float, 2> position{
                static_cast<float>(touch.x / 100),
                static_cast<float>(touch.y / 100)
            };

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

    void NativeWindow::addAccelerator(HACCEL accelerator)
    {
        engine->executeOnMainThread([this, accelerator]() {
            accelerators.insert(accelerator);
        });
    }

    void NativeWindow::removeAccelerator(HACCEL accelerator)
    {
        engine->executeOnMainThread([this, accelerator]() {
            accelerators.erase(accelerator);
        });
    }
}
