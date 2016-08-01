// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "input/Gamepad.h"

namespace ouzel
{
    namespace input
    {
        class GamepadRPI: public Gamepad
        {
        public:
            virtual ~GamepadRPI();

        protected:
            GamepadRPI();
        };
    } // namespace input
} // namespace ouzel
