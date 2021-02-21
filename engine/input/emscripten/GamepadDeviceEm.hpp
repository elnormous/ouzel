// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_GAMEPADDEVICEEM_HPP
#define OUZEL_INPUT_GAMEPADDEVICEEM_HPP

#include "../GamepadDevice.hpp"
#include "../Gamepad.hpp"

namespace ouzel::input::emscripten
{
    class InputSystem;

    class GamepadDevice final: public input::GamepadDevice
    {
    public:
        GamepadDevice(InputSystem& initInputSystem,
                      DeviceId initId,
                      long initIndex);

        void update();

        auto getIndex() const noexcept { return index; }

    private:
        void handleThumbAxisChange(double oldValue, double newValue,
                                   Gamepad::Button negativeButton, Gamepad::Button positiveButton);

        long index = 0;
        double axis[64]{};
        double analogButton[64]{};
    };
}

#endif // OUZEL_INPUT_GAMEPADDEVICEEM_HPP
