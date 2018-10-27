// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "GamepadDeviceDI.hpp"
#include "core/windows/NativeWindowWin.hpp"
#include "input/GamepadConfig.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

static const float THUMB_DEADZONE = 0.2F;
static const size_t INPUT_QUEUE_SIZE = 32;

namespace ouzel
{
    namespace input
    {
        GamepadDeviceDI::GamepadDeviceDI(InputSystem& initInputSystem,
                                         uint32_t initId,
                                         const DIDEVICEINSTANCEW* initInstance, IDirectInput8W* directInput, HWND window):
            GamepadDeviceWin(initInputSystem, initId),
            instance(initInstance)
        {
            int32_t vendorId = LOWORD(instance->guidProduct.Data1);
            int32_t productId = HIWORD(instance->guidProduct.Data1);

            int bufferSize = WideCharToMultiByte(CP_UTF8, 0, instance->tszProductName, -1, nullptr, 0, nullptr, nullptr);
            if (bufferSize != 0)
            {
                std::vector<char> buffer(bufferSize);
                if (WideCharToMultiByte(CP_UTF8, 0, instance->tszProductName, -1, buffer.data(), bufferSize, nullptr, nullptr) != 0)
                    name = buffer.data();
            }

            HRESULT hr = directInput->CreateDevice(instance->guidInstance, &device, nullptr);
            if (FAILED(hr))
                throw SystemError("Failed to create DirectInput device, error: " + std::to_string(hr));

            // Exclusive access is needed for force feedback
            hr = device->SetCooperativeLevel(window,
                DISCL_BACKGROUND | DISCL_EXCLUSIVE);
            if (FAILED(hr))
                throw SystemError("Failed to set DirectInput device format, error: " + std::to_string(hr));

            hr = device->SetDataFormat(&c_dfDIJoystick);
            if (FAILED(hr))
                throw SystemError("Failed to set DirectInput device format, error: " + std::to_string(hr));

            const GamepadConfig& gamepadConfig = getGamepadConfig(vendorId, productId);

            for (size_t i = 0; i < 24; ++i)
            {
                if (gamepadConfig.buttonMap[i] != Gamepad::Button::NONE)
                {
                    Button button;
                    button.button = gamepadConfig.buttonMap[i];
                    buttons.insert(std::make_pair(static_cast<DWORD>(DIJOFS_BUTTON(i)), button));
                }
            }

            static const std::array<std::pair<USAGE, DWORD>, 6> axisUsageMap = {
                std::make_pair(HID_USAGE_GENERIC_X, DIJOFS_X),
                std::make_pair(HID_USAGE_GENERIC_Y, DIJOFS_Y),
                std::make_pair(HID_USAGE_GENERIC_Z, DIJOFS_Z),
                std::make_pair(HID_USAGE_GENERIC_RX, DIJOFS_RX),
                std::make_pair(HID_USAGE_GENERIC_RY, DIJOFS_RY),
                std::make_pair(HID_USAGE_GENERIC_RZ, DIJOFS_RZ)
            };

            for (size_t i = 0; i < 6; ++i)
            {
                if (gamepadConfig.axisMap[i] != Gamepad::Axis::NONE)
                {
                    USAGE usage = axisUsageMap[i].first;
                    DWORD offset = axisUsageMap[i].second;

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
                        hr = device->SetProperty(DIPROP_DEADZONE, &propertyDeadZone.diph);
                        if (FAILED(hr))
                            Log(Log::Level::WARN) << "Failed to set DirectInput device dead zone property, error: " << hr;

                        DIPROPRANGE propertyAxisRange;
                        propertyAxisRange.diph.dwSize = sizeof(propertyAxisRange);
                        propertyAxisRange.diph.dwHeaderSize = sizeof(propertyAxisRange.diph);
                        propertyAxisRange.diph.dwObj = offset;
                        propertyAxisRange.diph.dwHow = DIPH_BYOFFSET;

                        hr = device->GetProperty(DIPROP_RANGE, &propertyAxisRange.diph);
                        if (FAILED(hr))
                            throw SystemError("Failed to get DirectInput device axis range property, error: " + std::to_string(hr));

                        axis.min = propertyAxisRange.lMin;
                        axis.max = propertyAxisRange.lMax;
                        axis.range = propertyAxisRange.lMax - propertyAxisRange.lMin;

                        switch (gamepadConfig.axisMap[i])
                        {
                            case Gamepad::Axis::NONE:
                                break;
                            case Gamepad::Axis::LEFT_THUMB_X:
                                axis.negativeButton = Gamepad::Button::LEFT_THUMB_LEFT;
                                axis.positiveButton = Gamepad::Button::LEFT_THUMB_RIGHT;
                                break;
                            case Gamepad::Axis::LEFT_THUMB_Y:
                                axis.negativeButton = Gamepad::Button::LEFT_THUMB_UP;
                                axis.positiveButton = Gamepad::Button::LEFT_THUMB_DOWN;
                                break;
                            case Gamepad::Axis::RIGHT_THUMB_X:
                                axis.negativeButton = Gamepad::Button::RIGHT_THUMB_LEFT;
                                axis.positiveButton = Gamepad::Button::RIGHT_THUMB_RIGHT;
                                break;
                            case Gamepad::Axis::RIGHT_THUMB_Y:
                                axis.negativeButton = Gamepad::Button::RIGHT_THUMB_UP;
                                axis.positiveButton = Gamepad::Button::RIGHT_THUMB_DOWN;
                                break;
                            case Gamepad::Axis::LEFT_TRIGGER:
                                axis.negativeButton = Gamepad::Button::LEFT_TRIGGER;
                                axis.positiveButton = Gamepad::Button::LEFT_TRIGGER;
                                hasLeftTrigger = true;
                                break;
                            case Gamepad::Axis::RIGHT_TRIGGER:
                                axis.negativeButton = Gamepad::Button::RIGHT_TRIGGER;
                                axis.positiveButton = Gamepad::Button::RIGHT_TRIGGER;
                                hasRightTrigger = true;
                                break;
                        }

                        axes.insert(std::make_pair(offset, axis));
                    }
                }
            }

