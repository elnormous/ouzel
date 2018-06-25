// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

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
