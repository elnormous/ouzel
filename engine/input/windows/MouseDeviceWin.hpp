// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_MOUSEDEVICEWIN_HPP
#define OUZEL_INPUT_MOUSEDEVICEWIN_HPP

#include "../MouseDevice.hpp"

namespace ouzel::input::windows
{
    class InputSystem;
    class Cursor;

    class MouseDevice final: public input::MouseDevice
    {
    public:
        MouseDevice(InputSystem& initInputSystem,
                    DeviceId initId);

        void setPosition(const Vector2F& position);
        auto isCursorVisible() const noexcept { return cursorVisible; }
        void setCursorVisible(bool visible);
        void setCursorLocked(bool locked);

        auto getCursor() const noexcept { return cursor; }
        void setCursor(Cursor* newCursor);

    private:
        Cursor* cursor = nullptr;
        bool cursorVisible = true;
    };
}

#endif // OUZEL_INPUT_MOUSEDEVICEWIN_HPP
