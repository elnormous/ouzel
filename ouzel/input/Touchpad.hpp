// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class Touchpad: public InputDevice
        {
        public:
            Touchpad(): InputDevice(InputDevice::Type::TOUCHPAD) {}
            virtual ~Touchpad() {}
        };
    }
}
