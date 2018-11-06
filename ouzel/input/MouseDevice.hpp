// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef MOUSEDEVICE_HPP
#define MOUSEDEVICE_HPP

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
            MouseDevice(InputSystem& initInputSystem, uint32_t initId):
                InputDevice(initInputSystem, initId, Controller::Type::MOUSE)
            {
            }

            std::future<bool> handleButtonPress(Mouse::Button button, const Vector2& position);
            std::future<bool> handleButtonRelease(Mouse::Button button, const Vector2& position);
            std::future<bool> handleMove(const Vector2& position);
            std::future<bool> handleRelativeMove(const Vector2& position);
            std::future<bool> handleScroll(const Vector2& scroll, const Vector2& position);
            std::future<bool> handleCursorLockChange(bool locked);
        };
    } // namespace input
} // namespace ouzel

#endif // MOUSEDEVICE_HPP
