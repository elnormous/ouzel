// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <string>

namespace ouzel
{
    namespace input
    {
        class InputSystemMacOS;

        class GamepadMacOS
        {
        public:
            GamepadMacOS(InputSystemMacOS& initInputSystemMacOS,
                         uint32_t initDeviceId):
                inputSystemMacOS(initInputSystemMacOS),
                deviceId(initDeviceId)
            {
            }

            virtual ~GamepadMacOS() {}

            inline uint32_t getDeviceId() const { return deviceId; }

        protected:
            InputSystemMacOS& inputSystemMacOS;
            uint32_t deviceId;
            std::string name;
        };
    }
}
