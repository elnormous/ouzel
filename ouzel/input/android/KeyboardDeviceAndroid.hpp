// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/KeyboardDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class KeyboardDeviceAndroid: public KeyboardDevice
        {
        public:
            KeyboardDeviceAndroid(InputSystem& initInputSystem,
                                  uint32_t initId):
                KeyboardDevice(initInputSystem, initId)
            {
            }

            virtual ~KeyboardDeviceAndroid() {}
        };
    } // namespace input
} // namespace ouzel
