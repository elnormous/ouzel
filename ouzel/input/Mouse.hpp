// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class Mouse: public InputDevice
        {
        public:
            Mouse(): InputDevice(InputDevice::Type::MOUSE) {}
            virtual ~Mouse() {}
        };
    }
}
