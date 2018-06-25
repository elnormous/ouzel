// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class Keyboard: public InputDevice
        {
        public:
            Keyboard(): InputDevice(InputDevice::Type::KEYBOARD) {}
            virtual ~Keyboard() {}
        };
    }
}
