// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <vector>
#include "input/Input.hpp"

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
            virtual bool init() override;

            uint32_t getModifiers() const;

            int maxFd = 0;
            std::vector<InputDeviceRasp> inputDevices;
            bool keyboardKeyDown[256];
            bool mouseButtonDown[3];
        };
    } // namespace input
} // namespace ouzel
