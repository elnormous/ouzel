// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>

namespace ouzel
{
    namespace input
    {
        class InputDevice
        {
        public:
            InputDevice(uint32_t initId): id(initId) {}
            virtual ~InputDevice() {}

            inline uint32_t getId() const { return id; }

        protected:
            uint32_t id;
        };
    } // namespace input
} // namespace ouzel
