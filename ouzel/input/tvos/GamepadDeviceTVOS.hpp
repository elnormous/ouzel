// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <string>
#if defined(__OBJC__)
#include <GameController/GameController.h>
typedef GCController* GCControllerPtr;
#else
#include <objc/objc.h>
typedef id GCControllerPtr;
#endif

#include "input/Gamepad.hpp"
#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemTVOS;

        class GamepadDeviceTVOS: public InputDevice
        {
        public:
            GamepadDeviceTVOS(InputSystemTVOS& initInputSystemTVOS,
                              uint32_t initDeviceId,
                              GCControllerPtr initController);

            void setAbsoluteDPadValues(bool absoluteDPadValues);
            bool isAbsoluteDPadValues() const;

            int32_t getPlayerIndex() const;
            void setPlayerIndex(int32_t playerIndex);

            inline GCControllerPtr getController() const { return controller; }

        private:
            InputSystemTVOS& inputSystemTVOS;
            GCControllerPtr controller;
            std::string name;
            bool attached = false;
        };
    } // namespace input
} // namespace ouzel