            DIDEVCAPS capabilities;
            capabilities.dwSize = sizeof(capabilities);
            hr = device->GetCapabilities(&capabilities);
            if (FAILED(hr))
                throw SystemError("Failed to get DirectInput device capabilities, error: " + std::to_string(hr));

            if (capabilities.dwFlags & DIDC_FORCEFEEDBACK)
            {
                hr = device->Acquire();
                if (FAILED(hr))
                    throw SystemError("Failed to acquire DirectInput device, error: " + std::to_string(hr));

                hr = device->SendForceFeedbackCommand(DISFFC_RESET);
                if (FAILED(hr))
                    throw SystemError("Failed to set DirectInput device force feedback command, error: " + std::to_string(hr));

                hr = device->Unacquire();
                if (FAILED(hr))
                    throw SystemError("Failed to unacquire DirectInput device, error: " + std::to_string(hr));

                DIPROPDWORD propertyAutoCenter;
                propertyAutoCenter.diph.dwSize = sizeof(propertyAutoCenter);
                propertyAutoCenter.diph.dwHeaderSize = sizeof(propertyAutoCenter.diph);
                propertyAutoCenter.diph.dwHow = DIPH_DEVICE;
                propertyAutoCenter.diph.dwObj = 0;
                propertyAutoCenter.dwData = DIPROPAUTOCENTER_ON;

                hr = device->SetProperty(DIPROP_AUTOCENTER, &propertyAutoCenter.diph);
                if (FAILED(hr))
                    Log(Log::Level::WARN) << "Failed to set DirectInput device autocenter property, error: " << hr;
            }

