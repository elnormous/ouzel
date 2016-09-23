// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <map>
#include <string>
#include <html5.h>
#include "input/Input.h"

namespace ouzel
{
    class Engine;

    namespace input
    {
        class InputEm: public Input
        {
            friend Engine;
        public:
            virtual ~InputEm();

            static KeyboardKey convertKeyCode(const EM_UTF8 key[32]);
            static uint32_t getKeyboardModifiers(const EmscriptenKeyboardEvent* keyboardEvent);
            static uint32_t getMouseModifiers(const EmscriptenMouseEvent* mouseEvent);

        protected:
            InputEm();
        };
    } // namespace input
} // namespace ouzel
