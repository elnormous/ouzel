// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/MouseDevice.hpp"
#include "input/Mouse.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemMacOS;

        class MouseDeviceMacOS: public MouseDevice
        {
        public:
            MouseDeviceMacOS(InputSystemMacOS& initInputSystemMacOS,
                             uint32_t initId):
                MouseDevice(initId),
                inputSystemMacOS(initInputSystemMacOS)
            {
            }

            virtual ~MouseDeviceMacOS() {}

            void setPosition(const Vector2& position);
            void handleButtonPress(Mouse::Button button, const Vector2& position, uint32_t modifiers);
            void handleButtonRelease(Mouse::Button button, const Vector2& position, uint32_t modifiers);
            void handleMove(const Vector2& position, uint32_t modifiers);
            void handleScroll(const Vector2& scroll, const Vector2& position, uint32_t modifiers);

        private:
            InputSystemMacOS& inputSystemMacOS;
        };
    } // namespace input
} // namespace ouzel
