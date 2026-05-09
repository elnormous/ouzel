#ifndef FLUXION_INPUT_CONTROLLER_HPP
#define FLUXION_INPUT_CONTROLLER_HPP

#include "DeviceId.hpp"

namespace fluxion::input
{
    class InputManager;

    class Controller
    {
    public:
        enum class Type
        {
            none,
            keyboard,
            mouse,
            touchpad,
            gamepad
        };

        Controller(InputManager& initInputManager, Type initType, DeviceId initDeviceId):
            inputManager{initInputManager}, type{initType}, deviceId{initDeviceId}
        {}
        virtual ~Controller() = default;

        auto getType() const noexcept { return type; }
        auto getDeviceId() const noexcept { return deviceId; }

    protected:
        InputManager& inputManager;
        Type type;
        DeviceId deviceId;
    };
}

#endif // FLUXION_INPUT_CONTROLLER_HPP
