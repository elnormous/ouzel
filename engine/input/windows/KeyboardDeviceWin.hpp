// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_KEYBOARDDEVICEWIN_HPP
#define OUZEL_INPUT_KEYBOARDDEVICEWIN_HPP

#include "../KeyboardDevice.hpp"

namespace ouzel::input::windows
{
    class InputSystem;

    class KeyboardDevice final: public input::KeyboardDevice
    {
    public:
        KeyboardDevice(InputSystem& initInputSystem,
                       DeviceId initId);

        std::future<bool> handleKeyPress(Keyboard::Key key);
        void update();

    private:
        bool leftShiftDown = false;
        bool rightShiftDown = false;
    };
}

#endif // OUZEL_INPUT_KEYBOARDDEVICEWIN_HPP
