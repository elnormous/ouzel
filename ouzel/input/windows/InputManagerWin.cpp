// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <unordered_map>
#include <Windows.h>
#include <WbemIdl.h>
#include <OleAuto.h>
#include "InputManagerWin.hpp"
#include "NativeCursorWin.hpp"
#include "GamepadDI.hpp"
#include "GamepadXI.hpp"
#include "core/Engine.hpp"
#include "core/windows/NativeWindowWin.hpp"
#include "events/EventDispatcher.hpp"
#include "thread/Lock.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

static BOOL CALLBACK enumDevicesCallback(const DIDEVICEINSTANCEW* didInstance, VOID* context)
{
    ouzel::input::InputManagerWin* inputWin = reinterpret_cast<ouzel::input::InputManagerWin*>(context);
    inputWin->handleDeviceConnect(didInstance);

    return DIENUM_CONTINUE;
}

namespace ouzel
{
    namespace input
    {
        static const std::unordered_map<UINT, KeyboardKey> keyMap = {
            {VK_CANCEL, KeyboardKey::CANCEL},
            {VK_BACK, KeyboardKey::BACKSPACE},
            {VK_TAB, KeyboardKey::TAB},
            {VK_CLEAR, KeyboardKey::CLEAR},
            {VK_RETURN, KeyboardKey::RETURN},

            {VK_MENU, KeyboardKey::LEFT_ALT},
            {VK_PAUSE, KeyboardKey::PAUSE},
            {VK_CAPITAL, KeyboardKey::CAPITAL},

            {VK_ESCAPE, KeyboardKey::ESCAPE},
            {VK_SPACE, KeyboardKey::SPACE},
            {VK_PRIOR, KeyboardKey::PRIOR},
            {VK_NEXT, KeyboardKey::NEXT},
            {VK_END, KeyboardKey::END},
            {VK_HOME, KeyboardKey::HOME},
            {VK_LEFT, KeyboardKey::LEFT},
            {VK_UP, KeyboardKey::UP},
            {VK_RIGHT, KeyboardKey::RIGHT},
            {VK_DOWN, KeyboardKey::DOWN},

            {VK_SELECT, KeyboardKey::SELECT},
            {VK_PRINT, KeyboardKey::PRINT},
            {VK_EXECUTE, KeyboardKey::EXECUTE},

            {VK_SNAPSHOT, KeyboardKey::SNAPSHOT},
            {VK_INSERT, KeyboardKey::INSERT},
            {VK_DELETE, KeyboardKey::DEL},
            {VK_HELP, KeyboardKey::HELP},

            {'0', KeyboardKey::NUM_0},
            {'1', KeyboardKey::NUM_1},
            {'2', KeyboardKey::NUM_2},
            {'3', KeyboardKey::NUM_3},
            {'4', KeyboardKey::NUM_4},
            {'5', KeyboardKey::NUM_5},
            {'6', KeyboardKey::NUM_6},
            {'7', KeyboardKey::NUM_7},
            {'8', KeyboardKey::NUM_8},
            {'9', KeyboardKey::NUM_9},

            {'A', KeyboardKey::A},
            {'B', KeyboardKey::B},
            {'C', KeyboardKey::C},
            {'D', KeyboardKey::D},
            {'E', KeyboardKey::E},
            {'F', KeyboardKey::F},
            {'G', KeyboardKey::G},
            {'H', KeyboardKey::H},
            {'I', KeyboardKey::I},
            {'J', KeyboardKey::J},
            {'K', KeyboardKey::K},
            {'L', KeyboardKey::L},
            {'M', KeyboardKey::M},
            {'N', KeyboardKey::N},
            {'O', KeyboardKey::O},
            {'P', KeyboardKey::P},
            {'Q', KeyboardKey::Q},
            {'R', KeyboardKey::R},
            {'S', KeyboardKey::S},
            {'T', KeyboardKey::T},
            {'U', KeyboardKey::U},
            {'V', KeyboardKey::V},
            {'W', KeyboardKey::W},
            {'X', KeyboardKey::X},
            {'Y', KeyboardKey::Y},
            {'Z', KeyboardKey::Z},

            {VK_LWIN, KeyboardKey::LEFT_SUPER},
            {VK_RWIN, KeyboardKey::RIGHT_SUPER},
            {VK_APPS, KeyboardKey::MENU},
            {VK_SLEEP, KeyboardKey::SLEEP},

            {VK_NUMPAD0, KeyboardKey::NUMPAD_0},
            {VK_NUMPAD1, KeyboardKey::NUMPAD_1},
            {VK_NUMPAD2, KeyboardKey::NUMPAD_2},
            {VK_NUMPAD3, KeyboardKey::NUMPAD_3},
            {VK_NUMPAD4, KeyboardKey::NUMPAD_4},
            {VK_NUMPAD5, KeyboardKey::NUMPAD_5},
            {VK_NUMPAD6, KeyboardKey::NUMPAD_6},
            {VK_NUMPAD7, KeyboardKey::NUMPAD_7},
            {VK_NUMPAD8, KeyboardKey::NUMPAD_8},
            {VK_NUMPAD9, KeyboardKey::NUMPAD_9},

            {VK_MULTIPLY, KeyboardKey::MULTIPLY},
            {VK_ADD, KeyboardKey::ADD},
            {VK_SEPARATOR, KeyboardKey::SEPARATOR},
            {VK_SUBTRACT, KeyboardKey::SUBTRACT},
            {VK_DECIMAL, KeyboardKey::DECIMAL},
            {VK_DIVIDE, KeyboardKey::DIVIDE},

            {VK_F1, KeyboardKey::F1},
            {VK_F2, KeyboardKey::F2},
            {VK_F3, KeyboardKey::F3},
            {VK_F4, KeyboardKey::F4},
            {VK_F5, KeyboardKey::F5},
            {VK_F6, KeyboardKey::F6},
            {VK_F7, KeyboardKey::F7},
            {VK_F8, KeyboardKey::F8},
            {VK_F9, KeyboardKey::F9},
            {VK_F10, KeyboardKey::F10},
            {VK_F11, KeyboardKey::F11},
            {VK_F12, KeyboardKey::F12},
            {VK_F13, KeyboardKey::F13},
            {VK_F14, KeyboardKey::F14},
            {VK_F15, KeyboardKey::F15},
            {VK_F16, KeyboardKey::F16},
            {VK_F17, KeyboardKey::F17},
            {VK_F18, KeyboardKey::F18},
            {VK_F19, KeyboardKey::F19},
            {VK_F20, KeyboardKey::F20},
            {VK_F21, KeyboardKey::F21},
            {VK_F22, KeyboardKey::F22},
            {VK_F23, KeyboardKey::F23},
            {VK_F24, KeyboardKey::F24},

            {VK_NUMLOCK, KeyboardKey::NUMLOCK},
            {VK_SCROLL, KeyboardKey::SCROLL},
            {VK_LSHIFT, KeyboardKey::LEFT_SHIFT},
            {VK_RSHIFT, KeyboardKey::RIGHT_SHIFT},
            {VK_LCONTROL, KeyboardKey::LEFT_CONTROL},
            {VK_RCONTROL, KeyboardKey::RIGHT_CONTROL},
            {VK_LMENU, KeyboardKey::LEFT_ALT},
            {VK_RMENU, KeyboardKey::RIGHT_ALT},

            {VK_OEM_1, KeyboardKey::SEMICOLON},
            {VK_OEM_PLUS, KeyboardKey::PLUS},
            {VK_OEM_COMMA, KeyboardKey::COMMA},
            {VK_OEM_MINUS, KeyboardKey::MINUS},
            {VK_OEM_PERIOD, KeyboardKey::PERIOD},
            {VK_OEM_2, KeyboardKey::SLASH},
            {VK_OEM_3, KeyboardKey::GRAVE},
            {VK_OEM_4, KeyboardKey::LEFT_BRACKET},
            {VK_OEM_5, KeyboardKey::BACKSLASH},
            {VK_OEM_6, KeyboardKey::RIGHT_BRACKET},
            {VK_OEM_7, KeyboardKey::QUOTE},
            {VK_OEM_8, KeyboardKey::GRAVE},
            {VK_OEM_AX, KeyboardKey::OEM_AX},
            {VK_OEM_102, KeyboardKey::LESS}
        };

