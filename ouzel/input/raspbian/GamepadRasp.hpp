// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

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
