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

            void handleButtonPress(Mouse::Button button, const Vector2& position, uint32_t modifiers);
            void handleButtonRelease(Mouse::Button button, const Vector2& position, uint32_t modifiers);
            void handleMove(const Vector2& position, uint32_t modifiers);
            void handleScroll(const Vector2& scroll, const Vector2& position, uint32_t modifiers);
        };
    } // namespace input
} // namespace ouzel
