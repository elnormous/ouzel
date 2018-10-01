// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <unordered_map>
#include <Windows.h>
#include <WbemIdl.h>
#include <OleAuto.h>
#include "InputManagerWin.hpp"
#include "NativeCursorWin.hpp"
#include "GamepadDeviceDI.hpp"
#include "GamepadDeviceXI.hpp"
#include "core/Engine.hpp"
#include "core/windows/NativeWindowWin.hpp"
#include "events/EventDispatcher.hpp"
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
                    Event event;
                    event.type = Event::Type::GAMEPAD_CONNECT;

                    /*std::unique_ptr<GamepadDeviceXI> gamepad(new GamepadDeviceXI(userIndex));
                    gamepadsXI[userIndex] = gamepad.get();
                    event.gamepadEvent.gamepad = gamepad.get();
                    gamepads.push_back(std::move(gamepad));*/

                    engine->getEventDispatcher().postEvent(event);
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
            /*if (keyboardKeyStates[static_cast<uint32_t>(Keyboard::Key::LEFT_SHIFT)] &&
                (GetKeyState(VK_LSHIFT) & 0x8000) == 0)
                keyRelease(Keyboard::Key::LEFT_SHIFT, 0);

            if (keyboardKeyStates[static_cast<uint32_t>(Keyboard::Key::RIGHT_SHIFT)] &&
                (GetKeyState(VK_RSHIFT) & 0x8000) == 0)
                keyRelease(Keyboard::Key::RIGHT_SHIFT, 0);

            for (DWORD userIndex = 0; userIndex < XUSER_MAX_COUNT; ++userIndex)
            {
                GamepadDeviceXI* gamepadXI = gamepadsXI[userIndex];

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

                        engine->getEventDispatcher().postEvent(event);

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
                GamepadDeviceDI* gamepadDI = *i;
                try
                {
                    gamepadDI->update();
                    ++i;
                }
                catch (...)
                {
                    Event event;
                    event.type = Event::Type::GAMEPAD_DISCONNECT;
                    //event.gamepadEvent.gamepad = gamepadDI;

                    engine->getEventDispatcher().postEvent(event);

                    i = gamepadsDI.erase(i);

                    auto gamepadIterator = std::find_if(gamepads.begin(), gamepads.end(), [gamepadDI](const std::unique_ptr<Gamepad>& gamepad) {
                        return gamepadDI == gamepad.get();
                    });

                    if (gamepadIterator != gamepads.end())
                        gamepads.erase(gamepadIterator);
                }
            }*/
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
            std::unique_lock<std::mutex> lock(resourceMutex);

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

        /*void InputManagerWin::setCursorPosition(const Vector2& position)
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
        }*/

        /*void InputManagerWin::startDeviceDiscovery()
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
                            Event event;
                            event.type = Event::Type::GAMEPAD_CONNECT;

                            std::unique_ptr<GamepadDeviceXI> gamepad(new GamepadDeviceXI(userIndex));
                            gamepadsXI[userIndex] = gamepad.get();
                            event.gamepadEvent.gamepad = gamepad.get();

                            gamepads.push_back(std::move(gamepad));

                            engine->getEventDispatcher().postEvent(event);
                        }
                        else if (result != ERROR_DEVICE_NOT_CONNECTED)
                            Log(Log::Level::WARN) << "Failed to get state for gamepad " << userIndex;
                    }
                }

                HRESULT hr = directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, enumDevicesCallback, this, DIEDFL_ATTACHEDONLY);
                if (FAILED(hr))
                    throw SystemError("Failed to enumerate devices, error: " + std::to_string(hr));
            });
        }*/

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
                    GamepadDeviceDI* gamepadDI = *i;

                    if (gamepadDI->getInstance()->guidInstance == didInstance->guidInstance)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    NativeWindowWin* windowWin = static_cast<NativeWindowWin*>(engine->getWindow()->getNativeWindow());

                    Event event;
                    event.type = Event::Type::GAMEPAD_CONNECT;
                    
                    /*std::unique_ptr<GamepadDeviceDI> gamepad(new GamepadDeviceDI(didInstance, directInput, windowWin->getNativeWindow()));
                    gamepadsDI.push_back(gamepad.get());
                    event.gamepadEvent.gamepad = gamepad.get();
                    gamepads.push_back(std::move(gamepad));*/

                    engine->getEventDispatcher().postEvent(event);
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
