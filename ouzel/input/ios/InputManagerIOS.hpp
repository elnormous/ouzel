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

            virtual void startGamepadDiscovery() override;
            virtual void stopGamepadDiscovery() override;

            virtual bool showVirtualKeyboard() override;
            virtual bool hideVirtualKeyboard() override;

            void handleGamepadDiscoveryCompleted();
            void handleGamepadConnected(GCControllerPtr controller);
            void handleGamepadDisconnected(GCControllerPtr controller);

            static KeyboardKey convertKeyCode(NSInteger keyCode);

        private:
            InputManagerIOS();
            virtual void init() override;

            id connectDelegate = nil;

            bool discovering = false;
            bool cursorVisible = true;
            bool cursorLocked = false;
        };
    } // namespace input
} // namespace ouzel
