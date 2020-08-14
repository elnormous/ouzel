// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

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
#include "../../core/Engine.hpp"
#include "../../core/windows/NativeWindowWin.hpp"
#include "../../utils/Log.hpp"

namespace ouzel::input::windows
{
    namespace
    {
        class ErrorCategory final: public std::error_category
        {
        public:
            const char* name() const noexcept final
            {
                return "DirectInput";
            }

            std::string message(int condition) const final
            {
                switch (condition)
                {
                    case DIERR_ACQUIRED: return "DIERR_ACQUIRED";
                    case DIERR_ALREADYINITIALIZED: return "DIERR_ALREADYINITIALIZED";
                    case DIERR_BADDRIVERVER: return "DIERR_BADDRIVERVER";
                    case DIERR_BETADIRECTINPUTVERSION: return "DIERR_BETADIRECTINPUTVERSION";
                    case DIERR_DEVICEFULL: return "DIERR_DEVICEFULL";
                    case DIERR_DEVICENOTREG: return "DIERR_DEVICENOTREG";
                    case DIERR_EFFECTPLAYING: return "DIERR_EFFECTPLAYING";
                    case DIERR_GENERIC: return "DIERR_GENERIC";
                    case DIERR_HANDLEEXISTS: return "DIERR_HANDLEEXISTS";
                    case DIERR_HASEFFECTS: return "DIERR_HASEFFECTS";
                    case DIERR_INCOMPLETEEFFECT: return "DIERR_INCOMPLETEEFFECT";
                    case DIERR_INPUTLOST: return "DIERR_INPUTLOST";
                    case DIERR_INVALIDPARAM: return "DIERR_INVALIDPARAM";
                    case DIERR_MAPFILEFAIL: return "DIERR_MAPFILEFAIL";
                    case DIERR_MOREDATA: return "DIERR_MOREDATA";
                    case DIERR_NOAGGREGATION: return "DIERR_NOAGGREGATION";
                    case DIERR_NOINTERFACE: return "DIERR_NOINTERFACE";
                    case DIERR_NOTACQUIRED: return "DIERR_NOTACQUIRED";
                    case DIERR_NOTBUFFERED: return "DIERR_NOTBUFFERED";
                    case DIERR_NOTDOWNLOADED: return "DIERR_NOTDOWNLOADED";
                    case DIERR_NOTEXCLUSIVEACQUIRED: return "DIERR_NOTEXCLUSIVEACQUIRED";
                    case DIERR_NOTFOUND: return "DIERR_NOTFOUND";
                    case DIERR_NOTINITIALIZED: return "DIERR_NOTINITIALIZED";
                    case DIERR_OLDDIRECTINPUTVERSION: return "DIERR_OLDDIRECTINPUTVERSION";
                    case DIERR_OUTOFMEMORY: return "DIERR_OUTOFMEMORY";
                    case DIERR_REPORTFULL: return "DIERR_REPORTFULL";
                    case DIERR_UNPLUGGED: return "DIERR_UNPLUGGED";
                    case DIERR_UNSUPPORTED: return "DIERR_UNSUPPORTED";
                    case E_HANDLE: return "E_HANDLE";
                    case E_PENDING: return "E_PENDING";
                    case E_POINTER: return "E_POINTER";
                    default: return "Unknown error (" + std::to_string(condition) + ")";
                }
            }
        };

        const ErrorCategory errorCategory{};

        BOOL CALLBACK enumDevicesCallback(const DIDEVICEINSTANCEW* didInstance, VOID* context)
        {
            auto inputWin = static_cast<InputSystem*>(context);
            inputWin->handleDeviceConnect(didInstance);

            return DIENUM_CONTINUE;
        }
    }

    const std::error_category& getErrorCategory() noexcept
    {
        return errorCategory;
    }

