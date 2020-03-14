// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <array>
#include "GamepadDeviceDI.hpp"
#include "InputSystemWin.hpp"
#include "core/Engine.hpp"
#include "core/windows/NativeWindowWin.hpp"
#include "input/GamepadConfig.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace input
    {
        namespace
        {
            constexpr float THUMB_DEADZONE = 0.2F;
            constexpr std::size_t INPUT_QUEUE_SIZE = 32;

            // converts the angle to the hat value
            constexpr std::uint32_t getHatValue(std::uint32_t value) noexcept
            {
                return (value == 0xFFFFFFFF) ?
                    8 : ((value + 4500 / 2) % 36000) / 4500; // round up
            }
        }

        GamepadDeviceDI::GamepadDeviceDI(InputSystem& initInputSystem,
                                         DeviceId initId,
                                         const DIDEVICEINSTANCEW* initInstance, IDirectInput8W* directInput, HWND window):
            GamepadDeviceWin(initInputSystem, initId),
            instance(initInstance)
        {
            const std::int32_t vendorId = LOWORD(instance->guidProduct.Data1);
            const std::int32_t productId = HIWORD(instance->guidProduct.Data1);

            const int bufferSize = WideCharToMultiByte(CP_UTF8, 0, instance->tszProductName, -1, nullptr, 0, nullptr, nullptr);
            if (bufferSize != 0)
            {
                std::vector<char> buffer(bufferSize);
                if (WideCharToMultiByte(CP_UTF8, 0, instance->tszProductName, -1, buffer.data(), bufferSize, nullptr, nullptr) != 0)
                    name = buffer.data();
            }

            HRESULT hr;
            if (FAILED(hr = directInput->CreateDevice(instance->guidInstance, &device, nullptr)))
                throw std::system_error(hr, getErrorCategory(), "Failed to create DirectInput device");

            // Exclusive access is needed for force feedback
            if (FAILED(hr = device->SetCooperativeLevel(window, DISCL_BACKGROUND | DISCL_EXCLUSIVE)))
                throw std::system_error(hr, getErrorCategory(), "Failed to set DirectInput device format");

            if (FAILED(hr = device->SetDataFormat(&c_dfDIJoystick)))
                throw std::system_error(hr, getErrorCategory(), "Failed to set DirectInput device format");

            const GamepadConfig& gamepadConfig = getGamepadConfig(vendorId, productId);

            for (std::size_t i = 0; i < 24; ++i)
            {
                if (gamepadConfig.buttonMap[i] != Gamepad::Button::Unknown)
                {
                    Button button;
                    button.button = gamepadConfig.buttonMap[i];
                    buttons.insert(std::make_pair(static_cast<DWORD>(DIJOFS_BUTTON(i)), button));
                }
            }

            constexpr std::array<std::pair<USAGE, DWORD>, 6> axisUsageMap = {{
                {HID_USAGE_GENERIC_X, static_cast<DWORD>(offsetof(DIJOYSTATE, lX))}, // DIJOFS_X
                {HID_USAGE_GENERIC_X, static_cast<DWORD>(offsetof(DIJOYSTATE, lY))}, // DIJOFS_Y
                {HID_USAGE_GENERIC_X, static_cast<DWORD>(offsetof(DIJOYSTATE, lZ))}, // DIJOFS_Z
                {HID_USAGE_GENERIC_X, static_cast<DWORD>(offsetof(DIJOYSTATE, lRx))}, // DIJOFS_RX
                {HID_USAGE_GENERIC_X, static_cast<DWORD>(offsetof(DIJOYSTATE, lRy))}, // DIJOFS_RY
                {HID_USAGE_GENERIC_X, static_cast<DWORD>(offsetof(DIJOYSTATE, lRz))} // DIJOFS_RZ
            }};

            for (std::size_t i = 0; i < 6; ++i)
            {
                if (gamepadConfig.axisMap[i] != Gamepad::Axis::Unknown)
                {
                    const USAGE usage = axisUsageMap[i].first;
                    const DWORD offset = axisUsageMap[i].second;

                    DIDEVICEOBJECTINSTANCEW didObjectInstance;
                    didObjectInstance.dwSize = sizeof(didObjectInstance);
                    hr = device->GetObjectInfo(&didObjectInstance, axisUsageMap[i].second, DIPH_BYOFFSET);

                    if (SUCCEEDED(hr) &&
                        didObjectInstance.wUsage == usage &&
                        (didObjectInstance.dwType & DIDFT_AXIS))
                    {
                        Axis axis;
                        axis.axis = gamepadConfig.axisMap[i];

                        DIPROPDWORD propertyDeadZone;
                        propertyDeadZone.diph.dwSize = sizeof(propertyDeadZone);
                        propertyDeadZone.diph.dwHeaderSize = sizeof(propertyDeadZone.diph);
                        propertyDeadZone.diph.dwObj = offset;
                        propertyDeadZone.diph.dwHow = DIPH_BYOFFSET;
                        propertyDeadZone.dwData = 0;

                        // Set the range for the axis
                        if (FAILED(hr = device->SetProperty(DIPROP_DEADZONE, &propertyDeadZone.diph)))
                            engine->log(Log::Level::Warning) << "Failed to set DirectInput device dead zone property, error: " << hr;

                        DIPROPRANGE propertyAxisRange;
                        propertyAxisRange.diph.dwSize = sizeof(propertyAxisRange);
                        propertyAxisRange.diph.dwHeaderSize = sizeof(propertyAxisRange.diph);
                        propertyAxisRange.diph.dwObj = offset;
                        propertyAxisRange.diph.dwHow = DIPH_BYOFFSET;

                        if (FAILED(hr = device->GetProperty(DIPROP_RANGE, &propertyAxisRange.diph)))
                            throw std::system_error(hr, getErrorCategory(), "Failed to get DirectInput device axis range property");

                        axis.min = propertyAxisRange.lMin;
                        axis.max = propertyAxisRange.lMax;
                        axis.range = propertyAxisRange.lMax - propertyAxisRange.lMin;

                        switch (gamepadConfig.axisMap[i])
                        {
                            case Gamepad::Axis::Unknown:
                                break;
                            case Gamepad::Axis::LeftThumbX:
                                axis.negativeButton = Gamepad::Button::LeftThumbLeft;
                                axis.positiveButton = Gamepad::Button::LeftThumbRight;
                                break;
                            case Gamepad::Axis::LeftThumbY:
                                axis.negativeButton = Gamepad::Button::LeftThumbUp;
                                axis.positiveButton = Gamepad::Button::LeftThumbDown;
                                break;
                            case Gamepad::Axis::RightThumbX:
                                axis.negativeButton = Gamepad::Button::RightThumbLeft;
                                axis.positiveButton = Gamepad::Button::RightThumbRight;
                                break;
                            case Gamepad::Axis::RightThumbY:
                                axis.negativeButton = Gamepad::Button::RightThumbUp;
                                axis.positiveButton = Gamepad::Button::RightThumbDown;
                                break;
                            case Gamepad::Axis::LeftTrigger:
                                axis.negativeButton = Gamepad::Button::LeftTrigger;
                                axis.positiveButton = Gamepad::Button::LeftTrigger;
                                hasLeftTrigger = true;
                                break;
                            case Gamepad::Axis::RightTrigger:
                                axis.negativeButton = Gamepad::Button::RightTrigger;
                                axis.positiveButton = Gamepad::Button::RightTrigger;
                                hasRightTrigger = true;
                                break;
                        }

                        axes.insert(std::make_pair(offset, axis));
                    }
                }
            }

            DIDEVCAPS capabilities;
            capabilities.dwSize = sizeof(capabilities);
            if (FAILED(hr = device->GetCapabilities(&capabilities)))
                throw std::system_error(hr, getErrorCategory(), "Failed to get DirectInput device capabilities");

            if (capabilities.dwFlags & DIDC_FORCEFEEDBACK)
            {
                if (FAILED(hr = device->Acquire()))
                    throw std::system_error(hr, getErrorCategory(), "Failed to acquire DirectInput device");

                if (FAILED(hr = device->SendForceFeedbackCommand(DISFFC_RESET)))
                    throw std::system_error(hr, getErrorCategory(), "Failed to set DirectInput device force feedback command");

                if (FAILED(hr = device->Unacquire()))
                    throw std::system_error(hr, getErrorCategory(), "Failed to unacquire DirectInput device");

                DIPROPDWORD propertyAutoCenter;
                propertyAutoCenter.diph.dwSize = sizeof(propertyAutoCenter);
                propertyAutoCenter.diph.dwHeaderSize = sizeof(propertyAutoCenter.diph);
                propertyAutoCenter.diph.dwHow = DIPH_DEVICE;
                propertyAutoCenter.diph.dwObj = 0;
                propertyAutoCenter.dwData = DIPROPAUTOCENTER_ON;

                if (FAILED(hr = device->SetProperty(DIPROP_AUTOCENTER, &propertyAutoCenter.diph)))
                    engine->log(Log::Level::Warning) << "Failed to set DirectInput device autocenter property, error: " << hr;
            }

            DIPROPDWORD propertyBufferSize;
            propertyBufferSize.diph.dwSize = sizeof(propertyBufferSize);
            propertyBufferSize.diph.dwHeaderSize = sizeof(propertyBufferSize.diph);
            propertyBufferSize.diph.dwHow = DIPH_DEVICE;
            propertyBufferSize.diph.dwObj = 0;
            propertyBufferSize.dwData = INPUT_QUEUE_SIZE;

            if (FAILED(hr = device->SetProperty(DIPROP_BUFFERSIZE, &propertyBufferSize.diph)))
                throw std::system_error(hr, getErrorCategory(), "Failed to set DirectInput device buffer size property");

            buffered = (hr != DI_POLLEDDEVICE);
        }

        GamepadDeviceDI::~GamepadDeviceDI()
        {
            if (device)
            {
                device->Unacquire();
                device->Release();
            }
        }

        void GamepadDeviceDI::update()
        {
            HRESULT hr = device->Poll();

            if (hr == DIERR_NOTACQUIRED)
            {
                if (FAILED(hr = device->Acquire()))
                    throw std::system_error(hr, getErrorCategory(), "Failed to acquire DirectInput device");

                if (FAILED(hr = device->Poll()))
                    throw std::system_error(hr, getErrorCategory(), "Failed to poll DirectInput device");
            }

            return buffered ? checkInputBuffered() : checkInputPolled();
        }

        void GamepadDeviceDI::checkInputBuffered()
        {
            DWORD eventCount = INPUT_QUEUE_SIZE;
            DIDEVICEOBJECTDATA events[INPUT_QUEUE_SIZE];

            HRESULT hr = device->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), events, &eventCount, 0);

            if (hr == DIERR_NOTACQUIRED)
            {
                if (FAILED(hr = device->Acquire()))
                    throw std::system_error(hr, getErrorCategory(), "Failed to acquire DirectInput device");

                hr = device->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), events, &eventCount, 0);
            }

            if (FAILED(hr))
                throw std::system_error(hr, getErrorCategory(), "Failed to get DirectInput device state");

            for (DWORD e = 0; e < eventCount; ++e)
            {
                if (events[e].dwOfs == DIJOFS_POV(0))
                {
                    const std::uint32_t oldHatValue = getHatValue(hatValue);
                    const std::uint32_t newHatValue = getHatValue(events[e].dwData);

                    const std::uint32_t oldBitmask = (oldHatValue >= 8) ? 0 : (1 << (oldHatValue / 2)) | // first bit
                        (1 << (oldHatValue / 2 + oldHatValue % 2)) % 4; // second bit

                    const std::uint32_t newBitmask = (newHatValue >= 8) ? 0 : (1 << (newHatValue / 2)) | // first bit
                        (1 << (newHatValue / 2 + newHatValue % 2)) % 4; // second bit

                    if ((oldBitmask & 0x01) != (newBitmask & 0x01))
                        handleButtonValueChange(Gamepad::Button::DpadUp,
                                                (newBitmask & 0x01) > 0,
                                                (newBitmask & 0x01) > 0 ? 1.0F : 0.0F);
                    if ((oldBitmask & 0x02) != (newBitmask & 0x02))
                        handleButtonValueChange(Gamepad::Button::DpadRight,
                                                (newBitmask & 0x02) > 0,
                                                (newBitmask & 0x02) > 0 ? 1.0F : 0.0F);
                    if ((oldBitmask & 0x04) != (newBitmask & 0x04))
                        handleButtonValueChange(Gamepad::Button::DpadDown,
                                                (newBitmask & 0x04) > 0,
                                                (newBitmask & 0x04) > 0 ? 1.0F : 0.0F);
                    if ((oldBitmask & 0x08) != (newBitmask & 0x08))
                        handleButtonValueChange(Gamepad::Button::DpadLeft,
                                                (newBitmask & 0x08) > 0,
                                                (newBitmask & 0x08) > 0 ? 1.0F : 0.0F);

                    hatValue = events[e].dwData;
                }

                auto buttonIterator = buttons.find(events[e].dwOfs);

                if (buttonIterator != buttons.end())
                {
                    Button& button = buttonIterator->second;

                    if ((button.button != Gamepad::Button::LeftTrigger || !hasLeftTrigger) &&
                        (button.button != Gamepad::Button::RightTrigger || !hasRightTrigger))
                    {
                        handleButtonValueChange(button.button,
                                                events[e].dwData > 0,
                                                (events[e].dwData > 0) ? 1.0F : 0.0F);
                    }

                    button.value = static_cast<BYTE>(events[e].dwData);
                }

                auto axisIterator = axes.find(events[e].dwOfs);

                if (axisIterator != axes.end())
                {
                    Axis& axis = axisIterator->second;

                    handleAxisChange(axis.value,
                                     events[e].dwData,
                                     axis.min, axis.range,
                                     axis.negativeButton, axis.positiveButton);

                    axis.value = events[e].dwData;
                }
            }
        }

        void GamepadDeviceDI::checkInputPolled()
        {
            DIJOYSTATE newDIState;

            HRESULT hr = device->GetDeviceState(sizeof(newDIState), &newDIState);

            if (hr == DIERR_NOTACQUIRED)
            {
                if (FAILED(hr = device->Acquire()))
                    throw std::system_error(hr, getErrorCategory(), "Failed to acquire DirectInput device");

                hr = device->GetDeviceState(sizeof(newDIState), &newDIState);
            }

            if (FAILED(hr))
                throw std::system_error(hr, getErrorCategory(), "Failed to get DirectInput device state");

            if (hatValue != newDIState.rgdwPOV[0])
            {
                const std::uint32_t oldHatValue = getHatValue(hatValue);
                const std::uint32_t newHatValue = getHatValue(newDIState.rgdwPOV[0]);

                const std::uint32_t oldBitmask = (oldHatValue >= 8) ? 0 : (1 << (oldHatValue / 2)) | // first bit
                    (1 << (oldHatValue / 2 + oldHatValue % 2)) % 4; // second bit

                const std::uint32_t newBitmask = (newHatValue >= 8) ? 0 : (1 << (newHatValue / 2)) | // first bit
                    (1 << (newHatValue / 2 + newHatValue % 2)) % 4; // second bit

                if ((oldBitmask & 0x01) != (newBitmask & 0x01))
                    handleButtonValueChange(Gamepad::Button::DpadUp,
                                            (newBitmask & 0x01) > 0,
                                            (newBitmask & 0x01) > 0 ? 1.0F : 0.0F);
                if ((oldBitmask & 0x02) != (newBitmask & 0x02))
                    handleButtonValueChange(Gamepad::Button::DpadRight,
                                            (newBitmask & 0x02) > 0,
                                            (newBitmask & 0x02) > 0 ? 1.0F : 0.0F);
                if ((oldBitmask & 0x04) != (newBitmask & 0x04))
                    handleButtonValueChange(Gamepad::Button::DpadDown,
                                            (newBitmask & 0x04) > 0,
                                            (newBitmask & 0x04) > 0 ? 1.0F : 0.0F);
                if ((oldBitmask & 0x08) != (newBitmask & 0x08))
                    handleButtonValueChange(Gamepad::Button::DpadLeft,
                                            (newBitmask & 0x08) > 0,
                                            (newBitmask & 0x08) > 0 ? 1.0F : 0.0F);

                hatValue = newDIState.rgdwPOV[0];
            }

            for (auto& buttonPair : buttons)
            {
                const DWORD offset = buttonPair.first;
                Button& button = buttonPair.second;
                const BYTE newValue = *reinterpret_cast<const BYTE*>(reinterpret_cast<const std::uint8_t*>(&newDIState) + offset);

                if (button.value != newValue)
                {
                    if ((button.button != Gamepad::Button::LeftTrigger || !hasLeftTrigger) &&
                        (button.button != Gamepad::Button::RightTrigger || !hasRightTrigger))
                    {
                        handleButtonValueChange(button.button,
                                                newValue > 0,
                                                (newValue > 0) ? 1.0F : 0.0F);
                    }

                    button.value = newValue;
                }
            }

            for (auto& axisPair : axes)
            {
                const DWORD offset = axisPair.first;
                Axis& axis = axisPair.second;
                const LONG newValue = *reinterpret_cast<const LONG*>(reinterpret_cast<const std::uint8_t*>(&newDIState) + offset);

                if (axis.value != newValue)
                {
                    handleAxisChange(axis.value,
                                     newValue,
                                     axis.min, axis.range,
                                     axis.negativeButton, axis.positiveButton);

                    axis.value = newValue;
                }
            }
        }

        void GamepadDeviceDI::handleAxisChange(LONG oldValue, LONG newValue,
                                               LONG min, LONG range,
                                               Gamepad::Button negativeButton, Gamepad::Button positiveButton)
        {
            if (negativeButton == positiveButton)
            {
                const auto floatValue = static_cast<float>(newValue - min) / range;

                handleButtonValueChange(negativeButton,
                                        floatValue > 0.0F,
                                        floatValue);
            }
            else
            {
                const auto floatValue = 2.0F * static_cast<float>(newValue - min) / range - 1.0F;

                if (floatValue > 0.0F)
                {
                    handleButtonValueChange(positiveButton,
                                            floatValue > THUMB_DEADZONE,
                                            floatValue);
                }
                else if (floatValue < 0.0F)
                {
                    handleButtonValueChange(negativeButton,
                                            -floatValue > THUMB_DEADZONE,
                                            -floatValue);
                }
                else // thumbstick is 0
                {
                    if (oldValue > newValue)
                        handleButtonValueChange(positiveButton, false, 0.0F);
                    else
                        handleButtonValueChange(negativeButton, false, 0.0F);
                }
            }
        }
    } // namespace input
} // namespace ouzel
