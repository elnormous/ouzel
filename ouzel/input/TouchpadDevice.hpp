// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_TOUCHPADDEVICE_HPP
#define OUZEL_INPUT_TOUCHPADDEVICE_HPP

#include <future>
#include "input/InputDevice.hpp"
#include "math/Vector2.hpp"

namespace ouzel
{
    namespace input
    {
        class TouchpadDevice: public InputDevice
        {
        public:
            TouchpadDevice(InputSystem& initInputSystem, uint32_t initId, bool screen);
            ~TouchpadDevice();

            std::future<bool> handleTouchBegin(uint64_t touchId, const Vector2F& position, float force = 1.0F);
            std::future<bool> handleTouchEnd(uint64_t touchId, const Vector2F& position, float force = 1.0F);
            std::future<bool> handleTouchMove(uint64_t touchId, const Vector2F& position, float force = 1.0F);
            std::future<bool> handleTouchCancel(uint64_t touchId, const Vector2F& position, float force = 1.0F);
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_TOUCHPADDEVICE_HPP
