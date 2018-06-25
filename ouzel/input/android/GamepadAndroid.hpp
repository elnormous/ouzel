// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/Gamepad.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadAndroid: public Gamepad
        {
        public:
            GamepadAndroid();
            virtual ~GamepadAndroid();
        };
    } // namespace input
} // namespace ouzel
