// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <unordered_map>
#include <Windows.h>
#include <WbemIdl.h>
#include <OleAuto.h>
#include "InputSystemWin.hpp"
#include "GamepadDeviceDI.hpp"
#include "GamepadDeviceXI.hpp"
#include "core/Engine.hpp"
#include "core/windows/NativeWindowWin.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

static BOOL CALLBACK enumDevicesCallback(const DIDEVICEINSTANCEW* didInstance, VOID* context)
{
    ouzel::input::InputSystemWin* inputWin = reinterpret_cast<ouzel::input::InputSystemWin*>(context);
    inputWin->handleDeviceConnect(didInstance);

    return DIENUM_CONTINUE;
}

namespace ouzel
{
    namespace input
    {
        static const std::unordered_map<UINT, Keyboard::Key> keyMap = {
            {VK_CANCEL, Keyboard::Key::CANCEL},
            {VK_BACK, Keyboard::Key::BACKSPACE},
            {VK_TAB, Keyboard::Key::TAB},
            {VK_CLEAR, Keyboard::Key::CLEAR},
            {VK_RETURN, Keyboard::Key::RETURN},

            {VK_MENU, Keyboard::Key::LEFT_ALT},
            {VK_PAUSE, Keyboard::Key::PAUSE},
            {VK_CAPITAL, Keyboard::Key::CAPITAL},

            {VK_ESCAPE, Keyboard::Key::ESCAPE},
            {VK_SPACE, Keyboard::Key::SPACE},
            {VK_PRIOR, Keyboard::Key::PRIOR},
            {VK_NEXT, Keyboard::Key::NEXT},
            {VK_END, Keyboard::Key::END},
            {VK_HOME, Keyboard::Key::HOME},
            {VK_LEFT, Keyboard::Key::LEFT},
            {VK_UP, Keyboard::Key::UP},
            {VK_RIGHT, Keyboard::Key::RIGHT},
            {VK_DOWN, Keyboard::Key::DOWN},

            {VK_SELECT, Keyboard::Key::SELECT},
            {VK_PRINT, Keyboard::Key::PRINT},
            {VK_EXECUTE, Keyboard::Key::EXECUTE},

            {VK_SNAPSHOT, Keyboard::Key::SNAPSHOT},
            {VK_INSERT, Keyboard::Key::INSERT},
            {VK_DELETE, Keyboard::Key::DEL},
            {VK_HELP, Keyboard::Key::HELP},

            {'0', Keyboard::Key::NUM_0},
            {'1', Keyboard::Key::NUM_1},
            {'2', Keyboard::Key::NUM_2},
            {'3', Keyboard::Key::NUM_3},
            {'4', Keyboard::Key::NUM_4},
            {'5', Keyboard::Key::NUM_5},
            {'6', Keyboard::Key::NUM_6},
            {'7', Keyboard::Key::NUM_7},
            {'8', Keyboard::Key::NUM_8},
            {'9', Keyboard::Key::NUM_9},

            {'A', Keyboard::Key::A},
            {'B', Keyboard::Key::B},
            {'C', Keyboard::Key::C},
            {'D', Keyboard::Key::D},
            {'E', Keyboard::Key::E},
            {'F', Keyboard::Key::F},
            {'G', Keyboard::Key::G},
            {'H', Keyboard::Key::H},
            {'I', Keyboard::Key::I},
            {'J', Keyboard::Key::J},
            {'K', Keyboard::Key::K},
            {'L', Keyboard::Key::L},
            {'M', Keyboard::Key::M},
            {'N', Keyboard::Key::N},
            {'O', Keyboard::Key::O},
            {'P', Keyboard::Key::P},
            {'Q', Keyboard::Key::Q},
            {'R', Keyboard::Key::R},
            {'S', Keyboard::Key::S},
            {'T', Keyboard::Key::T},
            {'U', Keyboard::Key::U},
            {'V', Keyboard::Key::V},
            {'W', Keyboard::Key::W},
            {'X', Keyboard::Key::X},
            {'Y', Keyboard::Key::Y},
            {'Z', Keyboard::Key::Z},

            {VK_LWIN, Keyboard::Key::LEFT_SUPER},
            {VK_RWIN, Keyboard::Key::RIGHT_SUPER},
            {VK_APPS, Keyboard::Key::MENU},
            {VK_SLEEP, Keyboard::Key::SLEEP},

            {VK_NUMPAD0, Keyboard::Key::NUMPAD_0},
            {VK_NUMPAD1, Keyboard::Key::NUMPAD_1},
            {VK_NUMPAD2, Keyboard::Key::NUMPAD_2},
            {VK_NUMPAD3, Keyboard::Key::NUMPAD_3},
            {VK_NUMPAD4, Keyboard::Key::NUMPAD_4},
            {VK_NUMPAD5, Keyboard::Key::NUMPAD_5},
            {VK_NUMPAD6, Keyboard::Key::NUMPAD_6},
            {VK_NUMPAD7, Keyboard::Key::NUMPAD_7},
            {VK_NUMPAD8, Keyboard::Key::NUMPAD_8},
            {VK_NUMPAD9, Keyboard::Key::NUMPAD_9},

            {VK_MULTIPLY, Keyboard::Key::MULTIPLY},
            {VK_ADD, Keyboard::Key::ADD},
            {VK_SEPARATOR, Keyboard::Key::SEPARATOR},
            {VK_SUBTRACT, Keyboard::Key::SUBTRACT},
            {VK_DECIMAL, Keyboard::Key::DECIMAL},
            {VK_DIVIDE, Keyboard::Key::DIVIDE},

            {VK_F1, Keyboard::Key::F1},
            {VK_F2, Keyboard::Key::F2},
            {VK_F3, Keyboard::Key::F3},
            {VK_F4, Keyboard::Key::F4},
            {VK_F5, Keyboard::Key::F5},
            {VK_F6, Keyboard::Key::F6},
            {VK_F7, Keyboard::Key::F7},
            {VK_F8, Keyboard::Key::F8},
            {VK_F9, Keyboard::Key::F9},
            {VK_F10, Keyboard::Key::F10},
            {VK_F11, Keyboard::Key::F11},
            {VK_F12, Keyboard::Key::F12},
            {VK_F13, Keyboard::Key::F13},
            {VK_F14, Keyboard::Key::F14},
            {VK_F15, Keyboard::Key::F15},
            {VK_F16, Keyboard::Key::F16},
            {VK_F17, Keyboard::Key::F17},
            {VK_F18, Keyboard::Key::F18},
            {VK_F19, Keyboard::Key::F19},
            {VK_F20, Keyboard::Key::F20},
            {VK_F21, Keyboard::Key::F21},
            {VK_F22, Keyboard::Key::F22},
            {VK_F23, Keyboard::Key::F23},
            {VK_F24, Keyboard::Key::F24},

            {VK_NUMLOCK, Keyboard::Key::NUMLOCK},
            {VK_SCROLL, Keyboard::Key::SCROLL},
            {VK_LSHIFT, Keyboard::Key::LEFT_SHIFT},
            {VK_RSHIFT, Keyboard::Key::RIGHT_SHIFT},
            {VK_LCONTROL, Keyboard::Key::LEFT_CONTROL},
            {VK_RCONTROL, Keyboard::Key::RIGHT_CONTROL},
            {VK_LMENU, Keyboard::Key::LEFT_ALT},
            {VK_RMENU, Keyboard::Key::RIGHT_ALT},

            {VK_OEM_1, Keyboard::Key::SEMICOLON},
            {VK_OEM_PLUS, Keyboard::Key::PLUS},
            {VK_OEM_COMMA, Keyboard::Key::COMMA},
            {VK_OEM_MINUS, Keyboard::Key::MINUS},
            {VK_OEM_PERIOD, Keyboard::Key::PERIOD},
            {VK_OEM_2, Keyboard::Key::SLASH},
            {VK_OEM_3, Keyboard::Key::GRAVE},
            {VK_OEM_4, Keyboard::Key::LEFT_BRACKET},
            {VK_OEM_5, Keyboard::Key::BACKSLASH},
            {VK_OEM_6, Keyboard::Key::RIGHT_BRACKET},
            {VK_OEM_7, Keyboard::Key::QUOTE},
            {VK_OEM_8, Keyboard::Key::GRAVE},
            {VK_OEM_AX, Keyboard::Key::OEM_AX},
            {VK_OEM_102, Keyboard::Key::LESS}
        };

