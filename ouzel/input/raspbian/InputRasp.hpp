// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <vector>
#include "input/Input.hpp"

namespace ouzel
{
    class Engine;
    class EngineRasp;

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
            friend EngineRasp;
        public:
            virtual ~InputRasp();

        protected:
            InputRasp();
            virtual bool init() override;

            void update();

            uint32_t getModifiers() const;

            int maxFd = 0;
            std::vector<InputDeviceRasp> inputDevices;
            bool keyboardKeyDown[256];
            bool mouseButtonDown[3];
        };
    } // namespace input
} // namespace ouzel