        KeyboardKey InputManagerWin::convertKeyCode(UINT keyCode)
        {
            auto i = keyMap.find(keyCode);

            if (i != keyMap.end())
                return i->second;
            else
                return KeyboardKey::NONE;
        }

        uint32_t InputManagerWin::getModifiers(WPARAM wParam)
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

        InputManagerWin::InputManagerWin()
        {
            std::fill(std::begin(gamepadsXI), std::end(gamepadsXI), nullptr);
            currentCursor = defaultCursor = LoadCursor(nullptr, IDC_ARROW);

            HINSTANCE instance = GetModuleHandleW(nullptr);
            HRESULT hr = DirectInput8Create(instance, DIRECTINPUT_VERSION, IID_IDirectInput8W, reinterpret_cast<LPVOID*>(&directInput), nullptr);
            if (FAILED(hr))
                throw SystemError("Failed to initialize DirectInput, error: " + std::to_string(hr));

            for (DWORD userIndex = 0; userIndex < XUSER_MAX_COUNT; ++userIndex)
            {
                XINPUT_STATE state;
                ZeroMemory(&state, sizeof(XINPUT_STATE));

                DWORD result = XInputGetState(userIndex, &state);

                if (result == ERROR_SUCCESS)
                {
                    std::unique_ptr<GamepadXI> gamepad(new GamepadXI(userIndex));
                    gamepadsXI[userIndex] = gamepad.get();

                    Event event;
                    event.type = Event::Type::GAMEPAD_CONNECT;
                    event.gamepadEvent.gamepad = gamepad.get();

                    gamepads.push_back(std::move(gamepad));

                    engine->getEventDispatcher()->postEvent(event);
                }
                else if (result != ERROR_DEVICE_NOT_CONNECTED)
                    Log(Log::Level::WARN) << "Failed to get state for gamepad " << userIndex;
            }

            hr = directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, enumDevicesCallback, this, DIEDFL_ATTACHEDONLY);
            if (FAILED(hr))
                throw SystemError("Failed to enumerate devices, error: " + std::to_string(hr));
        }

