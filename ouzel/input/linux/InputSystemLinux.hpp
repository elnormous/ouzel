// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"
#if OUZEL_SUPPORTS_X11
#  include <X11/keysym.h>
#  include <X11/X.h>
#endif
#include "input/InputSystem.hpp"
#include "input/Keyboard.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemLinux: public InputSystem
        {
        public:
#if OUZEL_SUPPORTS_X11
            static Keyboard::Key convertKeyCode(KeySym keyCode);
            static uint32_t getModifiers(unsigned int state);
#else
            static Keyboard::Key convertKeyCode(uint16_t keyCode);
#endif
            uint32_t getModifiers() const;

            virtual ~InputSystemLinux() {}
        };
    }
}
