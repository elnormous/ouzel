// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"
#include "input/Mouse.hpp"

namespace ouzel
{
    namespace input
    {
        class MouseDevice: public InputDevice
        {
        public:
            MouseDevice(InputSystem& initInputSystem, uint32_t initId):
                InputDevice(initInputSystem, initId, Controller::Type::MOUSE)
            {
            }

            void handleButtonPress(Mouse::Button button, const Vector2& position);
            void handleButtonRelease(Mouse::Button button, const Vector2& position);
            void handleMove(const Vector2& position);
            void handleRelativeMove(const Vector2& position);
            void handleScroll(const Vector2& scroll, const Vector2& position);
            void handleCursorLockChange(bool locked);
        };
    } // namespace input
} // namespace ouzel
