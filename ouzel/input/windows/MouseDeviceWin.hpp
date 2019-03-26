// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_MOUSEDEVICEWIN_HPP
#define OUZEL_INPUT_MOUSEDEVICEWIN_HPP

#include "input/MouseDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class CursorWin;

        class MouseDeviceWin final: public MouseDevice
        {
        public:
            MouseDeviceWin(InputSystem& initInputSystem,
                           uint32_t initId):
                MouseDevice(initInputSystem, initId)
            {
            }

            void setPosition(const Vector2F& position);
            inline bool isCursorVisible() const { return cursorVisible; }
            void setCursorVisible(bool visible);
            void setCursorLocked(bool locked);

            inline CursorWin* getCursor() const { return cursor; }
            void setCursor(CursorWin* newCursor);

        private:
            CursorWin* cursor = nullptr;
            bool cursorVisible = true;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_MOUSEDEVICEWIN_HPP
