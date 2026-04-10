#ifndef FLUXION_INPUT_GAMEPADDEVICE_HPP
#define FLUXION_INPUT_GAMEPADDEVICE_HPP

#include <future>
#include "InputDevice.hpp"
#include "Gamepad.hpp"

namespace fluxion::input
{
    class GamepadDevice: public InputDevice
    {
    public:
        GamepadDevice(InputSystem& initInputSystem, DeviceId initId);
        ~GamepadDevice() override;

        std::future<bool> handleButtonValueChange(Gamepad::Button button, bool pressed, float value);
    };
}

#endif // FLUXION_INPUT_GAMEPADDEVICE_HPP
