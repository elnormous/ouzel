// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <emscripten/html5.h>
#include "input/InputSystem.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadDeviceEm;

        class InputSystemEm: public InputSystem
        {
        public:
            static Keyboard::Key convertKeyCode(const EM_UTF8 key[32]);
            static uint32_t getKeyboardModifiers(const EmscriptenKeyboardEvent* keyboardEvent);
            static uint32_t getMouseModifiers(const EmscriptenMouseEvent* mouseEvent);

            virtual ~InputSystemEm() {}

            void handleButtonValueChange(const GamepadDeviceEm& gamepad, Gamepad::Button button, bool pressed, float value);
        };
    } // namespace input
} // namespace ouzel