        Keyboard::Key InputSystemWin::convertKeyCode(UINT keyCode)
        {
            auto i = keyMap.find(keyCode);

            if (i != keyMap.end())
                return i->second;
            else
                return Keyboard::Key::NONE;
        }

        InputSystemWin::InputSystemWin():
            keyboardDevice(new KeyboardDeviceWin(*this, ++lastDeviceId)),
            mouseDevice(new MouseDeviceWin(*this, ++lastDeviceId)),
            touchpadDevice(new TouchpadDevice(*this, ++lastDeviceId))
        {
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
                    gamepadsXI[userIndex].reset(new GamepadDeviceXI(*this, ++lastDeviceId, userIndex));
                }
                else if (result != ERROR_DEVICE_NOT_CONNECTED)
                    Log(Log::Level::WARN) << "Failed to get state for gamepad " << userIndex;
            }

            hr = directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, enumDevicesCallback, this, DIEDFL_ATTACHEDONLY);
            if (FAILED(hr))
                throw SystemError("Failed to enumerate devices, error: " + std::to_string(hr));
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
                case Command::Type::SET_CURSOR:
                {
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
                    catch (...)
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
                catch (...)
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
                        XINPUT_STATE state;
                        ZeroMemory(&state, sizeof(XINPUT_STATE));

                        DWORD result = XInputGetState(userIndex, &state);

                        if (result == ERROR_SUCCESS)
                            gamepadsXI[userIndex].reset(new GamepadDeviceXI(*this, ++lastDeviceId, userIndex));
                        else if (result != ERROR_DEVICE_NOT_CONNECTED)
                            Log(Log::Level::WARN) << "Failed to get state for gamepad " << userIndex;
                    }
                }

                HRESULT hr = directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, enumDevicesCallback, this, DIEDFL_ATTACHEDONLY);
                if (FAILED(hr))
                    throw SystemError("Failed to enumerate devices, error: " + std::to_string(hr));
            }
        }

        void InputSystemWin::handleDeviceConnect(const DIDEVICEINSTANCEW* didInstance)
        {
            bool isXInputDevice = false;

            IWbemLocator* wbemLocator = nullptr;
            HRESULT hr = CoCreateInstance(__uuidof(WbemLocator), nullptr, CLSCTX_INPROC_SERVER,
                                          __uuidof(IWbemLocator), reinterpret_cast<LPVOID*>(&wbemLocator));

            if (FAILED(hr))
                throw SystemError("Failed to create WMI locator instance, error: " + std::to_string(hr));

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

                hr = CoSetProxyBlanket(wbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
                    RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);

                if (FAILED(hr))
                    throw SystemError("Failed to set authentication information, error: " + std::to_string(hr));

                IEnumWbemClassObject* enumDevices = nullptr;
                hr = wbemServices->CreateInstanceEnum(className, 0, nullptr, &enumDevices);
                if (FAILED(hr))
                    throw SystemError("Failed to create the device enumerator, error: " + std::to_string(hr));

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

                    gamepadsDI.push_back(std::unique_ptr<GamepadDeviceDI>(new GamepadDeviceDI(*this, ++lastDeviceId,
                                                                                              didInstance, directInput,
                                                                                              windowWin->getNativeWindow())));
                }
            }
        }
    } // namespace input
} // namespace ouzel
