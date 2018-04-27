// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

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
