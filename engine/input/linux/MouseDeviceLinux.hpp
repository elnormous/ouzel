// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_MOUSEDEVICELINUX_HPP
#define OUZEL_INPUT_MOUSEDEVICELINUX_HPP

#include "../MouseDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class CursorLinux;

        class MouseDeviceLinux final: public MouseDevice
        {
        public:
            MouseDeviceLinux(InputSystem& initInputSystem,
                             DeviceId initId):
                MouseDevice(initInputSystem, initId)
            {
            }

            void setPosition(const Vector2F& position);
            inline auto isCursorVisible() const noexcept { return cursorVisible; }
            void setCursorVisible(bool visible);
            void setCursorLocked(bool locked);

            inline auto getCursor() const noexcept { return cursor; }
            void setCursor(CursorLinux* newCursor);

        private:
            CursorLinux* cursor = nullptr;
            bool cursorVisible = true;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_MOUSEDEVICELINUX_HPP
