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
        class GamepadDeviceTVOS;

        class InputSystemTVOS: public InputSystem
        {
        public:
            static Keyboard::Key convertKeyCode(NSInteger keyCode);

            InputSystemTVOS();
            virtual ~InputSystemTVOS() {}

            KeyboardDevice* getKeyboardDevice() const { return keyboardDevice; }

        private:
            uint32_t lastDeviceId = 0;
            KeyboardDevice* keyboardDevice = nullptr;
        };
    } // namespace input
} // namespace ouzel
