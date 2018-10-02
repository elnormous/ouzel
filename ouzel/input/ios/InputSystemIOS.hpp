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

            InputSystemIOS();
            virtual ~InputSystemIOS();

            KeyboardDevice* getKeyboardDevice() const { return keyboardDevice; }
            TouchpadDevice* getTouchpadDevice() const { return touchpadDevice; }

            void startDeviceDiscovery();
            void stopDeviceDiscovery();

            void handleGamepadDiscoveryCompleted();
            void handleGamepadConnected(GCControllerPtr controller);
            void handleGamepadDisconnected(GCControllerPtr controller);

        private:
            id connectDelegate = nil;

            uint32_t lastDeviceId = 0;
            KeyboardDevice* keyboardDevice = nullptr;
            TouchpadDevice* touchpadDevice = nullptr;
        };
    } // namespace input
} // namespace ouzel
