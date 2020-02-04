// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

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
            MouseDevice(InputSystem& initInputSystem, uintptr_t initId);
            ~MouseDevice();

            std::future<bool> handleButtonPress(Mouse::Button button, const Vector2F& position);
            std::future<bool> handleButtonRelease(Mouse::Button button, const Vector2F& position);
            std::future<bool> handleMove(const Vector2F& position);
            std::future<bool> handleRelativeMove(const Vector2F& position);
            std::future<bool> handleScroll(const Vector2F& scroll, const Vector2F& position);
            std::future<bool> handleCursorLockChange(bool locked);
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_MOUSEDEVICE_HPP
