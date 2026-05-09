#ifndef FLUXION_INPUT_KEYBOARDDEVICE_HPP
#define FLUXION_INPUT_KEYBOARDDEVICE_HPP

#include <future>
#include "InputDevice.hpp"
#include "Keyboard.hpp"

namespace fluxion::input
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

#endif // FLUXION_INPUT_KEYBOARDDEVICE_HPP
