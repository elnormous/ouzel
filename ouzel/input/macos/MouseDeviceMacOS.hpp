// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_MOUSEDEVICEMACOS_HPP
#define OUZEL_INPUT_MOUSEDEVICEMACOS_HPP

#include "input/MouseDevice.hpp"
#include "input/Mouse.hpp"

namespace ouzel
{
    namespace input
    {
        class CursorMacOS;

        class MouseDeviceMacOS final: public MouseDevice
        {
        public:
            MouseDeviceMacOS(InputSystem& initInputSystem,
                             uint32_t initId):
                MouseDevice(initInputSystem, initId)
            {
            }

            void setPosition(const Vector2F& position);
            inline bool isCursorVisible() const { return cursorVisible; }
            void setCursorVisible(bool newVisible);
            void setCursorLocked(bool locked);

            inline CursorMacOS* getCursor() const { return cursor; }
            void setCursor(CursorMacOS* newCursor);

        private:
            CursorMacOS* cursor = nullptr;
            bool cursorVisible = true;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_MOUSEDEVICEMACOS_HPP
