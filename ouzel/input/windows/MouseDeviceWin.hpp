// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_MOUSEDEVICEWIN_HPP
#define OUZEL_INPUT_MOUSEDEVICEWIN_HPP

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
            bool isCursorVisible() const { return cursorVisible; }
            void setCursorVisible(bool visible);
            void setCursorLocked(bool locked);

            NativeCursorWin* getCursor() const { return cursor; }
            void setCursor(NativeCursorWin* newCursor);

        private:
            NativeCursorWin* cursor = nullptr;
            bool cursorVisible = true;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_MOUSEDEVICEWIN_HPP