        InputManagerWin::~InputManagerWin()
        {
            resourceDeleteSet.clear();
            resources.clear();

            if (directInput) directInput->Release();
        }

        void InputManagerWin::update()
        {
            if (keyboardKeyStates[static_cast<uint32_t>(KeyboardKey::LEFT_SHIFT)] &&
                (GetKeyState(VK_LSHIFT) & 0x8000) == 0)
                keyRelease(KeyboardKey::LEFT_SHIFT, 0);

            if (keyboardKeyStates[static_cast<uint32_t>(KeyboardKey::RIGHT_SHIFT)] &&
                (GetKeyState(VK_RSHIFT) & 0x8000) == 0)
                keyRelease(KeyboardKey::RIGHT_SHIFT, 0);

            for (DWORD userIndex = 0; userIndex < XUSER_MAX_COUNT; ++userIndex)
            {
                GamepadXI* gamepadXI = gamepadsXI[userIndex];

                if (gamepadXI)
                {
                    try
                    {
                        gamepadXI->update();
                    }
                    catch (...)
                    {
                        Event event;
                        event.type = Event::Type::GAMEPAD_DISCONNECT;
                        event.gamepadEvent.gamepad = gamepadXI;

                        engine->getEventDispatcher()->postEvent(event);

                        gamepadsXI[userIndex] = nullptr;

                        auto gamepadIterator = std::find_if(gamepads.begin(), gamepads.end(), [gamepadXI](const std::unique_ptr<Gamepad>& gamepad) {
                            return gamepadXI == gamepad.get();
                        });

                        if (gamepadIterator != gamepads.end())
                            gamepads.erase(gamepadIterator);
                    }
                }
            }

            for (auto i = gamepadsDI.begin(); i != gamepadsDI.end();)
            {
                GamepadDI* gamepadDI = *i;
                try
                {
                    gamepadDI->update();
                    ++i;
                }
                catch (...)
                {
                    Event event;
                    event.type = Event::Type::GAMEPAD_DISCONNECT;
                    event.gamepadEvent.gamepad = gamepadDI;

                    engine->getEventDispatcher()->postEvent(event);

                    i = gamepadsDI.erase(i);

                    auto gamepadIterator = std::find_if(gamepads.begin(), gamepads.end(), [gamepadDI](const std::unique_ptr<Gamepad>& gamepad) {
                        return gamepadDI == gamepad.get();
                    });

                    if (gamepadIterator != gamepads.end())
                        gamepads.erase(gamepadIterator);
                }
            }
        }

