#ifndef FLUXION_INPUT_GAMEPADDEVICEWIN_HPP
#define FLUXION_INPUT_GAMEPADDEVICEWIN_HPP

#include <cstdint>
#include "../GamepadDevice.hpp"

namespace fluxion::input::windows
{
    class InputSystem;

    class GamepadDevice : public input::GamepadDevice
    {
    public:
        GamepadDevice(InputSystem& initInputSystem,
                      DeviceId initId);
    };
}

#endif // FLUXION_INPUT_GAMEPADDEVICEWIN_HPP