            DIPROPDWORD propertyBufferSize;
            propertyBufferSize.diph.dwSize = sizeof(propertyBufferSize);
            propertyBufferSize.diph.dwHeaderSize = sizeof(propertyBufferSize.diph);
            propertyBufferSize.diph.dwHow = DIPH_DEVICE;
            propertyBufferSize.diph.dwObj = 0;
            propertyBufferSize.dwData = INPUT_QUEUE_SIZE;

            hr = device->SetProperty(DIPROP_BUFFERSIZE, &propertyBufferSize.diph);

            if (hr == DI_POLLEDDEVICE)
                buffered = false;
            else if (FAILED(hr))
                throw SystemError("Failed to set DirectInput device buffer size property, error: " + std::to_string(hr));
            else
                buffered = true;
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
            HRESULT result = device->Poll();

            if (result == DIERR_NOTACQUIRED)
            {
                HRESULT hr = device->Acquire();
                if (FAILED(hr))
                    throw SystemError("Failed to acquire DirectInput device, error: " + std::to_string(hr));

                result = device->Poll();
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
                hr = device->Acquire();
                if (FAILED(hr))
                    throw SystemError("Failed to acquire DirectInput device, error: " + std::to_string(hr));

                hr = device->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), events, &eventCount, 0);
            }

            if (FAILED(hr))
                throw SystemError("Failed to get DirectInput device state, error: " + std::to_string(hr));

            for (DWORD e = 0; e < eventCount; ++e)
            {
                if (events[e].dwOfs == DIJOFS_POV(0))
                {
                    uint32_t oldHatValue = hatValue;
                    if (oldHatValue == 0xffffffff)
                        oldHatValue = 8;
                    else
                    {
                        // round up
                        oldHatValue += 4500 / 2;
                        oldHatValue %= 36000;
                        oldHatValue /= 4500;
                    }

                    uint32_t newHatValue = events[e].dwData;
                    if (newHatValue == 0xffffffff)
                        newHatValue = 8;
                    else
                    {
                        // round up
                        newHatValue += 4500 / 2;
                        newHatValue %= 36000;
                        newHatValue /= 4500;
                    }

                    uint32_t oldBitmask = (oldHatValue >= 8) ? 0 : (1 << (oldHatValue / 2)) | // first bit
                        (1 << (oldHatValue / 2 + oldHatValue % 2)) % 4; // second bit

                    uint32_t newBitmask = (newHatValue >= 8) ? 0 : (1 << (newHatValue / 2)) | // first bit
                        (1 << (newHatValue / 2 + newHatValue % 2)) % 4; // second bit

                    if ((oldBitmask & 0x01) != (newBitmask & 0x01))
                        handleButtonValueChange(Gamepad::Button::DPAD_UP,
                                                (newBitmask & 0x01) > 0,
                                                (newBitmask & 0x01) > 0 ? 1.0F : 0.0F);
                    if ((oldBitmask & 0x02) != (newBitmask & 0x02))
                        handleButtonValueChange(Gamepad::Button::DPAD_RIGHT,
                                                (newBitmask & 0x02) > 0,
                                                (newBitmask & 0x02) > 0 ? 1.0F : 0.0F);
                    if ((oldBitmask & 0x04) != (newBitmask & 0x04))
                        handleButtonValueChange(Gamepad::Button::DPAD_DOWN,
                                                (newBitmask & 0x04) > 0,
                                                (newBitmask & 0x04) > 0 ? 1.0F : 0.0F);
                    if ((oldBitmask & 0x08) != (newBitmask & 0x08))
                        handleButtonValueChange(Gamepad::Button::DPAD_LEFT,
                                                (newBitmask & 0x08) > 0,
                                                (newBitmask & 0x08) > 0 ? 1.0F : 0.0F);

                    hatValue = events[e].dwData;
                }

                auto buttonIterator = buttons.find(events[e].dwOfs);

                if (buttonIterator != buttons.end())
                {
                    Button& button = buttonIterator->second;

                    if ((button.button != Gamepad::Button::LEFT_TRIGGER || !hasLeftTrigger) &&
                        (button.button != Gamepad::Button::RIGHT_TRIGGER || !hasRightTrigger))
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
                hr = device->Acquire();
                if (FAILED(hr))
                    throw SystemError("Failed to acquire DirectInput device, error: " + std::to_string(hr));

                hr = device->GetDeviceState(sizeof(newDIState), &newDIState);
            }

            if (FAILED(hr))
                throw SystemError("Failed to get DirectInput device state, error: " + std::to_string(hr));

            if (hatValue != newDIState.rgdwPOV[0])
            {
                uint32_t oldHatValue = hatValue;
                if (oldHatValue == 0xffffffff)
                    oldHatValue = 8;
                else
                {
                    // round up
                    oldHatValue += 4500 / 2;
                    oldHatValue %= 36000;
                    oldHatValue /= 4500;
                }

                uint32_t newHatValue = newDIState.rgdwPOV[0];
                if (newHatValue == 0xffffffff)
                    newHatValue = 8;
                else
                {
                    // round up
                    newHatValue += 4500 / 2;
                    newHatValue %= 36000;
                    newHatValue /= 4500;
                }

                uint32_t oldBitmask = (oldHatValue >= 8) ? 0 : (1 << (oldHatValue / 2)) | // first bit
                    (1 << (oldHatValue / 2 + oldHatValue % 2)) % 4; // second bit

                uint32_t newBitmask = (newHatValue >= 8) ? 0 : (1 << (newHatValue / 2)) | // first bit
                    (1 << (newHatValue / 2 + newHatValue % 2)) % 4; // second bit

                if ((oldBitmask & 0x01) != (newBitmask & 0x01))
                    handleButtonValueChange(Gamepad::Button::DPAD_UP,
                                            (newBitmask & 0x01) > 0,
                                            (newBitmask & 0x01) > 0 ? 1.0F : 0.0F);
                if ((oldBitmask & 0x02) != (newBitmask & 0x02))
                    handleButtonValueChange(Gamepad::Button::DPAD_RIGHT,
                                            (newBitmask & 0x02) > 0,
                                            (newBitmask & 0x02) > 0 ? 1.0F : 0.0F);
                if ((oldBitmask & 0x04) != (newBitmask & 0x04))
                    handleButtonValueChange(Gamepad::Button::DPAD_DOWN,
                                            (newBitmask & 0x04) > 0,
                                            (newBitmask & 0x04) > 0 ? 1.0F : 0.0F);
                if ((oldBitmask & 0x08) != (newBitmask & 0x08))
                    handleButtonValueChange(Gamepad::Button::DPAD_LEFT,
                                            (newBitmask & 0x08) > 0,
                                            (newBitmask & 0x08) > 0 ? 1.0F : 0.0F);

                hatValue = newDIState.rgdwPOV[0];
            }

            for (auto& buttonPair : buttons)
            {
                DWORD offset = buttonPair.first;
                Button& button = buttonPair.second;
                BYTE newValue = *reinterpret_cast<const BYTE*>(reinterpret_cast<const uint8_t*>(&newDIState) + offset);

                if (button.value != newValue)
                {
                    if ((button.button != Gamepad::Button::LEFT_TRIGGER || !hasLeftTrigger) &&
                        (button.button != Gamepad::Button::RIGHT_TRIGGER || !hasRightTrigger))
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
                DWORD offset = axisPair.first;
                Axis& axis = axisPair.second;
                LONG newValue = *reinterpret_cast<const LONG*>(reinterpret_cast<const uint8_t*>(&newDIState) + offset);

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
                float floatValue = static_cast<float>(newValue - min) / range;

                handleButtonValueChange(negativeButton,
                                        floatValue > 0.0F,
                                        floatValue);
            }
            else
            {
                float floatValue = 2.0F * (newValue - min) / range - 1.0F;

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
