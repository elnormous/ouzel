// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_MOUSEDEVICEMACOS_HPP
#define OUZEL_INPUT_MOUSEDEVICEMACOS_HPP

#include "input/MouseDevice.hpp"
#include "input/Mouse.hpp"

namespace ouzel
{
    namespace input
    {
        class NativeCursorMacOS;

        class MouseDeviceMacOS final: public MouseDevice
        {
        public:
            MouseDeviceMacOS(InputSystem& initInputSystem,
                             uint32_t initId):
                MouseDevice(initInputSystem, initId)
            {
            }

            void setPosition(const Vector2& position);
            bool isCursorVisible() const { return cursorVisible; }
            void setCursorVisible(bool newVisible);
            void setCursorLocked(bool locked);

            NativeCursorMacOS* getCursor() const { return cursor; }
            void setCursor(NativeCursorMacOS* newCursor);

        private:
            NativeCursorMacOS* cursor = nullptr;
            bool cursorVisible = true;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_MOUSEDEVICEMACOS_HPP
