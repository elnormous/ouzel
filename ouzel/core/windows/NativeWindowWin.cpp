// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <system_error>
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
#include "utils/Log.hpp"

static constexpr LONG_PTR SIGNATURE_MASK = 0x0FFFFFF00;
static constexpr LONG_PTR MOUSEEVENTF_FROMTOUCH = 0x0FF515700;

static const std::unordered_map<uint8_t, ouzel::input::Keyboard::Key> scanCodeMap =
{
    {0x01, ouzel::input::Keyboard::Key::ESCAPE},
    {0x02, ouzel::input::Keyboard::Key::NUM_1},
    {0x03, ouzel::input::Keyboard::Key::NUM_2},
    {0x04, ouzel::input::Keyboard::Key::NUM_3},
    {0x05, ouzel::input::Keyboard::Key::NUM_4},
    {0x06, ouzel::input::Keyboard::Key::NUM_5},
    {0x07, ouzel::input::Keyboard::Key::NUM_6},
    {0x08, ouzel::input::Keyboard::Key::NUM_7},
    {0x09, ouzel::input::Keyboard::Key::NUM_8},
    {0x0A, ouzel::input::Keyboard::Key::NUM_9},
    {0x0B, ouzel::input::Keyboard::Key::NUM_0},
    {0x0C, ouzel::input::Keyboard::Key::MINUS},
    {0x0D, ouzel::input::Keyboard::Key::EQUAL},
    {0x0E, ouzel::input::Keyboard::Key::BACKSPACE},
    {0x0F, ouzel::input::Keyboard::Key::TAB},
    {0x10, ouzel::input::Keyboard::Key::Q},
    {0x11, ouzel::input::Keyboard::Key::W},
    {0x12, ouzel::input::Keyboard::Key::E},
    {0x13, ouzel::input::Keyboard::Key::R},
    {0x14, ouzel::input::Keyboard::Key::T},
    {0x15, ouzel::input::Keyboard::Key::Y},
    {0x16, ouzel::input::Keyboard::Key::U},
    {0x17, ouzel::input::Keyboard::Key::I},
    {0x18, ouzel::input::Keyboard::Key::O},
    {0x19, ouzel::input::Keyboard::Key::P},
    {0x1A, ouzel::input::Keyboard::Key::LEFT_BRACKET},
    {0x1B, ouzel::input::Keyboard::Key::RIGHT_BRACKET},
    {0x1C, ouzel::input::Keyboard::Key::ENTER},
    {0x1D, ouzel::input::Keyboard::Key::LEFT_CONTROL},
    {0x1E, ouzel::input::Keyboard::Key::A},
    {0x1F, ouzel::input::Keyboard::Key::S},
    {0x20, ouzel::input::Keyboard::Key::D},
    {0x21, ouzel::input::Keyboard::Key::F},
    {0x22, ouzel::input::Keyboard::Key::G},
    {0x23, ouzel::input::Keyboard::Key::H},
    {0x24, ouzel::input::Keyboard::Key::J},
    {0x25, ouzel::input::Keyboard::Key::K},
    {0x26, ouzel::input::Keyboard::Key::L},
    {0x27, ouzel::input::Keyboard::Key::SEMICOLON},
    {0x28, ouzel::input::Keyboard::Key::QUOTE},
    {0x29, ouzel::input::Keyboard::Key::GRAVE},
    {0x2A, ouzel::input::Keyboard::Key::LEFT_SHIFT},
    {0x2B, ouzel::input::Keyboard::Key::BACKSLASH},
    {0x2C, ouzel::input::Keyboard::Key::Z},
    {0x2D, ouzel::input::Keyboard::Key::X},
    {0x2E, ouzel::input::Keyboard::Key::C},
    {0x2F, ouzel::input::Keyboard::Key::V},
    {0x30, ouzel::input::Keyboard::Key::B},
    {0x31, ouzel::input::Keyboard::Key::N},
    {0x32, ouzel::input::Keyboard::Key::M},
    {0x33, ouzel::input::Keyboard::Key::COMMA},
    {0x34, ouzel::input::Keyboard::Key::PERIOD},
    {0x35, ouzel::input::Keyboard::Key::SLASH},
    {0x36, ouzel::input::Keyboard::Key::RIGHT_SHIFT},
    {0x37, ouzel::input::Keyboard::Key::PRINT_SCREEN},
    {0x38, ouzel::input::Keyboard::Key::LEFT_ALT},
    {0x39, ouzel::input::Keyboard::Key::SPACE},
    {0x3A, ouzel::input::Keyboard::Key::CAPS_LOCK},
    {0x3B, ouzel::input::Keyboard::Key::F1},
    {0x3C, ouzel::input::Keyboard::Key::F2},
    {0x3D, ouzel::input::Keyboard::Key::F3},
    {0x3E, ouzel::input::Keyboard::Key::F4},
    {0x3F, ouzel::input::Keyboard::Key::F5},
    {0x40, ouzel::input::Keyboard::Key::F6},
    {0x41, ouzel::input::Keyboard::Key::F7},
    {0x42, ouzel::input::Keyboard::Key::F8},
    {0x43, ouzel::input::Keyboard::Key::F9},
    {0x44, ouzel::input::Keyboard::Key::F10},
    {0x45, ouzel::input::Keyboard::Key::NUM_LOCK},
    {0x46, ouzel::input::Keyboard::Key::SCROLL_LOCK},
    {0x47, ouzel::input::Keyboard::Key::HOME},
    {0x48, ouzel::input::Keyboard::Key::UP},
    {0x49, ouzel::input::Keyboard::Key::PAGE_UP},
    {0x4A, ouzel::input::Keyboard::Key::NUMPAD_MINUS},
    {0x4B, ouzel::input::Keyboard::Key::LEFT},
    {0x4C, ouzel::input::Keyboard::Key::NUMPAD_5},
    {0x4D, ouzel::input::Keyboard::Key::RIGHT},
    {0x4E, ouzel::input::Keyboard::Key::NUMPAD_PLUS},
    {0x4F, ouzel::input::Keyboard::Key::END},
    {0x50, ouzel::input::Keyboard::Key::DOWN},
    {0x51, ouzel::input::Keyboard::Key::PAGE_DOWN},
    {0x52, ouzel::input::Keyboard::Key::INSERT},
    {0x53, ouzel::input::Keyboard::Key::DEL},
    {0x54, ouzel::input::Keyboard::Key::NONE},
    {0x55, ouzel::input::Keyboard::Key::NONE},
    {0x56, ouzel::input::Keyboard::Key::INTL_BACKSLASH},
    {0x57, ouzel::input::Keyboard::Key::F11},
    {0x58, ouzel::input::Keyboard::Key::F12},
    {0x59, ouzel::input::Keyboard::Key::PAUSE},
    {0x5A, ouzel::input::Keyboard::Key::NONE},
    {0x5B, ouzel::input::Keyboard::Key::LEFT_SUPER},
    {0x5C, ouzel::input::Keyboard::Key::RIGHT_SUPER},
    {0x5D, ouzel::input::Keyboard::Key::MENU},
    {0x5E, ouzel::input::Keyboard::Key::NONE},
    {0x5F, ouzel::input::Keyboard::Key::NONE},
    {0x60, ouzel::input::Keyboard::Key::NONE},
    {0x61, ouzel::input::Keyboard::Key::NONE},
    {0x62, ouzel::input::Keyboard::Key::NONE},
    {0x63, ouzel::input::Keyboard::Key::NONE},
    {0x64, ouzel::input::Keyboard::Key::F13},
    {0x65, ouzel::input::Keyboard::Key::F14},
    {0x66, ouzel::input::Keyboard::Key::F15},
    {0x67, ouzel::input::Keyboard::Key::F16},
    {0x68, ouzel::input::Keyboard::Key::F17},
    {0x69, ouzel::input::Keyboard::Key::F18},
    {0x6A, ouzel::input::Keyboard::Key::F19},
    {0x6B, ouzel::input::Keyboard::Key::NONE},
    {0x6C, ouzel::input::Keyboard::Key::NONE},
    {0x6D, ouzel::input::Keyboard::Key::NONE},
    {0x6E, ouzel::input::Keyboard::Key::NONE},
    {0x6F, ouzel::input::Keyboard::Key::NONE},
    {0x70, ouzel::input::Keyboard::Key::KATAKANA_HIRAGANA},
    {0x71, ouzel::input::Keyboard::Key::NONE},
    {0x72, ouzel::input::Keyboard::Key::NONE},
    {0x73, ouzel::input::Keyboard::Key::RO},
    {0x74, ouzel::input::Keyboard::Key::NONE},
    {0x75, ouzel::input::Keyboard::Key::NONE},
    {0x76, ouzel::input::Keyboard::Key::NONE},
    {0x77, ouzel::input::Keyboard::Key::NONE},
    {0x78, ouzel::input::Keyboard::Key::NONE},
    {0x79, ouzel::input::Keyboard::Key::HENKAN},
    {0x7A, ouzel::input::Keyboard::Key::NONE},
    {0x7B, ouzel::input::Keyboard::Key::MUHENKAN},
    {0x7C, ouzel::input::Keyboard::Key::NONE},
    {0x7D, ouzel::input::Keyboard::Key::YEN},
    {0x7E, ouzel::input::Keyboard::Key::NONE},
    {0x7F, ouzel::input::Keyboard::Key::NONE}
};

