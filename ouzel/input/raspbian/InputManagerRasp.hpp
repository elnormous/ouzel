// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <vector>
#include "input/raspbian/EventDevice.hpp"
#include "input/InputManager.hpp"

namespace ouzel
{
    class Engine;
    class EngineRasp;

    namespace input
    {
        class InputManagerRasp: public InputManager
        {
            friend Engine;
            friend EngineRasp;
        private:
            InputManagerRasp();
            virtual void init() override;

            void update();

            uint32_t getModifiers() const;

            int maxFd = 0;
            std::vector<EventDevice> inputDevices;
            bool keyboardKeyDown[256];
            bool mouseButtonDown[3];
        };
    } // namespace input
} // namespace ouzel
