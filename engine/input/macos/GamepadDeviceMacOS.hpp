#ifndef FLUXION_INPUT_GAMEPADDEVICEMACOS_HPP
#define FLUXION_INPUT_GAMEPADDEVICEMACOS_HPP

#include <string>
#include "../GamepadDevice.hpp"

namespace fluxion::input::macos
{
    class InputSystem;

    class GamepadDevice: public input::GamepadDevice
    {
    public:
        GamepadDevice(InputSystem& initInputSystem, DeviceId initId);

        virtual void setAbsoluteDpadValues([[maybe_unused]] bool absoluteDpadValues) {}
        virtual void setRotationAllowed([[maybe_unused]] bool rotationAllowed) {}
        virtual void setPlayerIndex(std::int32_t) {}

    protected:
        std::string name;
    };
}

#endif // FLUXION_INPUT_GAMEPADDEVICEMACOS_HPP
