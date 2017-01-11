// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "input/Gamepad.h"

namespace ouzel
{
    namespace input
    {
        class GamepadAndroid: public Gamepad
        {
        public:
            virtual ~GamepadAndroid();

        protected:
            GamepadAndroid();
        };
    } // namespace input
} // namespace ouzel
