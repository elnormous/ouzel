// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include "input/Input.h"

namespace ouzel
{
    class Engine;

    namespace input
    {
        class InputLinux: public Input
        {
            friend Engine;
        public:
            static KeyboardKey convertKeyCode(KeySym keyCode);
            static uint32_t getModifiers(unsigned int state);

            virtual ~InputLinux();
        protected:
            InputLinux();
        };
    } // namespace input
} // namespace ouzel
