// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "input/Gamepad.h"

namespace ouzel
{
    namespace input
    {
        class GamepadLinux: public Gamepad
        {
        public:
            virtual ~GamepadLinux();

        protected:
            GamepadLinux();
        };
    } // namespace input
} // namespace ouzel
