// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <emscripten/html5.h>
#include "input/Gamepad.h"

namespace ouzel
{
    namespace input
    {
        class InputEm;

        class GamepadEm: public Gamepad
        {
            friend InputEm;
        public:
            void update();

            long getIndex() const { return index; }

        protected:
            GamepadEm(long aIndex);

            long index = 0;
            double axis[64];
            double analogButton[64];
            EM_BOOL digitalButton[64];
        };
    } // namespace input
} // namespace ouzel
