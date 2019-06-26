// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <Windows.h>
#include <WbemIdl.h>
#include <OleAuto.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

#include "InputSystemWin.hpp"
#include "GamepadDeviceDI.hpp"
#include "GamepadDeviceXI.hpp"
#include "CursorWin.hpp"
#include "core/Engine.hpp"
#include "core/windows/NativeWindowWin.hpp"
#include "utils/Log.hpp"

static BOOL CALLBACK enumDevicesCallback(const DIDEVICEINSTANCEW* didInstance, VOID* context)
{
    ouzel::input::InputSystemWin* inputWin = static_cast<ouzel::input::InputSystemWin*>(context);
    inputWin->handleDeviceConnect(didInstance);

    return DIENUM_CONTINUE;
}

namespace ouzel
{
    namespace input
    {
        const DirectInputErrorCategory directInputErrorCategory {};

        InputSystemWin::InputSystemWin(const std::function<std::future<bool>(const Event&)>& initCallback):
            InputSystem(initCallback),
            keyboardDevice(new KeyboardDeviceWin(*this, ++lastDeviceId)),
            mouseDevice(new MouseDeviceWin(*this, ++lastDeviceId)),
            touchpadDevice(new TouchpadDevice(*this, ++lastDeviceId, true))
        {
            defaultCursor = LoadCursor(nullptr, IDC_ARROW);

            HINSTANCE instance = GetModuleHandleW(nullptr);
            if (!instance)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to get module handle");

            HRESULT hr;
            if (FAILED(hr = DirectInput8Create(instance, DIRECTINPUT_VERSION, IID_IDirectInput8W, reinterpret_cast<LPVOID*>(&directInput), nullptr)))
                throw std::system_error(hr, directInputErrorCategory, "Failed to initialize DirectInput");

            for (DWORD userIndex = 0; userIndex < XUSER_MAX_COUNT; ++userIndex)
            {
                XINPUT_STATE state = {};
                DWORD result = XInputGetState(userIndex, &state);

                if (result == ERROR_SUCCESS)
                    gamepadsXI[userIndex].reset(new GamepadDeviceXI(*this, ++lastDeviceId, userIndex));
                else if (result != ERROR_DEVICE_NOT_CONNECTED)
                    throw std::system_error(result, std::system_category(), "Failed to get state for gamepad " + std::to_string(userIndex));
            }

            if (FAILED(hr = directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, enumDevicesCallback, this, DIEDFL_ATTACHEDONLY)))
                throw std::system_error(hr, directInputErrorCategory, "Failed to enumerate devices");
        }

        InputSystemWin::~InputSystemWin()
        {
            if (directInput) directInput->Release();
        }

        void InputSystemWin::executeCommand(const Command& command)
        {
            switch (command.type)
            {
                case Command::Type::START_DEVICE_DISCOVERY:
                    discovering = true;
                    break;
                case Command::Type::STOP_DEVICE_DISCOVERY:
                    discovering = false;
                    break;
                case Command::Type::SET_PLAYER_INDEX:
                {
                    break;
                }
                case Command::Type::SET_VIBRATION:
                {
                    break;
                }
                case Command::Type::SET_POSITION:
                {
                    if (InputDevice* inputDevice = getInputDevice(command.deviceId))
                    {
                        if (inputDevice == mouseDevice.get())
                            mouseDevice->setPosition(command.position);
                    }
                    break;
                }
                case Command::Type::INIT_CURSOR:
                {
                    if (command.cursorResource > cursors.size())
                        cursors.resize(command.cursorResource);

                    if (command.data.empty())
                    {
                        std::unique_ptr<CursorWin> cursor(new CursorWin(command.systemCursor));
                        cursors[command.cursorResource - 1] = std::move(cursor);
                    }
                    else
                    {
                        std::unique_ptr<CursorWin> cursor(new CursorWin(command.data, command.size,
                                                                                    command.pixelFormat, command.hotSpot));
                        cursors[command.cursorResource - 1] = std::move(cursor);
                    }
                    break;
                }
                case Command::Type::DESTROY_CURSOR:
                {
                    CursorWin* cursor = cursors[command.cursorResource - 1].get();

                    if (mouseDevice->getCursor() == cursor)
                    {
                        mouseDevice->setCursor(nullptr);
                        updateCursor();
                    }

                    cursors[command.cursorResource - 1].reset();
                    break;
                }
                case Command::Type::SET_CURSOR:
                {
                    if (InputDevice* inputDevice = getInputDevice(command.deviceId))
                    {
                        if (inputDevice == mouseDevice.get())
                        {
                            if (command.cursorResource)
                                mouseDevice->setCursor(cursors[command.cursorResource - 1].get());
                            else
                                mouseDevice->setCursor(nullptr);

                            updateCursor();
                        }
                    }
                    break;
                }
                case Command::Type::SET_CURSOR_VISIBLE:
                {
                    if (InputDevice* inputDevice = getInputDevice(command.deviceId))
                    {
                        if (inputDevice == mouseDevice.get())
                            mouseDevice->setCursorVisible(command.visible);
                    }
                    break;
                }
                case Command::Type::SET_CURSOR_LOCKED:
                {
                    if (InputDevice* inputDevice = getInputDevice(command.deviceId))
                    {
                        if (inputDevice == mouseDevice.get())
                            mouseDevice->setCursorLocked(command.locked);
                    }
                    break;
                }
                default:
                    break;
            }
        }

