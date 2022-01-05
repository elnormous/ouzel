// Ouzel by Elviss Strazdins

#ifndef OUZEL_INPUT_GAMEPADDEVICEIOS_HPP
#define OUZEL_INPUT_GAMEPADDEVICEIOS_HPP

#include <string>
#ifdef __OBJC__
#  include <GameController/GameController.h>
using GCControllerPtr = GCController*;
#else
#  include <objc/objc.h>
using GCControllerPtr = id;
#endif

#include "../GamepadDevice.hpp"
#include "../Gamepad.hpp"

namespace ouzel::input::ios
{
    class InputSystem;

    class GamepadDevice final: public input::GamepadDevice
    {
    public:
        GamepadDevice(InputSystem& initInputSystem,
                      DeviceId initId,
                      GCControllerPtr initController);

        bool isAbsoluteDpadValues() const;
        void setAbsoluteDpadValues(bool absoluteDpadValues);

        bool isRotationAllowed() const;
        void setRotationAllowed(bool rotationAllowed);

        std::int32_t getPlayerIndex() const;
        void setPlayerIndex(std::int32_t playerIndex);

        auto getController() const noexcept { return controller; }

    private:
        GCControllerPtr controller = nil;
        std::string name;
        bool attached = false;
    };
}

#endif // OUZEL_INPUT_GAMEPADDEVICEIOS_HPP
