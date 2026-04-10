#ifndef FLUXION_INPUT_KEYBOARDDEVICELINUX_HPP
#define FLUXION_INPUT_KEYBOARDDEVICELINUX_HPP

#include "../KeyboardDevice.hpp"

namespace fluxion::input::linux
{
    class InputSystem;

    class KeyboardDevice final: public input::KeyboardDevice
    {
    public:
        KeyboardDevice(InputSystem& initInputSystem,
                       DeviceId initId);
    };
}

#endif // FLUXION_INPUT_KEYBOARDDEVICELINUX_HPP
