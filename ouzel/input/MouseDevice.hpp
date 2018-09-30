// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class MouseDevice: public InputDevice
        {
        public:
            MouseDevice(uint32_t initId):
                InputDevice(initId)
            {
            }
        };
    } // namespace input
} // namespace ouzel
