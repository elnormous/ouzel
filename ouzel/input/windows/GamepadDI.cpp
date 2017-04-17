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
            vendorId = LOWORD(instance->guidProduct.Data1);
            productId = HIWORD(instance->guidProduct.Data1);

            InputWin* inputWin = static_cast<InputWin*>(sharedEngine->getInput());

            if (FAILED(inputWin->getDirectInput()->CreateDevice(instance->guidInstance, &device, nullptr)))
            {
                Log(Log::Level::ERR) << "Failed to create DirectInput device";
            }
        }

        bool GamepadDI::update()
        {
            return true;
        }
    } // namespace input
} // namespace ouzel
