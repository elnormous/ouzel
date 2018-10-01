// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "GamepadDeviceAndroid.hpp"

namespace ouzel
{
    namespace input
    {
        GamepadDeviceAndroid::GamepadDeviceAndroid(InputSystem& initInputSystem,
                                                   uint32_t initId):
            GamepadDevice(initInputSystem, initId)
        {
        }

        GamepadDeviceAndroid::~GamepadDeviceAndroid()
        {
        }
    } // namespace input
} // namespace ouzel
