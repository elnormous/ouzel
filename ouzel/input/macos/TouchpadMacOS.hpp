// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "math/Vector2.hpp"
#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemMacOS;

        class TouchpadMacOS: public InputDevice
        {
        public:
            TouchpadMacOS(InputSystemMacOS& initInputSystemMacOS,
                          uint32_t initDeviceId):
                InputDevice(initDeviceId),
                inputSystemMacOS(initInputSystemMacOS)
            {
            }

            virtual ~TouchpadMacOS() {}

            void handleTouchBegin(uint64_t touchId, const Vector2& position, float force = 1.0F);
            void handleTouchEnd(uint64_t touchId, const Vector2& position, float force = 1.0F);
            void handleTouchMove(uint64_t touchId, const Vector2& position, float force = 1.0F);
            void handleTouchCancel(uint64_t touchId, const Vector2& position, float force = 1.0F);

        private:
            InputSystemMacOS& inputSystemMacOS;
        };
    } // namespace input
} // namespace ouzel