        void InputSystemWin::update()
        {
            keyboardDevice->update();

            for (DWORD userIndex = 0; userIndex < XUSER_MAX_COUNT; ++userIndex)
            {
                if (gamepadsXI[userIndex])
                {
                    try
                    {
                        gamepadsXI[userIndex]->update();
                    }
                    catch (const std::exception&)
                    {
                        gamepadsXI[userIndex].reset();
                    }
                }
            }

            for (auto i = gamepadsDI.begin(); i != gamepadsDI.end();)
            {
                try
                {
                    (*i)->update();
                    ++i;
                }
                catch (const std::exception&)
                {
                    i = gamepadsDI.erase(i);
                }
            }

            if (discovering)
            {
                for (DWORD userIndex = 0; userIndex < XUSER_MAX_COUNT; ++userIndex)
                {
                    if (!gamepadsXI[userIndex])
                    {
                        XINPUT_STATE state = {};
                        DWORD result = XInputGetState(userIndex, &state);

                        if (result == ERROR_SUCCESS)
                            gamepadsXI[userIndex].reset(new GamepadDeviceXI(*this, ++lastDeviceId, userIndex));
                        else if (result != ERROR_DEVICE_NOT_CONNECTED)
                            throw std::system_error(result, std::system_category(), "Failed to get state for gamepad " + std::to_string(userIndex));
                    }
                }

                HRESULT hr;
                if (FAILED(hr = directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, enumDevicesCallback, this, DIEDFL_ATTACHEDONLY)))
                    throw std::system_error(hr, directInputErrorCategory, "Failed to enumerate devices");
            }
        }

        void InputSystemWin::handleDeviceConnect(const DIDEVICEINSTANCEW* didInstance)
        {
            bool isXInputDevice = false;

            IWbemLocator* wbemLocator = nullptr;
            HRESULT hr;

            if (FAILED(hr = CoCreateInstance(__uuidof(WbemLocator), nullptr, CLSCTX_INPROC_SERVER,
                                             __uuidof(IWbemLocator), reinterpret_cast<LPVOID*>(&wbemLocator))))
                throw std::system_error(hr, directInputErrorCategory, "Failed to create WMI locator instance");

            BSTR namespaceStr = SysAllocString(L"\\\\.\\root\\cimv2");
            BSTR className = SysAllocString(L"Win32_PNPEntity");
            BSTR deviceID = SysAllocString(L"DeviceID");

            if (className && namespaceStr && deviceID)
            {
                IWbemServices* wbemServices = nullptr;

                if (FAILED(hr = wbemLocator->ConnectServer(namespaceStr, nullptr, nullptr, 0L,
                                                           0L, nullptr, nullptr, &wbemServices)))
                    throw std::system_error(hr, directInputErrorCategory, "Failed to create a connection to the WMI namespace");

                if (FAILED(hr = CoSetProxyBlanket(wbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
                                                  RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE)))
                    throw std::system_error(hr, directInputErrorCategory, "Failed to set authentication information");

                IEnumWbemClassObject* enumDevices = nullptr;
                if (FAILED(hr = wbemServices->CreateInstanceEnum(className, 0, nullptr, &enumDevices)))
                    throw std::system_error(hr, directInputErrorCategory, "Failed to create the device enumerator");

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
                                DWORD dwPid = 0
                                DWORD dwVid = 0;
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
                wbemServices->Release();
            }

            if (namespaceStr) SysFreeString(namespaceStr);
            if (deviceID) SysFreeString(deviceID);
            if (className) SysFreeString(className);

            wbemLocator->Release();

            if (!isXInputDevice)
            {
                bool found = false;

                for (auto i = gamepadsDI.begin(); i != gamepadsDI.end();)
                {
                    if ((*i)->getInstance()->guidInstance == didInstance->guidInstance)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    NativeWindowWin* windowWin = static_cast<NativeWindowWin*>(engine->getWindow()->getNativeWindow());

                    gamepadsDI.emplace_back(new GamepadDeviceDI(*this, ++lastDeviceId,
                                                                didInstance, directInput,
                                                                windowWin->getNativeWindow()));
                }
            }
        }

        void InputSystemWin::updateCursor() const
        {
            if (mouseDevice->isCursorVisible())
            {
                if (mouseDevice->getCursor())
                    SetCursor(mouseDevice->getCursor()->getCursor());
                else
                    SetCursor(defaultCursor);
            }
            else
                SetCursor(nullptr);
        }
    } // namespace input
} // namespace ouzel
