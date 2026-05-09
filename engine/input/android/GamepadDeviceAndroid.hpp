#ifndef FLUXION_INPUT_GAMEPADDEVICEANDROID_HPP
#define FLUXION_INPUT_GAMEPADDEVICEANDROID_HPP

#include "../GamepadDevice.hpp"
#include "../Gamepad.hpp"

namespace fluxion::input::android
{
    class InputSystem;

    class GamepadDevice final: public input::GamepadDevice
    {
    public:
        GamepadDevice(InputSystem& initInputSystem,
                      DeviceId initId);
    };
}

#endif // FLUXION_INPUT_GAMEPADDEVICEANDROID_HPP
