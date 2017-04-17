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

            if (FAILED(device->Acquire()))
            {
                Log(Log::Level::ERR) << "Failed to acquire DirectInput device";
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

            if (device->GetDeviceState(sizeof(newState), &newState) != DI_OK)
            {
                return false;
            }

            // TODO: check state changes

            state = newState;

            return true;
        }
    } // namespace input
} // namespace ouzel
