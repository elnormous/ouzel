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
            InputDevice(uint32_t initDeviceId): deviceId(initDeviceId) {}
            virtual ~InputDevice() {}

            inline uint32_t getDeviceId() const { return deviceId; }

        protected:
            uint32_t deviceId;
        };
    } // namespace input
} // namespace ouzel