static const std::unordered_map<WPARAM, ouzel::input::Keyboard::Key> virtualKeyMap = {
    {VK_CLEAR, ouzel::input::Keyboard::Key::CLEAR},
    {VK_MODECHANGE, ouzel::input::Keyboard::Key::MODE_CHANGE},
    {VK_SELECT, ouzel::input::Keyboard::Key::SELECT},
    {VK_EXECUTE, ouzel::input::Keyboard::Key::EXECUTE},
    {VK_HELP, ouzel::input::Keyboard::Key::HELP},
    {VK_PAUSE, ouzel::input::Keyboard::Key::PAUSE},
    {VK_NUMLOCK, ouzel::input::Keyboard::Key::NUM_LOCK},

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

    {VK_OEM_NEC_EQUAL, ouzel::input::Keyboard::Key::NUMPAD_EQUAL},
    {VK_BROWSER_BACK, ouzel::input::Keyboard::Key::BACK},
    {VK_BROWSER_FORWARD, ouzel::input::Keyboard::Key::FORWARD},
    {VK_BROWSER_REFRESH, ouzel::input::Keyboard::Key::REFRESH},
    {VK_BROWSER_STOP, ouzel::input::Keyboard::Key::STOP},
    {VK_BROWSER_SEARCH, ouzel::input::Keyboard::Key::SEARCH},
    {VK_BROWSER_FAVORITES, ouzel::input::Keyboard::Key::BOOKMARKS},
    {VK_BROWSER_HOME, ouzel::input::Keyboard::Key::HOME},
    {VK_VOLUME_MUTE, ouzel::input::Keyboard::Key::MUTE},
    {VK_VOLUME_DOWN, ouzel::input::Keyboard::Key::VOLUME_DOWN},
    {VK_VOLUME_UP, ouzel::input::Keyboard::Key::VOLUME_UP},

    {VK_MEDIA_NEXT_TRACK, ouzel::input::Keyboard::Key::AUDIO_NEXT},
    {VK_MEDIA_PREV_TRACK, ouzel::input::Keyboard::Key::AUDIO_PREVIOUS},
    {VK_MEDIA_STOP, ouzel::input::Keyboard::Key::AUDIO_STOP},
    {VK_MEDIA_PLAY_PAUSE, ouzel::input::Keyboard::Key::AUDIO_PLAY},
    {VK_LAUNCH_MAIL, ouzel::input::Keyboard::Key::MAIL},
    {VK_LAUNCH_MEDIA_SELECT, ouzel::input::Keyboard::Key::MEDIA_SELECT},

    {VK_OEM_102, ouzel::input::Keyboard::Key::INTL_BACKSLASH},

    {VK_ATTN, ouzel::input::Keyboard::Key::PRINT_SCREEN},
    {VK_CRSEL, ouzel::input::Keyboard::Key::CRSEL},
    {VK_EXSEL, ouzel::input::Keyboard::Key::EXSEL},
    {VK_OEM_CLEAR, ouzel::input::Keyboard::Key::CLEAR},

    {VK_LAUNCH_APP1, ouzel::input::Keyboard::Key::APP1},
    {VK_LAUNCH_APP2, ouzel::input::Keyboard::Key::APP2}
};

