// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <jni.h>
#include "input/InputSystem.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadDeviceAndroid;

        class InputSystemAndroid: public InputSystem
        {
        public:
            static Keyboard::Key convertKeyCode(jint keyCode);

            virtual ~InputSystemAndroid() {}

            void handleButtonValueChange(const GamepadDeviceAndroid& gamepad, Gamepad::Button button, bool pressed, float value);
        };
    }
}
