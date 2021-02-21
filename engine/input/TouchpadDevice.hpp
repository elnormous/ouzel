// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_TOUCHPADDEVICE_HPP
#define OUZEL_INPUT_TOUCHPADDEVICE_HPP

#include <future>
#include "InputDevice.hpp"
#include "../math/Vector.hpp"

namespace ouzel::input
{
    class TouchpadDevice: public InputDevice
    {
    public:
        TouchpadDevice(InputSystem& initInputSystem, DeviceId initId, bool screen);
        ~TouchpadDevice() override;

        std::future<bool> handleTouchBegin(std::uint64_t touchId, const Vector2F& position, float force = 1.0F);
        std::future<bool> handleTouchEnd(std::uint64_t touchId, const Vector2F& position, float force = 1.0F);
        std::future<bool> handleTouchMove(std::uint64_t touchId, const Vector2F& position, float force = 1.0F);
        std::future<bool> handleTouchCancel(std::uint64_t touchId, const Vector2F& position, float force = 1.0F);
    };
}

#endif // OUZEL_INPUT_TOUCHPADDEVICE_HPP