static ouzel::input::Keyboard::Key convertKeyCode(LPARAM lParam, WPARAM wParam)
{
    auto virtualKeyIterator = virtualKeyMap.find(wParam);
    if (virtualKeyIterator != virtualKeyMap.end())
        return virtualKeyIterator->second;
    else
    {
        ouzel::input::Keyboard::Key key = ouzel::input::Keyboard::Key::NONE;

        uint8_t scanCode = (lParam >> 16) & 0xFF;
        if (scanCode <= 127)
        {
            auto i = scanCodeMap.find(scanCode);
            if (i != scanCodeMap.end())
            {
                key = i->second;

                bool isExtended = (lParam & (1 << 24)) != 0;
                if (isExtended)
                {
                    switch (key)
                    {
                        case ouzel::input::Keyboard::Key::ENTER:
                            key = ouzel::input::Keyboard::Key::NUMPAD_ENTER;
                            break;
                        case ouzel::input::Keyboard::Key::LEFT_ALT:
                            key = ouzel::input::Keyboard::Key::RIGHT_ALT;
                            break;
                        case ouzel::input::Keyboard::Key::LEFT_CONTROL:
                            key = ouzel::input::Keyboard::Key::RIGHT_CONTROL;
                            break;
                        case ouzel::input::Keyboard::Key::SLASH:
                            key = ouzel::input::Keyboard::Key::NUMPAD_DIVIDE;
                            break;
                        case ouzel::input::Keyboard::Key::CAPS_LOCK:
                            key = ouzel::input::Keyboard::Key::NUMPAD_PLUS;
                            break;
                        default:
                            break;
                    }
                }
                else
                {
                    switch (key)
                    {
                        case ouzel::input::Keyboard::Key::HOME:
                            key = ouzel::input::Keyboard::Key::NUMPAD_7;
                            break;
                        case ouzel::input::Keyboard::Key::UP:
                            key = ouzel::input::Keyboard::Key::NUMPAD_8;
                            break;
                        case ouzel::input::Keyboard::Key::PAGE_UP:
                            key = ouzel::input::Keyboard::Key::NUMPAD_9;
                            break;
                        case ouzel::input::Keyboard::Key::LEFT:
                            key = ouzel::input::Keyboard::Key::NUMPAD_4;
                            break;
                        case ouzel::input::Keyboard::Key::RIGHT:
                            key = ouzel::input::Keyboard::Key::NUMPAD_6;
                            break;
                        case ouzel::input::Keyboard::Key::END:
                            key = ouzel::input::Keyboard::Key::NUMPAD_1;
                            break;
                        case ouzel::input::Keyboard::Key::DOWN:
                            key = ouzel::input::Keyboard::Key::NUMPAD_2;
                            break;
                        case ouzel::input::Keyboard::Key::PAGE_DOWN:
                            key = ouzel::input::Keyboard::Key::NUMPAD_3;
                            break;
                        case ouzel::input::Keyboard::Key::INSERT:
                            key = ouzel::input::Keyboard::Key::NUMPAD_0;
                            break;
                        case ouzel::input::Keyboard::Key::DEL:
                            key = ouzel::input::Keyboard::Key::NUMPAD_DECIMAL;
                            break;
                        case ouzel::input::Keyboard::Key::PRINT_SCREEN:
                            key = ouzel::input::Keyboard::Key::NUMPAD_MULTIPLY;
                            break;
                        default:
                            break;
                    }
                }
            }
        }

        return key;
    }
}

