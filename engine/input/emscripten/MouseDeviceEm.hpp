#ifndef FLUXION_INPUT_MOUSEDEVICEEM_HPP
#define FLUXION_INPUT_MOUSEDEVICEEM_HPP

#include "../MouseDevice.hpp"

namespace fluxion::input::emscripten
{
    class InputSystem;

    class MouseDevice final: public input::MouseDevice
    {
    public:
        MouseDevice(InputSystem& initInputSystem,
                    DeviceId initId);

        void setCursorVisible(bool visible);
        void setCursorLocked(bool locked);
    };
}

#endif // FLUXION_INPUT_MOUSEDEVICEEM_HPP
