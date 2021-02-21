// Ouzel by Elviss Strazdins

#ifndef OUZEL_INPUT_KEYBOARDDEVICE_HPP
#define OUZEL_INPUT_KEYBOARDDEVICE_HPP

#include <future>
#include "InputDevice.hpp"
#include "Keyboard.hpp"

namespace ouzel::input
{
    class KeyboardDevice: public InputDevice
    {
    public:
        KeyboardDevice(InputSystem& initInputSystem, DeviceId initId);
        ~KeyboardDevice() override;

        std::future<bool> handleKeyPress(Keyboard::Key key);
        std::future<bool> handleKeyRelease(Keyboard::Key key);
    };
}

#endif // OUZEL_INPUT_KEYBOARDDEVICE_HPP