static LRESULT CALLBACK windowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    ouzel::NativeWindowWin* windowWin = reinterpret_cast<ouzel::NativeWindowWin*>(GetWindowLongPtr(window, GWLP_USERDATA));
    if (!windowWin) return DefWindowProcW(window, message, wParam, lParam);

    switch (message)
    {
        case WM_ACTIVATEAPP:
            windowWin->handleActivateEvent(wParam);
            break;
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
            windowWin->handleKeyEvent(message, wParam, lParam);
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
                windowWin->handleMouseMoveEvent(lParam);
                return 0;
            }
            break;
        }
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
            windowWin->handleMouseWheelEvent(message, wParam, lParam);
            return 0;
        case WM_TOUCH:
            windowWin->handleTouchEvent(wParam, lParam);
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
        case WM_SIZE:
        {
            switch (wParam)
            {
                case SIZE_MINIMIZED:
                    windowWin->handleMinimizeEvent();
                    break;
                case SIZE_RESTORED:
                    windowWin->handleRestoreEvent();
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
            windowWin->handleMove();
            break;
        case WM_ERASEBKGND:
        {
            // Erase background only for the Empty renderer
            //if (ouzel::engine->getRenderer()->getDevice()->getDriver() != ouzel::graphics::Driver::EMPTY)
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

static constexpr LPCWSTR WINDOW_CLASS_NAME = L"OuzelWindow";

namespace ouzel
{
    NativeWindowWin::NativeWindowWin(const std::function<void(const Event&)>& initCallback,
                                     const Size2& newSize,
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

        int x = CW_USEDEFAULT;
        int y = CW_USEDEFAULT;

        windowStyle = windowWindowedStyle;
        windowExStyle = WS_EX_APPWINDOW;

        RECT windowRect = {0, 0, static_cast<LONG>(size.width), static_cast<LONG>(size.height)};
        if (!AdjustWindowRectEx(&windowRect, windowStyle, FALSE, windowExStyle))
            throw std::system_error(GetLastError(), std::system_category(), "Failed to adjust window rectangle");

        int width = CW_USEDEFAULT;
        int height = CW_USEDEFAULT;
        if (size.width > 0.0F) width = windowRect.right - windowRect.left;
        if (size.height > 0.0F) height = windowRect.bottom - windowRect.top;

        int titleBufferSize = MultiByteToWideChar(CP_UTF8, 0, title.c_str(), -1, nullptr, 0);
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

        size.width = static_cast<float>(windowRect.right - windowRect.left);
        size.height = static_cast<float>(windowRect.bottom - windowRect.top);
        resolution = size;

        if (!RegisterTouchWindow(window, 0))
            engine->log(Log::Level::WARN) << "Failed to enable touch for window";

        ShowWindow(window, SW_SHOW);

        if (!SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this)))
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
            case Command::Type::CHANGE_SIZE:
                setSize(command.size);
                break;
            case Command::Type::CHANGE_FULLSCREEN:
                setFullscreen(command.fullscreen);
                break;
            case Command::Type::CLOSE:
                close();
                break;
            case Command::Type::SET_TITLE:
                setTitle(command.title);
                break;
            default:
                throw std::runtime_error("Invalid command");
        }
    }

    void NativeWindowWin::close()
    {
        SendMessage(window, WM_CLOSE, 0, 0);
    }

    void NativeWindowWin::setSize(const Size2& newSize)
    {
        size = newSize;

        UINT width = static_cast<UINT>(newSize.width);
        UINT height = static_cast<UINT>(newSize.height);

        UINT swpFlags = SWP_NOMOVE | SWP_NOZORDER;

        RECT rect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
        if (!AdjustWindowRectEx(&rect, windowStyle, GetMenu(window) ? TRUE : FALSE, windowExStyle))
            throw std::system_error(GetLastError(), std::system_category(), "Failed to adjust window rectangle");

        if (!SetWindowPos(window, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, swpFlags))
            throw std::system_error(GetLastError(), std::system_category(), "Failed to set window position");

        resolution = size;

        Event resolutionChangeEvent(Event::Type::RESOLUTION_CHANGE);
        resolutionChangeEvent.resolution = resolution;
        sendEvent(resolutionChangeEvent);
    }

    void NativeWindowWin::setTitle(const std::string& newTitle)
    {
        if (title != newTitle)
        {
            int titleBufferSize = MultiByteToWideChar(CP_UTF8, 0, newTitle.c_str(), -1, nullptr, 0);
            if (titleBufferSize == 0)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

            std::vector<WCHAR> titleBuffer(titleBufferSize);
            if (MultiByteToWideChar(CP_UTF8, 0, newTitle.c_str(), -1, titleBuffer.data(), titleBufferSize) == 0)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to convert UTF-8 to wide char");

            if (!SetWindowTextW(window, titleBuffer.data()))
                throw std::system_error(GetLastError(), std::system_category(), "Failed to set window title");
        }

        title = newTitle;
    }

    void NativeWindowWin::setFullscreen(bool newFullscreen)
    {
        fullscreen = newFullscreen;

        switchFullscreen(newFullscreen);
    }

    void NativeWindowWin::switchFullscreen(bool newFullscreen)
    {
        if (exclusiveFullscreen)
        {
            if (engine->getRenderer()->getDevice()->getDriver() == graphics::Driver::DIRECT3D11)
            {
                graphics::RenderDeviceD3D11* renderDeviceD3D11 = static_cast<graphics::RenderDeviceD3D11*>(engine->getRenderer()->getDevice());
                renderDeviceD3D11->setFullscreen(newFullscreen);
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
                GetMonitorInfo(monitor, &info);

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

    void NativeWindowWin::handleResize(const Size2& newSize)
    {
        monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);

        size = newSize;
        resolution = size;

        Event sizeChangeEvent(Event::Type::SIZE_CHANGE);
        sizeChangeEvent.size = size;
        sendEvent(sizeChangeEvent);

        Event resolutionChangeEvent(Event::Type::RESOLUTION_CHANGE);
        resolutionChangeEvent.resolution = resolution;
        sendEvent(resolutionChangeEvent);
    }

    void NativeWindowWin::handleMove()
    {
        monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
    }

    void NativeWindowWin::handleActivateEvent(WPARAM wParam)
    {
        Event focusChangeEvent(Event::Type::FOCUS_CHANGE);
        focusChangeEvent.focus = wParam != 0;
        sendEvent(focusChangeEvent);

        if (wParam)
        {
                input::InputSystemWin* inputSystemWin = static_cast<input::InputSystemWin*>(engine->getInputManager()->getInputSystem());
                input::MouseDeviceWin* mouseDevice = inputSystemWin->getMouseDevice();

                POINT cursorPos;
                if (!GetCursorPos(&cursorPos))
                    throw std::system_error(GetLastError(), std::system_category(), "Failed to get cursor position");

                Vector2 position(static_cast<float>(cursorPos.x),
                                 static_cast<float>(cursorPos.y));
                mouseDevice->handleMove(engine->getWindow()->convertWindowToNormalizedLocation(position));
        }
    }

    void NativeWindowWin::handleMinimizeEvent()
    {
        Event focusChangeEvent(Event::Type::FOCUS_CHANGE);
        focusChangeEvent.focus = false;
        sendEvent(focusChangeEvent);
    }

    void NativeWindowWin::handleRestoreEvent()
    {
        Event focusChangeEvent(Event::Type::FOCUS_CHANGE);
        focusChangeEvent.focus = true;
        sendEvent(focusChangeEvent);

        input::InputSystemWin* inputSystemWin = static_cast<input::InputSystemWin*>(engine->getInputManager()->getInputSystem());
        input::MouseDeviceWin* mouseDevice = inputSystemWin->getMouseDevice();

        POINT cursorPos;
        if (!GetCursorPos(&cursorPos))
            throw std::system_error(GetLastError(), std::system_category(), "Failed to get cursor position");

        Vector2 position(static_cast<float>(cursorPos.x),
                         static_cast<float>(cursorPos.y));
        mouseDevice->handleMove(engine->getWindow()->convertWindowToNormalizedLocation(position));
    }

    void NativeWindowWin::handleKeyEvent(UINT message, WPARAM wParam, LPARAM lParam)
    {
        input::InputSystemWin* inputSystemWin = static_cast<input::InputSystemWin*>(engine->getInputManager()->getInputSystem());
        input::KeyboardDeviceWin* keyboardDevice = inputSystemWin->getKeyboardDevice();

        if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN)
            keyboardDevice->handleKeyPress(convertKeyCode(lParam, wParam));
        else if (message == WM_KEYUP || message == WM_SYSKEYUP)
            keyboardDevice->handleKeyRelease(convertKeyCode(lParam, wParam));
    }

    void NativeWindowWin::handleMouseMoveEvent(LPARAM lParam)
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

        if (!GetTouchInputInfo(reinterpret_cast<HTOUCHINPUT>(lParam), inputCount, touches.data(), sizeof(TOUCHINPUT)))
            throw std::system_error(GetLastError(), std::system_category(), "Failed to get touch info");

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
