// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"
#include "math/Vector2.hpp"

namespace ouzel
{
    namespace input
    {
        class TouchpadDevice: public InputDevice
        {
        public:
            TouchpadDevice(InputSystem& initInputSystem, uint32_t initId):
                InputDevice(initInputSystem, initId, Controller::Type::TOUCHPAD)
            {
            }

            void handleTouchBegin(uint64_t touchId, const Vector2& position, float force = 1.0F);
            void handleTouchEnd(uint64_t touchId, const Vector2& position, float force = 1.0F);
            void handleTouchMove(uint64_t touchId, const Vector2& position, float force = 1.0F);
            void handleTouchCancel(uint64_t touchId, const Vector2& position, float force = 1.0F);
        };
    } // namespace input
} // namespace ouzel
