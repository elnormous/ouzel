// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"
#include "input/Mouse.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemMacOS;

        class MouseMacOS: public InputDevice
        {
        public:
            MouseMacOS(InputSystemMacOS& initInputSystemMacOS,
                       uint32_t initDeviceId):
                InputDevice(initDeviceId),
                inputSystemMacOS(initInputSystemMacOS)
            {
            }

            virtual ~MouseMacOS() {}

            void handleButtonPress(Mouse::Button button, const Vector2& position, uint32_t modifiers);
            void handleButtonRelease(Mouse::Button button, const Vector2& position, uint32_t modifiers);
            void handleMove(const Vector2& position, uint32_t modifiers);
            void handleScroll(const Vector2& scroll, const Vector2& position, uint32_t modifiers);

        private:
            InputSystemMacOS& inputSystemMacOS;
        };
    } // namespace input
} // namespace ouzel