        void InputManagerWin::activateNativeCursor(NativeCursor* resource)
        {
            InputManager::activateNativeCursor(resource);

            NativeCursorWin* cursorWin = static_cast<NativeCursorWin*>(resource);

            if (cursorWin)
                currentCursor = cursorWin->getNativeCursor();
            else
                currentCursor = defaultCursor;

            updateCursor();
        }

        NativeCursor* InputManagerWin::createNativeCursor()
        {
            Lock lock(resourceMutex);

            std::unique_ptr<NativeCursorWin> cursorResource(new NativeCursorWin(*this));
            NativeCursor* result = cursorResource.get();

            resources.push_back(std::move(cursorResource));

            return result;
        }

        void InputManagerWin::setCursorVisible(bool visible)
        {
            cursorVisible = visible;

            engine->executeOnMainThread([this, visible] {
                if (visible)
                    SetCursor(currentCursor);
                else
                    SetCursor(nullptr);
            });
        }

        bool InputManagerWin::isCursorVisible() const
        {
            return cursorVisible;
        }

        void InputManagerWin::setCursorLocked(bool locked)
        {
            engine->executeOnMainThread([locked] {
                if (locked)
                {
                    HWND nativeWindow = static_cast<NativeWindowWin*>(engine->getWindow()->getNativeWindow())->getNativeWindow();

                    RECT rect;
                    GetWindowRect(nativeWindow, &rect);

                    LONG centerX = (rect.left + rect.right) / 2;
                    LONG centerY = (rect.top + rect.bottom) / 2;

                    rect.left = centerX;
                    rect.right = centerX + 1;
                    rect.top = centerY;
                    rect.bottom = centerY + 1;

                    if (!ClipCursor(&rect))
                        throw SystemError("Failed to grab pointer");
                }
                else
                    ClipCursor(nullptr);
            });
            cursorLocked = locked;
        }

        bool InputManagerWin::isCursorLocked() const
        {
            return cursorLocked;
        }

        void InputManagerWin::setCursorPosition(const Vector2& position)
        {
            InputManager::setCursorPosition(position);

            engine->executeOnMainThread([position] {
                Vector2 windowLocation = engine->getWindow()->convertNormalizedToWindowLocation(position);
                HWND nativeWindow = static_cast<NativeWindowWin*>(engine->getWindow()->getNativeWindow())->getNativeWindow();

                POINT p;
                p.x = static_cast<LONG>(windowLocation.x);
                p.y = static_cast<LONG>(windowLocation.y);
                ClientToScreen(nativeWindow, &p);
                SetCursorPos(static_cast<int>(p.x),
                             static_cast<int>(p.y));
            });
        }

