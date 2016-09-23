// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include "input/Input.h"

namespace ouzel
{
    class Engine;

    namespace input
    {
        struct InputDeviceRasp
        {
            enum DeviceClass
            {
                CLASS_KEYBOARD = 1,
                CLASS_MOUSE = 2,
                CLASS_TOUCHPAD = 4,
                CLASS_GAMEPAD = 8
            };

            uint32_t deviceClass = 0;
            int fd = 0;
        };

        class InputRasp: public Input
        {
            friend Engine;
        public:
            virtual ~InputRasp();

            void update();

        protected:
            InputRasp();

            int maxFd = 0;
            std::vector<InputDeviceRasp> inputDevices;
        };
    } // namespace input
} // namespace ouzel
