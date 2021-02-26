// Ouzel by Elviss Strazdins

#ifndef OUZEL_INPUT_MOUSEDEVICELINUX_HPP
#define OUZEL_INPUT_MOUSEDEVICELINUX_HPP

#include "../MouseDevice.hpp"

namespace ouzel::input::linux
{
    class InputSystem;
    class Cursor;

    class MouseDevice final: public input::MouseDevice
    {
    public:
        MouseDevice(InputSystem& initInputSystem,
                    DeviceId initId);

        void setPosition(const Vector<float, 2>& position);
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

#endif // OUZEL_INPUT_MOUSEDEVICELINUX_HPP
