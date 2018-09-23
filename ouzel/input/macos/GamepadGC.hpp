// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#if defined(__OBJC__)
#include <GameController/GameController.h>
typedef GCController* GCControllerPtr;
#else
#include <objc/objc.h>
typedef id GCControllerPtr;
#endif

#include "input/macos/GamepadMacOS.hpp"
#include "input/Gamepad.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadGC: public GamepadMacOS
        {
        public:
            GamepadGC(InputSystemMacOS& initInputSystemMacOS,
                      uint32_t initDeviceId,
                      GCControllerPtr initController);

            void setAbsoluteDpadValues(bool absoluteDpadValues);
            bool isAbsoluteDpadValues() const;

            int32_t getPlayerIndex() const;
            void setPlayerIndex(int32_t playerIndex);

            GCControllerPtr getController() const { return controller; }

        private:
            GCControllerPtr controller;
            bool attached;
        };
    } // namespace input
} // namespace ouzel
