// Ouzel by Elviss Strazdins

#ifndef OUZEL_INPUT_MOUSEDEVICEEM_HPP
#define OUZEL_INPUT_MOUSEDEVICEEM_HPP

#include "../MouseDevice.hpp"

namespace ouzel::input::emscripten
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

#endif // OUZEL_INPUT_MOUSEDEVICEEM_HPP
