// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>

namespace ouzel
{
    namespace input
    {
        class InputSystem;

        class InputDevice
        {
        public:
            InputDevice(InputSystem& initInputSystem, uint32_t initId):
                inputSystem(initInputSystem), id(initId)
            {
            }
            virtual ~InputDevice() {}

            inline uint32_t getId() const { return id; }

        protected:
            InputSystem& inputSystem;
            uint32_t id;
        };
    } // namespace input
} // namespace ouzel
