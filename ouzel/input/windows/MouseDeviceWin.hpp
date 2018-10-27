// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/MouseDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class NativeCursorWin;

        class MouseDeviceWin final: public MouseDevice
        {
        public:
            MouseDeviceWin(InputSystem& initInputSystem,
                           uint32_t initId):
                MouseDevice(initInputSystem, initId)
            {
            }

            void setPosition(const Vector2& position);
            void setCursorVisible(bool visible);
            void setCursorLocked(bool locked);

            NativeCursorWin* getCursor() const { return cursor; }
            void setCursor(NativeCursorWin* newCursor);

        private:
            NativeCursorWin* cursor = nullptr;
        };
    } // namespace input
} // namespace ouzel
