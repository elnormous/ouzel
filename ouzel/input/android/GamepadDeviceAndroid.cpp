// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "GamepadDeviceAndroid.hpp"
#include "InputSystemAndroid.hpp"

namespace ouzel
{
    namespace input
    {
        GamepadDeviceAndroid::GamepadDeviceAndroid(InputSystemAndroid& initInputSystemAndroid,
                                                   uint32_t initId):
            InputDevice(initId),
            inputSystemAndroid(initInputSystemAndroid)
        {
        }

        GamepadDeviceAndroid::~GamepadDeviceAndroid()
        {
        }
    } // namespace input
} // namespace ouzel
