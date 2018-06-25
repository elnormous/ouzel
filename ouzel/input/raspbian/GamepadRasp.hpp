// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/Gamepad.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadRasp: public Gamepad
        {
        public:
            GamepadRasp();
            virtual ~GamepadRasp();
        };
    } // namespace input
} // namespace ouzel
