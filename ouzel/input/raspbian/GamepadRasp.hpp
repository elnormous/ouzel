// Copyright (C) 2017 Elviss Strazdins
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
            virtual ~GamepadRasp();

        protected:
            GamepadRasp();
        };
    } // namespace input
} // namespace ouzel
