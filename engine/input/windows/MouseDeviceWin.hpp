// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_MOUSEDEVICEWIN_HPP
#define OUZEL_INPUT_MOUSEDEVICEWIN_HPP

#include "../MouseDevice.hpp"

namespace ouzel::input
{
    class CursorWin;

    class MouseDeviceWin final: public MouseDevice
    {
    public:
        MouseDeviceWin(InputSystem& initInputSystem,
                       DeviceId initId):
            MouseDevice(initInputSystem, initId)
        {
        }

        void setPosition(const Vector2F& position);
        auto isCursorVisible() const noexcept { return cursorVisible; }
        void setCursorVisible(bool visible);
        void setCursorLocked(bool locked);

        auto getCursor() const noexcept { return cursor; }
        void setCursor(CursorWin* newCursor);

    private:
        CursorWin* cursor = nullptr;
        bool cursorVisible = true;
    };
}

#endif // OUZEL_INPUT_MOUSEDEVICEWIN_HPP
