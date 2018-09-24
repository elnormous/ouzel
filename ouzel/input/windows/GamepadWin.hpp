// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>

namespace ouzel
{
    namespace input
    {
        class InputSystemWin;

        class GamepadWin
        {
        public:
            GamepadWin(InputSystemWin& initInputSystemWin,
                       uint32_t initDeviceId):
                inputSystemWin(initInputSystemWin),
                deviceId(initDeviceId)
            {
            }

            virtual ~GamepadWin() {}

            inline uint32_t getDeviceId() const { return deviceId; }

        protected:
            InputSystemWin& inputSystemWin;
            uint32_t deviceId;
        };
    } // namespace input
} // namespace ouzel
