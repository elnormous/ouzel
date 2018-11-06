// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <string>
#include "input/GamepadDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadDeviceMacOS: public GamepadDevice
        {
        public:
            GamepadDeviceMacOS(InputSystem& initInputSystem,
                               uint32_t initId):
                GamepadDevice(initInputSystem, initId)
            {
            }

            virtual void setPlayerIndex(int32_t) {};

        protected:
            std::string name;
        };
    } // namespace input
} // namespace ouzel
