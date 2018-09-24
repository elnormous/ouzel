// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "GamepadAndroid.hpp"
#include "InputSystemAndroid.hpp"

namespace ouzel
{
    namespace input
    {
        GamepadAndroid::GamepadAndroid(InputSystemAndroid& initInputSystemAndroid,
                                       uint32_t initDeviceId):
            inputSystemAndroid(initInputSystemAndroid),
            deviceId(initDeviceId)
        {
        }

        GamepadAndroid::~GamepadAndroid()
        {
        }
    } // namespace input
} // namespace ouzel
