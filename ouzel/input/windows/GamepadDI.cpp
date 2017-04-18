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
            ZeroMemory(&diState, sizeof(state));

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
            DIJOYSTATE2 newDIState;

            HRESULT hr = device->GetDeviceState(sizeof(newDIState), &newDIState);

            if (hr == DIERR_NOTACQUIRED)
            {
                if (FAILED(device->Acquire()))
                {
                    Log(Log::Level::ERR) << "Failed to acquire DirectInput device";
                    return false;
                }

                hr = device->GetDeviceState(sizeof(newDIState), &newDIState);
            }

            if (hr != DI_OK)
            {
                return false;
            }

            if (newDIState.rgbButtons[0] != diState.rgbButtons[0]) handleButtonValueChange(GamepadButton::FACE3,
                                                                                           newDIState.rgbButtons[0] > 0,
                                                                                           (newDIState.rgbButtons[0] > 0) ? 1.0f : 0.0f);
            if (newDIState.rgbButtons[1] != diState.rgbButtons[1]) handleButtonValueChange(GamepadButton::FACE1,
                                                                                           newDIState.rgbButtons[1] > 0,
                                                                                           (newDIState.rgbButtons[1] > 0) ? 1.0f : 0.0f);
            if (newDIState.rgbButtons[2] != diState.rgbButtons[2]) handleButtonValueChange(GamepadButton::FACE2,
                                                                                           newDIState.rgbButtons[2] > 0,
                                                                                           (newDIState.rgbButtons[2] > 0) ? 1.0f : 0.0f);
            if (newDIState.rgbButtons[3] != diState.rgbButtons[3]) handleButtonValueChange(GamepadButton::FACE4,
                                                                                           newDIState.rgbButtons[3] > 0,
                                                                                           (newDIState.rgbButtons[3] > 0) ? 1.0f : 0.0f);
            if (newDIState.rgbButtons[4] != diState.rgbButtons[4]) handleButtonValueChange(GamepadButton::LEFT_SHOULDER,
                                                                                           newDIState.rgbButtons[4] > 0,
                                                                                           (newDIState.rgbButtons[4] > 0) ? 1.0f : 0.0f);
            if (newDIState.rgbButtons[5] != diState.rgbButtons[5]) handleButtonValueChange(GamepadButton::RIGHT_SHOULDER,
                                                                                           newDIState.rgbButtons[5] > 0,
                                                                                           (newDIState.rgbButtons[5] > 0) ? 1.0f : 0.0f);
            if (newDIState.rgbButtons[6] != diState.rgbButtons[6]) handleButtonValueChange(GamepadButton::LEFT_TRIGGER,
                                                                                           newDIState.rgbButtons[6] > 0,
                                                                                           (newDIState.rgbButtons[6] > 0) ? 1.0f : 0.0f);
            if (newDIState.rgbButtons[7] != diState.rgbButtons[7]) handleButtonValueChange(GamepadButton::RIGHT_TRIGGER,
                                                                                           newDIState.rgbButtons[7] > 0,
                                                                                           (newDIState.rgbButtons[7] > 0) ? 1.0f : 0.0f);
            if (newDIState.rgbButtons[8] != diState.rgbButtons[8]) handleButtonValueChange(GamepadButton::BACK,
                                                                                           newDIState.rgbButtons[8] > 0,
                                                                                           (newDIState.rgbButtons[8] > 0) ? 1.0f : 0.0f);
            if (newDIState.rgbButtons[9] != diState.rgbButtons[9]) handleButtonValueChange(GamepadButton::START,
                                                                                           newDIState.rgbButtons[9] > 0,
                                                                                           (newDIState.rgbButtons[9] > 0) ? 1.0f : 0.0f);
            if (newDIState.rgbButtons[10] != diState.rgbButtons[10]) handleButtonValueChange(GamepadButton::LEFT_THUMB,
                                                                                             newDIState.rgbButtons[10] > 0,
                                                                                             (newDIState.rgbButtons[10] > 0) ? 1.0f : 0.0f);
            if (newDIState.rgbButtons[11] != diState.rgbButtons[11]) handleButtonValueChange(GamepadButton::RIGHT_THUMB,
                                                                                             newDIState.rgbButtons[11] > 0,
                                                                                             (newDIState.rgbButtons[11] > 0) ? 1.0f : 0.0f);

            if (newDIState.rgdwPOV[0] != diState.rgdwPOV[0])
            {
                State newState = state;

                switch (newDIState.rgdwPOV[0])
                {
                    case 0:
                        newState.dpadLeft = false;
                        newState.dpadRight = false;
                        newState.dpadUp = true;
                        newState.dpadDown = false;
                        break;
                    case 4500:
                        newState.dpadLeft = false;
                        newState.dpadRight = true;
                        newState.dpadUp = true;
                        newState.dpadDown = false;
                        break;
                    case 9000:
                        newState.dpadLeft = false;
                        newState.dpadRight = true;
                        newState.dpadUp = false;
                        newState.dpadDown = false;
                        break;
                    case 13500:
                        newState.dpadLeft = false;
                        newState.dpadRight = true;
                        newState.dpadUp = false;
                        newState.dpadDown = true;
                        break;
                    case 18000:
                        newState.dpadLeft = false;
                        newState.dpadRight = false;
                        newState.dpadUp = false;
                        newState.dpadDown = true;
                        break;
                    case 22500:
                        newState.dpadLeft = true;
                        newState.dpadRight = false;
                        newState.dpadUp = false;
                        newState.dpadDown = true;
                        break;
                    case 27000:
                        newState.dpadLeft = true;
                        newState.dpadRight = false;
                        newState.dpadUp = false;
                        newState.dpadDown = false;
                        break;
                    case 31500:
                        newState.dpadLeft = true;
                        newState.dpadRight = false;
                        newState.dpadUp = true;
                        newState.dpadDown = false;
                        break;
                    case 0xFFFFFFFF:
                        newState.dpadLeft = false;
                        newState.dpadRight = false;
                        newState.dpadUp = false;
                        newState.dpadDown = false;
                        break;
                }

                if (newState.dpadLeft != state.dpadLeft) handleButtonValueChange(GamepadButton::DPAD_LEFT,
                                                                                 newState.dpadLeft,
                                                                                 newState.dpadLeft ? 1.0f : 0.0f);
                if (newState.dpadRight != state.dpadRight) handleButtonValueChange(GamepadButton::DPAD_RIGHT,
                                                                                   newState.dpadRight,
                                                                                   newState.dpadRight ? 1.0f : 0.0f);
                if (newState.dpadUp != state.dpadUp) handleButtonValueChange(GamepadButton::DPAD_UP,
                                                                             newState.dpadUp,
                                                                             newState.dpadUp ? 1.0f : 0.0f);
                if (newState.dpadDown != state.dpadDown) handleButtonValueChange(GamepadButton::DPAD_DOWN,
                                                                                 newState.dpadDown,
                                                                                 newState.dpadDown ? 1.0f : 0.0f);

                state = newState;
            }

            diState = newDIState;

            return true;
        }
    } // namespace input
} // namespace ouzel
