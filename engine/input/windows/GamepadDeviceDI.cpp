// Ouzel by Elviss Strazdins

#include <array>
#include "GamepadDeviceDI.hpp"
#include "InputSystemWin.hpp"
#include "DIErrorCategory.hpp"
#include "../GamepadConfig.hpp"
#include "../../core/windows/NativeWindowWin.hpp"
#include "../../utils/Log.hpp"

namespace ouzel::input::windows
{
    namespace
    {
        constexpr float thumbDeadzone = 0.2F;
        constexpr std::size_t inputQueueSize = 32U;

        // converts the angle to the hat value
        constexpr std::uint32_t getHatValue(std::uint32_t value) noexcept
        {
            return (value == 0xFFFFFFFFU) ?
                8 : ((value + 4500 / 2) % 36000) / 4500; // round up
        }
    }

    GamepadDeviceDI::GamepadDeviceDI(InputSystem& initInputSystem,
                                     DeviceId initId,
                                     const DIDEVICEINSTANCEW* initInstance,
                                     IDirectInput8W* directInput,
                                     HWND window):
        GamepadDevice{initInputSystem, initId},
        instance{initInstance}
    {
        const std::int32_t vendorId = LOWORD(instance->guidProduct.Data1);
        const std::int32_t productId = HIWORD(instance->guidProduct.Data1);

        if (const auto bufferSize = WideCharToMultiByte(CP_UTF8, 0, instance->tszProductName, -1, nullptr, 0, nullptr, nullptr); bufferSize != 0)
        {
            auto buffer = std::make_unique<char[]>(bufferSize);
            if (WideCharToMultiByte(CP_UTF8, 0, instance->tszProductName, -1, buffer.get(), bufferSize, nullptr, nullptr) != 0)
                name = buffer.get();
        }

        if (const auto result = directInput->CreateDevice(instance->guidInstance, &device, nullptr); FAILED(result))
            throw std::system_error{result, getErrorCategory(), "Failed to create DirectInput device"};

        // Exclusive access is needed for force feedback
        if (const auto result = device->SetCooperativeLevel(window, DISCL_BACKGROUND | DISCL_EXCLUSIVE); FAILED(result))
            throw std::system_error{result, getErrorCategory(), "Failed to set DirectInput device format"};

        if (const auto result = device->SetDataFormat(&c_dfDIJoystick); FAILED(result))
            throw std::system_error{result, getErrorCategory(), "Failed to set DirectInput device format"};

        const auto& gamepadConfig = getGamepadConfig(vendorId, productId);

        for (std::size_t i = 0; i < 24; ++i)
        {
            if (gamepadConfig.buttonMap[i] != Gamepad::Button::none)
            {
                Button button;
                button.button = gamepadConfig.buttonMap[i];
                buttons.insert(std::pair(static_cast<DWORD>(DIJOFS_BUTTON(i)), button));
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
            if (gamepadConfig.axisMap[i] != Gamepad::Axis::none)
            {
                const auto [usage, offset] = axisUsageMap[i];

                DIDEVICEOBJECTINSTANCEW didObjectInstance;
                didObjectInstance.dwSize = sizeof(didObjectInstance);

                if (SUCCEEDED(device->GetObjectInfo(&didObjectInstance, offset, DIPH_BYOFFSET)) &&
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
                    if (const auto result = device->SetProperty(DIPROP_DEADZONE, &propertyDeadZone.diph); FAILED(result))
                        log(Log::Level::warning) << "Failed to set DirectInput device dead zone property, error: " << result;

                    DIPROPRANGE propertyAxisRange;
                    propertyAxisRange.diph.dwSize = sizeof(propertyAxisRange);
                    propertyAxisRange.diph.dwHeaderSize = sizeof(propertyAxisRange.diph);
                    propertyAxisRange.diph.dwObj = offset;
                    propertyAxisRange.diph.dwHow = DIPH_BYOFFSET;

                    if (const auto result = device->GetProperty(DIPROP_RANGE, &propertyAxisRange.diph); FAILED(result))
                        throw std::system_error{result, getErrorCategory(), "Failed to get DirectInput device axis range property"};

                    axis.min = propertyAxisRange.lMin;
                    axis.max = propertyAxisRange.lMax;
                    axis.range = propertyAxisRange.lMax - propertyAxisRange.lMin;

                    switch (gamepadConfig.axisMap[i])
                    {
                        case Gamepad::Axis::none:
                            break;
                        case Gamepad::Axis::leftThumbX:
                            axis.negativeButton = Gamepad::Button::leftThumbLeft;
                            axis.positiveButton = Gamepad::Button::leftThumbRight;
                            break;
                        case Gamepad::Axis::leftThumbY:
                            axis.negativeButton = Gamepad::Button::leftThumbUp;
                            axis.positiveButton = Gamepad::Button::leftThumbDown;
                            break;
                        case Gamepad::Axis::rightThumbX:
                            axis.negativeButton = Gamepad::Button::rightThumbLeft;
                            axis.positiveButton = Gamepad::Button::rightThumbRight;
                            break;
                        case Gamepad::Axis::rightThumbY:
                            axis.negativeButton = Gamepad::Button::rightThumbUp;
                            axis.positiveButton = Gamepad::Button::rightThumbDown;
                            break;
                        case Gamepad::Axis::leftTrigger:
                            axis.negativeButton = Gamepad::Button::leftTrigger;
                            axis.positiveButton = Gamepad::Button::leftTrigger;
                            hasLeftTrigger = true;
                            break;
                        case Gamepad::Axis::rightTrigger:
                            axis.negativeButton = Gamepad::Button::rightTrigger;
                            axis.positiveButton = Gamepad::Button::rightTrigger;
                            hasRightTrigger = true;
                            break;
                    }

                    axes.insert(std::pair(offset, axis));
                }
            }

        DIDEVCAPS capabilities;
        capabilities.dwSize = sizeof(capabilities);
        if (const auto result = device->GetCapabilities(&capabilities); FAILED(result))
            throw std::system_error{result, getErrorCategory(), "Failed to get DirectInput device capabilities"};

        if (capabilities.dwFlags & DIDC_FORCEFEEDBACK)
        {
            if (const auto result = device->Acquire(); FAILED(result))
                throw std::system_error{result, getErrorCategory(), "Failed to acquire DirectInput device"};

            if (const auto result = device->SendForceFeedbackCommand(DISFFC_RESET); FAILED(result))
                throw std::system_error{result, getErrorCategory(), "Failed to set DirectInput device force feedback command"};

            if (const auto result = device->Unacquire(); FAILED(result))
                throw std::system_error{result, getErrorCategory(), "Failed to unacquire DirectInput device"};

            DIPROPDWORD propertyAutoCenter;
            propertyAutoCenter.diph.dwSize = sizeof(propertyAutoCenter);
            propertyAutoCenter.diph.dwHeaderSize = sizeof(propertyAutoCenter.diph);
            propertyAutoCenter.diph.dwHow = DIPH_DEVICE;
            propertyAutoCenter.diph.dwObj = 0;
            propertyAutoCenter.dwData = DIPROPAUTOCENTER_ON;

            if (const auto result = device->SetProperty(DIPROP_AUTOCENTER, &propertyAutoCenter.diph); FAILED(result))
                log(Log::Level::warning) << "Failed to set DirectInput device autocenter property, error: " << result;
        }

        DIPROPDWORD propertyBufferSize;
        propertyBufferSize.diph.dwSize = sizeof(propertyBufferSize);
        propertyBufferSize.diph.dwHeaderSize = sizeof(propertyBufferSize.diph);
        propertyBufferSize.diph.dwHow = DIPH_DEVICE;
        propertyBufferSize.diph.dwObj = 0;
        propertyBufferSize.dwData = inputQueueSize;

        const auto result = device->SetProperty(DIPROP_BUFFERSIZE, &propertyBufferSize.diph);
        if (FAILED(result))
            throw std::system_error{result, getErrorCategory(), "Failed to set DirectInput device buffer size property"};

        buffered = (result != DI_POLLEDDEVICE);
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
        if (const auto result = device->Poll(); result == DIERR_NOTACQUIRED)
        {
            if (const auto acquireResult = device->Acquire(); FAILED(acquireResult))
                throw std::system_error{acquireResult, getErrorCategory(), "Failed to acquire DirectInput device"};

            if (const auto pollResult = device->Poll(); FAILED(pollResult))
                throw std::system_error{pollResult, getErrorCategory(), "Failed to poll DirectInput device"};
        }

        return buffered ? checkInputBuffered() : checkInputPolled();
    }

    void GamepadDeviceDI::checkInputBuffered()
    {
        DWORD eventCount = inputQueueSize;
        DIDEVICEOBJECTDATA events[inputQueueSize];

        auto result = device->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), events, &eventCount, 0);

        if (result == DIERR_NOTACQUIRED)
        {
            if (const auto acquireResult = device->Acquire(); FAILED(acquireResult))
                throw std::system_error{acquireResult, getErrorCategory(), "Failed to acquire DirectInput device"};

            result = device->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), events, &eventCount, 0);
        }

        if (FAILED(result))
            throw std::system_error{result, getErrorCategory(), "Failed to get DirectInput device state"};

        for (DWORD e = 0; e < eventCount; ++e)
        {
            if (events[e].dwOfs == DIJOFS_POV(0))
            {
                const auto oldHatValue = getHatValue(hatValue);
                const auto newHatValue = getHatValue(events[e].dwData);

                const std::uint32_t oldBitmask = (oldHatValue >= 8) ? 0 : (1 << (oldHatValue / 2)) | // first bit
                    (1 << (oldHatValue / 2 + oldHatValue % 2)) % 4; // second bit

                const std::uint32_t newBitmask = (newHatValue >= 8) ? 0 : (1 << (newHatValue / 2)) | // first bit
                    (1 << (newHatValue / 2 + newHatValue % 2)) % 4; // second bit

                if ((oldBitmask & 0x01) != (newBitmask & 0x01))
                    handleButtonValueChange(Gamepad::Button::dPadUp,
                                            (newBitmask & 0x01) > 0,
                                            (newBitmask & 0x01) > 0 ? 1.0F : 0.0F);
                if ((oldBitmask & 0x02) != (newBitmask & 0x02))
                    handleButtonValueChange(Gamepad::Button::dPadRight,
                                            (newBitmask & 0x02) > 0,
                                            (newBitmask & 0x02) > 0 ? 1.0F : 0.0F);
                if ((oldBitmask & 0x04) != (newBitmask & 0x04))
                    handleButtonValueChange(Gamepad::Button::dPadDown,
                                            (newBitmask & 0x04) > 0,
                                            (newBitmask & 0x04) > 0 ? 1.0F : 0.0F);
                if ((oldBitmask & 0x08) != (newBitmask & 0x08))
                    handleButtonValueChange(Gamepad::Button::dPadLeft,
                                            (newBitmask & 0x08) > 0,
                                            (newBitmask & 0x08) > 0 ? 1.0F : 0.0F);

                hatValue = events[e].dwData;
            }

            if (const auto buttonIterator = buttons.find(events[e].dwOfs); buttonIterator != buttons.end())
            {
                Button& button = buttonIterator->second;

                if ((button.button != Gamepad::Button::leftTrigger || !hasLeftTrigger) &&
                    (button.button != Gamepad::Button::rightTrigger || !hasRightTrigger))
                    handleButtonValueChange(button.button,
                                            events[e].dwData > 0,
                                            (events[e].dwData > 0) ? 1.0F : 0.0F);

                button.value = static_cast<BYTE>(events[e].dwData);
            }

            if (const auto axisIterator = axes.find(events[e].dwOfs); axisIterator != axes.end())
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

        auto result = device->GetDeviceState(sizeof(newDIState), &newDIState);

        if (result == DIERR_NOTACQUIRED)
        {
            if (const auto acquireResult = device->Acquire(); FAILED(acquireResult))
                throw std::system_error{acquireResult, getErrorCategory(), "Failed to acquire DirectInput device"};

            result = device->GetDeviceState(sizeof(newDIState), &newDIState);
        }

        if (FAILED(result))
            throw std::system_error{result, getErrorCategory(), "Failed to get DirectInput device state"};

        if (hatValue != newDIState.rgdwPOV[0])
        {
            const auto oldHatValue = getHatValue(hatValue);
            const auto newHatValue = getHatValue(newDIState.rgdwPOV[0]);

            const std::uint32_t oldBitmask = (oldHatValue >= 8) ? 0 : (1 << (oldHatValue / 2)) | // first bit
                (1 << (oldHatValue / 2 + oldHatValue % 2)) % 4; // second bit

            const std::uint32_t newBitmask = (newHatValue >= 8) ? 0 : (1 << (newHatValue / 2)) | // first bit
                (1 << (newHatValue / 2 + newHatValue % 2)) % 4; // second bit

            if ((oldBitmask & 0x01) != (newBitmask & 0x01))
                handleButtonValueChange(Gamepad::Button::dPadUp,
                                        (newBitmask & 0x01) > 0,
                                        (newBitmask & 0x01) > 0 ? 1.0F : 0.0F);
            if ((oldBitmask & 0x02) != (newBitmask & 0x02))
                handleButtonValueChange(Gamepad::Button::dPadRight,
                                        (newBitmask & 0x02) > 0,
                                        (newBitmask & 0x02) > 0 ? 1.0F : 0.0F);
            if ((oldBitmask & 0x04) != (newBitmask & 0x04))
                handleButtonValueChange(Gamepad::Button::dPadDown,
                                        (newBitmask & 0x04) > 0,
                                        (newBitmask & 0x04) > 0 ? 1.0F : 0.0F);
            if ((oldBitmask & 0x08) != (newBitmask & 0x08))
                handleButtonValueChange(Gamepad::Button::dPadLeft,
                                        (newBitmask & 0x08) > 0,
                                        (newBitmask & 0x08) > 0 ? 1.0F : 0.0F);

            hatValue = newDIState.rgdwPOV[0];
        }

        for (auto& [offset, button] : buttons)
        {
            const auto newValue = *reinterpret_cast<const BYTE*>(reinterpret_cast<const std::uint8_t*>(&newDIState) + offset);

            if (button.value != newValue)
            {
                if ((button.button != Gamepad::Button::leftTrigger || !hasLeftTrigger) &&
                    (button.button != Gamepad::Button::rightTrigger || !hasRightTrigger))
                    handleButtonValueChange(button.button,
                                            newValue > 0,
                                            (newValue > 0) ? 1.0F : 0.0F);

                button.value = newValue;
            }
        }

        for (auto& [offset, axis] : axes)
        {
            const auto newValue = *reinterpret_cast<const LONG*>(reinterpret_cast<const std::uint8_t*>(&newDIState) + offset);

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
                                        floatValue > thumbDeadzone,
                                        floatValue);
            }
            else if (floatValue < 0.0F)
            {
                handleButtonValueChange(negativeButton,
                                        -floatValue > thumbDeadzone,
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
}
