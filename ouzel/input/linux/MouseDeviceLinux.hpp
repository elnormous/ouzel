// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/MouseDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class NativeCursorLinux;

        class MouseDeviceLinux final: public MouseDevice
        {
        public:
            MouseDeviceLinux(InputSystem& initInputSystem,
                             uint32_t initId):
                MouseDevice(initInputSystem, initId)
            {
            }

            void setPosition(const Vector2& position);
            bool isCursorVisible() const { return cursorVisible; }
            void setCursorVisible(bool visible);
            void setCursorLocked(bool locked);

            NativeCursorLinux* getCursor() const { return cursor; }
            void setCursor(NativeCursorLinux* newCursor);

        private:
            NativeCursorLinux* cursor = nullptr;
            bool cursorVisible = true;
        };
    } // namespace input
} // namespace ouzel
