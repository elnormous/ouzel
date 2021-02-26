// Ouzel by Elviss Strazdins

#ifndef OUZEL_INPUT_MOUSEDEVICE_HPP
#define OUZEL_INPUT_MOUSEDEVICE_HPP

#include <future>
#include "InputDevice.hpp"
#include "Mouse.hpp"

namespace ouzel::input
{
    class MouseDevice: public InputDevice
    {
    public:
        MouseDevice(InputSystem& initInputSystem, DeviceId initId);
        ~MouseDevice() override;

        std::future<bool> handleButtonPress(Mouse::Button button, const Vector<float, 2>& position);
        std::future<bool> handleButtonRelease(Mouse::Button button, const Vector<float, 2>& position);
        std::future<bool> handleMove(const Vector<float, 2>& position);
        std::future<bool> handleRelativeMove(const Vector<float, 2>& position);
        std::future<bool> handleScroll(const Vector<float, 2>& scroll, const Vector<float, 2>& position);
        std::future<bool> handleCursorLockChange(bool locked);
    };
}

#endif // OUZEL_INPUT_MOUSEDEVICE_HPP
