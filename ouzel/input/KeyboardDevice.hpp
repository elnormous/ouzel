// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class KeyboardDevice: public InputDevice
        {
        public:
            KeyboardDevice(uint32_t initId):
                InputDevice(initId)
            {
            }
        };
    } // namespace input
} // namespace ouzel