        void InputManagerWin::startDeviceDiscovery()
        {
            engine->executeOnMainThread([this] {
                for (DWORD userIndex = 0; userIndex < XUSER_MAX_COUNT; ++userIndex)
                {
                    if (!gamepadsXI[userIndex])
                    {
                        XINPUT_STATE state;
                        ZeroMemory(&state, sizeof(XINPUT_STATE));

                        DWORD result = XInputGetState(userIndex, &state);

                        if (result == ERROR_SUCCESS)
                        {
                            std::unique_ptr<GamepadXI> gamepad(new GamepadXI(userIndex));
                            gamepadsXI[userIndex] = gamepad.get();

                            Event event;
                            event.type = Event::Type::GAMEPAD_CONNECT;
                            event.gamepadEvent.gamepad = gamepad.get();

                            gamepads.push_back(std::move(gamepad));

                            engine->getEventDispatcher()->postEvent(event);
                        }
                        else if (result != ERROR_DEVICE_NOT_CONNECTED)
                            Log(Log::Level::WARN) << "Failed to get state for gamepad " << userIndex;
                    }
                }

                HRESULT hr = directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, enumDevicesCallback, this, DIEDFL_ATTACHEDONLY);
                if (FAILED(hr))
                    throw SystemError("Failed to enumerate devices, error: " + std::to_string(hr));
            });
        }

        void InputManagerWin::handleDeviceConnect(const DIDEVICEINSTANCEW* didInstance)
        {
            bool isXInputDevice = false;

            IWbemLocator* wbemLocator = nullptr;
            HRESULT hr = CoCreateInstance(__uuidof(WbemLocator), nullptr, CLSCTX_INPROC_SERVER,
                                          __uuidof(IWbemLocator), reinterpret_cast<LPVOID*>(&wbemLocator));

            if (FAILED(hr))
                throw SystemError("Failed to create WMI locator instance, error: " + std::to_string(hr));
            else
            {
                BSTR namespaceStr = SysAllocString(L"\\\\.\\root\\cimv2");
                BSTR className = SysAllocString(L"Win32_PNPEntity");
                BSTR deviceID = SysAllocString(L"DeviceID");

                if (className && namespaceStr && deviceID)
                {
                    IWbemServices* wbemServices = nullptr;
                    hr = wbemLocator->ConnectServer(namespaceStr, nullptr, nullptr, 0L,
                                                    0L, nullptr, nullptr, &wbemServices);

                    if (FAILED(hr))
                        throw SystemError("Failed to create a connection to the WMI namespace, error: " + std::to_string(hr));
                    else
                    {
                        hr = CoSetProxyBlanket(wbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
                                               RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);

                        if (FAILED(hr))
                            throw SystemError("Failed to set authentication information, error: " + std::to_string(hr));
                        else
                        {
                            IEnumWbemClassObject* enumDevices = nullptr;
                            hr = wbemServices->CreateInstanceEnum(className, 0, nullptr, &enumDevices);
                            if (FAILED(hr))
                                throw SystemError("Failed to create the device enumerator, error: " + std::to_string(hr));
                            else
                            {
                                // Get 20 at a time
                                ULONG returned = 0;
                                IWbemClassObject* devices[20];
                                while (enumDevices->Next(10000, 20, devices, &returned) == WBEM_S_NO_ERROR)
                                {
                                    for (ULONG device = 0; device < returned; ++device)
                                    {
                                        // For each device, get its device ID
                                        VARIANT var;
                                        hr = devices[device]->Get(deviceID, 0L, &var, nullptr, nullptr);
                                        if (SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != nullptr)
                                        {
                                            // Check if the device ID contains "IG_". If it does, then it's an XInput device
                                            // This information can not be found from DirectInput
                                            if (wcsstr(var.bstrVal, L"IG_"))
                                            {
                                                // If it does, then get the VID/PID from var.bstrVal
                                                DWORD dwPid = 0, dwVid = 0;
                                                WCHAR* strVid = wcsstr(var.bstrVal, L"VID_");
                                                if (strVid && swscanf(strVid, L"VID_%4X", &dwVid) != 1) dwVid = 0;
                                                WCHAR* strPid = wcsstr(var.bstrVal, L"PID_");
                                                if (strPid && swscanf(strPid, L"PID_%4X", &dwPid) != 1) dwPid = 0;

                                                // Compare the VID/PID to the DInput device
                                                DWORD dwVidPid = MAKELONG(dwVid, dwPid);
                                                if (dwVidPid == didInstance->guidProduct.Data1)
                                                    isXInputDevice = true;
                                            }
                                        }

                                        devices[device]->Release();
                                    }
                                }

                                enumDevices->Release();
                            }
                        }
                    }

                    wbemServices->Release();
                }

                if (namespaceStr) SysFreeString(namespaceStr);
                if (deviceID) SysFreeString(deviceID);
                if (className) SysFreeString(className);

                wbemLocator->Release();
            }

            if (!isXInputDevice)
            {
                bool found = false;

                for (auto i = gamepadsDI.begin(); i != gamepadsDI.end();)
                {
                    GamepadDI* gamepadDI = *i;

                    if (gamepadDI->getInstance()->guidInstance == didInstance->guidInstance)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    NativeWindowWin* windowWin = static_cast<NativeWindowWin*>(engine->getWindow()->getNativeWindow());

                    std::unique_ptr<GamepadDI> gamepad(new GamepadDI(didInstance, directInput, windowWin->getNativeWindow()));
                    gamepadsDI.push_back(gamepad.get());

                    Event event;
                    event.type = Event::Type::GAMEPAD_CONNECT;
                    event.gamepadEvent.gamepad = gamepad.get();

                    gamepads.push_back(std::move(gamepad));

                    engine->getEventDispatcher()->postEvent(event);
                }
            }
        }

        void InputManagerWin::updateCursor()
        {
            if (cursorVisible)
                SetCursor(currentCursor);
            else
                SetCursor(nullptr);
        }
    } // namespace input
} // namespace ouzel
