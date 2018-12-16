// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_GAMEPADDEVICE_HPP
#define OUZEL_INPUT_GAMEPADDEVICE_HPP

#include <future>
#include "input/InputDevice.hpp"
#include "input/Gamepad.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadDevice: public InputDevice
        {
        public:
            GamepadDevice(InputSystem& initInputSystem, uint32_t initId);
            ~GamepadDevice();

            std::future<bool> handleButtonValueChange(Gamepad::Button button, bool pressed, float value);
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_GAMEPADDEVICE_HPP
