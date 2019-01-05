// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_MOUSEDEVICE_HPP
#define OUZEL_INPUT_MOUSEDEVICE_HPP

#include <future>
#include "input/InputDevice.hpp"
#include "input/Mouse.hpp"

namespace ouzel
{
    namespace input
    {
        class MouseDevice: public InputDevice
        {
        public:
            MouseDevice(InputSystem& initInputSystem, uint32_t initId);
            ~MouseDevice();

            std::future<bool> handleButtonPress(Mouse::Button button, const Vector2<float>& position);
            std::future<bool> handleButtonRelease(Mouse::Button button, const Vector2<float>& position);
            std::future<bool> handleMove(const Vector2<float>& position);
            std::future<bool> handleRelativeMove(const Vector2<float>& position);
            std::future<bool> handleScroll(const Vector2<float>& scroll, const Vector2<float>& position);
            std::future<bool> handleCursorLockChange(bool locked);
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_MOUSEDEVICE_HPP
