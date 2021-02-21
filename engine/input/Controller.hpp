// Ouzel by Elviss Strazdins

#ifndef OUZEL_INPUT_CONTROLLER_HPP
#define OUZEL_INPUT_CONTROLLER_HPP

#include "DeviceId.hpp"

namespace ouzel::input
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
            inputManager(initInputManager), type(initType), deviceId(initDeviceId)
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

#endif // OUZEL_INPUT_CONTROLLER_HPP
