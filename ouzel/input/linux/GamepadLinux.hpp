// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#pragma once

#include "input/Gamepad.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadLinux: public Gamepad
        {
        public:
            GamepadLinux();
            virtual ~GamepadLinux();
        };
    } // namespace input
} // namespace ouzel
