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

            virtual void showVirtualKeyboard() override;
            virtual void hideVirtualKeyboard() override;

        private:
            InputManagerIOS();
        };
    } // namespace input
} // namespace ouzel
