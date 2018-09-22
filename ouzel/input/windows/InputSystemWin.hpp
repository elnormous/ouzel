// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputSystem.hpp"
#include "input/Keyboard.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemWin: public InputSystem
        {
        public:
            virtual ~InputSystemWin() {}

            static Keyboard::Key convertKeyCode(UINT keyCode);
            static uint32_t getModifiers(WPARAM wParam);
        };
    }
}
