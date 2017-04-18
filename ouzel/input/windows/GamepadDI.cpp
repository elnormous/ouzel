// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "GamepadDI.h"
#include "InputWin.h"
#include "core/Engine.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace input
    {
        static const int32_t MAX_THUMB_VALUE = 32767;
        static const int32_t MIN_THUMB_VALUE = -32768;

        GamepadDI::GamepadDI(const DIDEVICEINSTANCE* aInstance):
            instance(aInstance)
        {
            ZeroMemory(&state, sizeof(state));

            vendorId = LOWORD(instance->guidProduct.Data1);
            productId = HIWORD(instance->guidProduct.Data1);

            InputWin* inputWin = static_cast<InputWin*>(sharedEngine->getInput());

            if (FAILED(inputWin->getDirectInput()->CreateDevice(instance->guidInstance, &device, nullptr)))
            {
                Log(Log::Level::ERR) << "Failed to create DirectInput device";
                return;
            }

            if (FAILED(device->SetDataFormat(&c_dfDIJoystick2)))
            {
                Log(Log::Level::ERR) << "Failed to set DirectInput device format";
                return;
            }
        }

        GamepadDI::~GamepadDI()
        {
            if (device)
            {
                device->Unacquire();
                device->Release();
            }
        }

        bool GamepadDI::update()
        {
            DIJOYSTATE2 newState;

            HRESULT hr = device->GetDeviceState(sizeof(newState), &newState);

            if (hr == DIERR_NOTACQUIRED)
            {
                if (FAILED(device->Acquire()))
                {
                    Log(Log::Level::ERR) << "Failed to acquire DirectInput device";
                    return false;
                }

                hr = device->GetDeviceState(sizeof(newState), &newState);
            }

            if (hr != DI_OK)
            {
                return false;
            }

            if (newState.rgbButtons[0] != state.rgbButtons[0]) handleButtonValueChange(GamepadButton::FACE3,
                                                                                       newState.rgbButtons[0] > 0,
                                                                                       (newState.rgbButtons[0] > 0) ? 1.0f : 0.0f);
            if (newState.rgbButtons[1] != state.rgbButtons[1]) handleButtonValueChange(GamepadButton::FACE1,
                                                                                       newState.rgbButtons[1] > 0,
                                                                                       (newState.rgbButtons[1] > 0) ? 1.0f : 0.0f);
            if (newState.rgbButtons[2] != state.rgbButtons[2]) handleButtonValueChange(GamepadButton::FACE2,
                                                                                       newState.rgbButtons[2] > 0,
                                                                                       (newState.rgbButtons[2] > 0) ? 1.0f : 0.0f);
            if (newState.rgbButtons[3] != state.rgbButtons[3]) handleButtonValueChange(GamepadButton::FACE4,
                                                                                       newState.rgbButtons[3] > 0,
                                                                                       (newState.rgbButtons[3] > 0) ? 1.0f : 0.0f);
            if (newState.rgbButtons[4] != state.rgbButtons[4]) handleButtonValueChange(GamepadButton::LEFT_SHOULDER,
                                                                                       newState.rgbButtons[4] > 0,
                                                                                       (newState.rgbButtons[4] > 0) ? 1.0f : 0.0f);
            if (newState.rgbButtons[5] != state.rgbButtons[5]) handleButtonValueChange(GamepadButton::RIGHT_SHOULDER,
                                                                                       newState.rgbButtons[5] > 0,
                                                                                       (newState.rgbButtons[5] > 0) ? 1.0f : 0.0f);
            if (newState.rgbButtons[6] != state.rgbButtons[6]) handleButtonValueChange(GamepadButton::LEFT_TRIGGER,
                                                                                       newState.rgbButtons[6] > 0,
                                                                                       (newState.rgbButtons[6] > 0) ? 1.0f : 0.0f);
            if (newState.rgbButtons[7] != state.rgbButtons[7]) handleButtonValueChange(GamepadButton::RIGHT_TRIGGER,
                                                                                       newState.rgbButtons[7] > 0,
                                                                                       (newState.rgbButtons[7] > 0) ? 1.0f : 0.0f);
            if (newState.rgbButtons[8] != state.rgbButtons[8]) handleButtonValueChange(GamepadButton::BACK,
                                                                                       newState.rgbButtons[8] > 0,
                                                                                       (newState.rgbButtons[8] > 0) ? 1.0f : 0.0f);
            if (newState.rgbButtons[9] != state.rgbButtons[9]) handleButtonValueChange(GamepadButton::START,
                                                                                       newState.rgbButtons[9] > 0,
                                                                                       (newState.rgbButtons[9] > 0) ? 1.0f : 0.0f);
            if (newState.rgbButtons[10] != state.rgbButtons[10]) handleButtonValueChange(GamepadButton::LEFT_THUMB,
                                                                                         newState.rgbButtons[10] > 0,
                                                                                         (newState.rgbButtons[10] > 0) ? 1.0f : 0.0f);
            if (newState.rgbButtons[11] != state.rgbButtons[11]) handleButtonValueChange(GamepadButton::RIGHT_THUMB,
                                                                                         newState.rgbButtons[11] > 0,
                                                                                         (newState.rgbButtons[11] > 0) ? 1.0f : 0.0f);

            if ()

            state = newState;

            return true;
        }
    } // namespace input
} // namespace ouzel
