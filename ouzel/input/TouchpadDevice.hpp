// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class TouchpadDevice: public InputDevice
        {
        public:
            TouchpadDevice(uint32_t initId):
                InputDevice(initId)
            {
            }
        };
    } // namespace input
} // namespace ouzel
