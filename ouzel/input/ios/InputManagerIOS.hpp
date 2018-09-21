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

#include "input/InputManager.hpp"

namespace ouzel
{
    namespace input
    {
        class InputManagerIOS: public InputManager
        {
            friend Engine;
        public:
            virtual ~InputManagerIOS();

            virtual void startDeviceDiscovery() override;
            virtual void stopDeviceDiscovery() override;

            virtual void showVirtualKeyboard() override;
            virtual void hideVirtualKeyboard() override;

            void handleGamepadDiscoveryCompleted();
            void handleGamepadConnected(GCControllerPtr controller);
            void handleGamepadDisconnected(GCControllerPtr controller);

            static Keyboard::Key convertKeyCode(NSInteger keyCode);

        private:
            InputManagerIOS();

            id connectDelegate = nil;

            bool discovering = false;
            bool cursorVisible = true;
            bool cursorLocked = false;
        };
    } // namespace input
} // namespace ouzel
