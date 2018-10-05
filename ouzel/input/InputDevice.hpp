// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include "input/Controller.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystem;

        class InputDevice
        {
        public:
            InputDevice(InputSystem& initInputSystem, uint32_t initId, Controller::Type initType):
                inputSystem(initInputSystem), id(initId), type(initType)
            {
            }
            virtual ~InputDevice() {}

            inline uint32_t getId() const { return id; }
            inline Controller::Type getType() const { return type; }

        protected:
            InputSystem& inputSystem;
            uint32_t id;
            Controller::Type type;
        };
    } // namespace input
} // namespace ouzel
