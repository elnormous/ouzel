// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#if defined(__OBJC__)
#include <GameController/GameController.h>
typedef GCController* GCControllerPtr;
#else
#include <objc/objc.h>
#include <objc/NSObjCRuntime.h>
typedef id GCControllerPtr;
#endif

#include "input/InputSystem.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadDeviceIOS;

        class InputSystemIOS: public InputSystem
        {
        public:
            static Keyboard::Key convertKeyCode(NSInteger keyCode);

            virtual ~InputSystemIOS() {}

            void handleButtonValueChange(const GamepadDeviceIOS& gamepad, Gamepad::Button button, bool pressed, float value);
        };
    } // namespace input
} // namespace ouzel