    InputSystem::InputSystem(const std::function<std::future<bool>(const Event&)>& initCallback):
                             input::InputSystem(initCallback),
                             keyboardDevice(std::make_unique<KeyboardDevice>(*this, getNextDeviceId())),
                             mouseDevice(std::make_unique<MouseDevice>(*this, getNextDeviceId())),
                             touchpadDevice(std::make_unique<TouchpadDevice>(*this, getNextDeviceId(), true))
    {
        defaultCursor = LoadCursor(nullptr, IDC_ARROW);

        HINSTANCE instance = GetModuleHandleW(nullptr);
        if (!instance)
            throw std::system_error(GetLastError(), std::system_category(), "Failed to get module handle");

        void* directInputPointer;
        if (const auto hr = DirectInput8Create(instance, DIRECTINPUT_VERSION, IID_IDirectInput8W, &directInputPointer, nullptr); FAILED(hr))
            throw std::system_error(hr, errorCategory, "Failed to initialize DirectInput");

        directInput = static_cast<IDirectInput8W*>(directInputPointer);

        for (DWORD userIndex = 0; userIndex < XUSER_MAX_COUNT; ++userIndex)
        {
            XINPUT_STATE state = {};
            DWORD result = XInputGetState(userIndex, &state);

            if (result == ERROR_SUCCESS)
                gamepadsXI[userIndex] = std::make_unique<GamepadDeviceXI>(*this, getNextDeviceId(), userIndex);
            else if (result != ERROR_DEVICE_NOT_CONNECTED)
                throw std::system_error(result, std::system_category(), "Failed to get state for gamepad " + std::to_string(userIndex));
        }

        if (const auto hr = directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, enumDevicesCallback, this, DIEDFL_ATTACHEDONLY); FAILED(hr))
            throw std::system_error(hr, errorCategory, "Failed to enumerate devices");
    }

    InputSystem::~InputSystem()
    {
        if (directInput) directInput->Release();
    }

    void InputSystem::executeCommand(const Command& command)
    {
        switch (command.type)
        {
            case Command::Type::startDeviceDiscovery:
                discovering = true;
                break;
            case Command::Type::stopDeviceDiscovery:
                discovering = false;
                break;
            case Command::Type::setPlayerIndex:
            {
                break;
            }
            case Command::Type::setVibration:
            {
                break;
            }
            case Command::Type::setPosition:
            {
                if (InputDevice* inputDevice = getInputDevice(command.deviceId))
                {
                    if (inputDevice == mouseDevice.get())
                        mouseDevice->setPosition(command.position);
                }
                break;
            }
            case Command::Type::initCursor:
            {
                if (command.cursorResource > cursors.size())
                    cursors.resize(command.cursorResource);

                if (command.data.empty())
                {
                    auto cursor = std::make_unique<Cursor>(command.systemCursor);
                    cursors[command.cursorResource - 1] = std::move(cursor);
                }
                else
                {
                    auto cursor = std::make_unique<Cursor>(command.data, command.size,
                                                           command.pixelFormat, command.hotSpot);
                    cursors[command.cursorResource - 1] = std::move(cursor);
                }
                break;
            }
            case Command::Type::destroyCursor:
            {
                Cursor* cursor = cursors[command.cursorResource - 1].get();

                if (mouseDevice->getCursor() == cursor)
                {
                    mouseDevice->setCursor(nullptr);
                    updateCursor();
                }

                cursors[command.cursorResource - 1].reset();
                break;
            }
            case Command::Type::setCursor:
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
            case Command::Type::setCursorVisible:
            {
                if (InputDevice* inputDevice = getInputDevice(command.deviceId))
                {
                    if (inputDevice == mouseDevice.get())
                        mouseDevice->setCursorVisible(command.visible);
                }
                break;
            }
            case Command::Type::setCursorLocked:
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

    void InputSystem::update()
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
                        gamepadsXI[userIndex] = std::make_unique<GamepadDeviceXI>(*this, getNextDeviceId(), userIndex);
                    else if (result != ERROR_DEVICE_NOT_CONNECTED)
                        throw std::system_error(result, std::system_category(), "Failed to get state for gamepad " + std::to_string(userIndex));
                }
            }

            if (const auto hr = directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, enumDevicesCallback, this, DIEDFL_ATTACHEDONLY); FAILED(hr))
                throw std::system_error(hr, errorCategory, "Failed to enumerate devices");
        }
    }

    void InputSystem::handleDeviceConnect(const DIDEVICEINSTANCEW* didInstance)
    {
        bool isXInputDevice = false;

        void* wbemLocatorPointer;
        if (const auto hr = CoCreateInstance(__uuidof(WbemLocator), nullptr, CLSCTX_INPROC_SERVER,
                                             __uuidof(IWbemLocator), &wbemLocatorPointer); FAILED(hr))
            throw std::system_error(hr, errorCategory, "Failed to create WMI locator instance");

        auto wbemLocator = static_cast<IWbemLocator*>(wbemLocatorPointer);

        BSTR namespaceStr = SysAllocString(L"\\\\.\\root\\cimv2");
        BSTR className = SysAllocString(L"Win32_PNPEntity");
        BSTR deviceID = SysAllocString(L"DeviceID");

        if (className && namespaceStr && deviceID)
        {
            IWbemServices* wbemServices = nullptr;
            if (const auto hr = wbemLocator->ConnectServer(namespaceStr, nullptr, nullptr, 0L,
                                                           0L, nullptr, nullptr, &wbemServices); FAILED(hr))
                throw std::system_error(hr, errorCategory, "Failed to create a connection to the WMI namespace");

            if (const auto hr = CoSetProxyBlanket(wbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
                                                  RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE); FAILED(hr))
                throw std::system_error(hr, errorCategory, "Failed to set authentication information");

            IEnumWbemClassObject* enumDevices = nullptr;
            if (const auto hr = wbemServices->CreateInstanceEnum(className, 0, nullptr, &enumDevices); FAILED(hr))
                throw std::system_error(hr, errorCategory, "Failed to create the device enumerator");

            // Get 20 at a time
            ULONG returned = 0;
            IWbemClassObject* devices[20];
            while (enumDevices->Next(10000, 20, devices, &returned) == WBEM_S_NO_ERROR)
            {
                for (ULONG device = 0; device < returned; ++device)
                {
                    // For each device, get its device ID
                    VARIANT var;
                    const auto hr = devices[device]->Get(deviceID, 0L, &var, nullptr, nullptr);
                    if (SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != nullptr)
                    {
                        // Check if the device ID contains "IG_". If it does, then it's an XInput device
                        // This information can not be found from DirectInput
                        if (wcsstr(var.bstrVal, L"IG_"))
                        {
                            // If it does, then get the VID/PID from var.bstrVal
                            DWORD dwPid = 0;
                            DWORD dwVid = 0;
                            WCHAR* strVid = wcsstr(var.bstrVal, L"VID_");
                            if (strVid && swscanf(strVid, L"VID_%4X", &dwVid) != 1) dwVid = 0;
                            WCHAR* strPid = wcsstr(var.bstrVal, L"PID_");
                            if (strPid && swscanf(strPid, L"PID_%4X", &dwPid) != 1) dwPid = 0;

                            // Compare the VID/PID to the DInput device
                            const auto dwVidPid = static_cast<DWORD>(MAKELONG(dwVid, dwPid));
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
                auto windowWin = static_cast<core::windows::NativeWindow*>(engine->getWindow()->getNativeWindow());

                gamepadsDI.emplace_back(std::make_unique<GamepadDeviceDI>(*this, getNextDeviceId(),
                                                                          didInstance, directInput,
                                                                          windowWin->getNativeWindow()));
            }
        }
    }

    void InputSystem::updateCursor() const
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
}
