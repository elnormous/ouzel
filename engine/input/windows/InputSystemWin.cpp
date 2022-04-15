// Ouzel by Elviss Strazdins

#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <array>
#include <Windows.h>
#include <WbemIdl.h>
#include <OleAuto.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

#include "InputSystemWin.hpp"
#include "DIErrorCategory.hpp"
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
        BOOL CALLBACK enumDevicesCallback(const DIDEVICEINSTANCEW* didInstance, VOID* context)
        {
            const auto inputSystem = static_cast<InputSystem*>(context);
            inputSystem->handleDeviceConnect(didInstance);

            return DIENUM_CONTINUE;
        }
    }

    InputSystem::InputSystem():
        keyboardDevice(std::make_unique<KeyboardDevice>(*this, getNextDeviceId())),
        mouseDevice(std::make_unique<MouseDevice>(*this, getNextDeviceId())),
        touchpadDevice(std::make_unique<TouchpadDevice>(*this, getNextDeviceId(), true))
    {
        defaultCursor = LoadCursor(nullptr, IDC_ARROW);
        if (!defaultCursor)
            throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to load cursor"};

        HINSTANCE instance = GetModuleHandleW(nullptr);
        if (!instance)
            throw std::system_error{static_cast<int>(GetLastError()), std::system_category(), "Failed to get module handle"};

        void* directInputPointer;
        if (const auto result = DirectInput8Create(instance, DIRECTINPUT_VERSION, IID_IDirectInput8W, &directInputPointer, nullptr); FAILED(result))
            throw std::system_error{result, errorCategory, "Failed to initialize DirectInput"};

        directInput = static_cast<IDirectInput8W*>(directInputPointer);

        for (DWORD userIndex = 0; userIndex < XUSER_MAX_COUNT; ++userIndex)
        {
            XINPUT_STATE state{};
            DWORD result = XInputGetState(userIndex, &state);

            if (result == ERROR_SUCCESS)
                gamepadsXI[userIndex] = std::make_unique<GamepadDeviceXI>(*this, getNextDeviceId(), userIndex);
            else if (result != ERROR_DEVICE_NOT_CONNECTED)
                throw std::system_error{static_cast<int>(result), std::system_category(), "Failed to get state for gamepad " + std::to_string(userIndex)};
        }

        if (const auto result = directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, enumDevicesCallback, this, DIEDFL_ATTACHEDONLY); FAILED(result))
            throw std::system_error{result, errorCategory, "Failed to enumerate devices"};
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
                    XINPUT_STATE state{};

                    if (const auto result = XInputGetState(userIndex, &state); result == ERROR_SUCCESS)
                        gamepadsXI[userIndex] = std::make_unique<GamepadDeviceXI>(*this, getNextDeviceId(), userIndex);
                    else if (result != ERROR_DEVICE_NOT_CONNECTED)
                        throw std::system_error{static_cast<int>(result), std::system_category(), "Failed to get state for gamepad " + std::to_string(userIndex)};
                }
            }

            if (const auto result = directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, enumDevicesCallback, this, DIEDFL_ATTACHEDONLY); FAILED(result))
                throw std::system_error{result, errorCategory, "Failed to enumerate devices"};
        }
    }

    void InputSystem::handleDeviceConnect(const DIDEVICEINSTANCEW* didInstance)
    {
        bool isXInputDevice = false;

        void* wbemLocatorPointer;
        if (const auto result = CoCreateInstance(__uuidof(WbemLocator), nullptr, CLSCTX_INPROC_SERVER,
                                                 __uuidof(IWbemLocator), &wbemLocatorPointer); FAILED(result))
            throw std::system_error{result, std::system_category(), "Failed to create WMI locator instance"};

        const auto wbemLocator = static_cast<IWbemLocator*>(wbemLocatorPointer);

        using SysFreeStringFunction = void(__stdcall*)(BSTR);
        std::unique_ptr<WCHAR, SysFreeStringFunction> namespaceStr(SysAllocString(L"\\\\.\\root\\cimv2"), &SysFreeString);
        std::unique_ptr<WCHAR, SysFreeStringFunction> className(SysAllocString(L"Win32_PNPEntity"), &SysFreeString);
        std::unique_ptr<WCHAR, SysFreeStringFunction> deviceID(SysAllocString(L"DeviceID"), &SysFreeString);

        if (className && namespaceStr && deviceID)
        {
            IWbemServices* wbemServices = nullptr;
            if (const auto result = wbemLocator->ConnectServer(namespaceStr.get(), nullptr, nullptr, 0L,
                                                               0L, nullptr, nullptr, &wbemServices); FAILED(result))
                throw std::system_error{result, errorCategory, "Failed to create a connection to the WMI namespace"};

            if (const auto result = CoSetProxyBlanket(wbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
                                                      RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE); FAILED(result))
                throw std::system_error{result, errorCategory, "Failed to set authentication information"};

            IEnumWbemClassObject* enumDevices = nullptr;
            if (const auto result = wbemServices->CreateInstanceEnum(className.get(), 0, nullptr, &enumDevices); FAILED(result))
                throw std::system_error{result, errorCategory, "Failed to create the device enumerator"};

            // Get 20 at a time
            ULONG deviceCount = 0;
            std::array<IWbemClassObject*, 20> devices;
            while (enumDevices->Next(10000,
                                     static_cast<ULONG>(devices.size()),
                                     devices.data(),
                                     &deviceCount) == WBEM_S_NO_ERROR)
            {
                for (ULONG device = 0; device < deviceCount; ++device)
                {
                    // For each device, get its device ID
                    VARIANT var;
                    if (const auto result = devices[device]->Get(deviceID.get(), 0L, &var, nullptr, nullptr);
                        SUCCEEDED(result) && var.vt == VT_BSTR && var.bstrVal != nullptr)
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
                const auto& windowWin = engine->getWindow().getNativeWindow();

                gamepadsDI.emplace_back(std::make_unique<GamepadDeviceDI>(*this, getNextDeviceId(),
                                                                          didInstance, directInput,
                                                                          windowWin.getNativeWindow()));
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
