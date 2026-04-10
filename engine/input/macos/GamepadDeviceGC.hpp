#ifndef FLUXION_INPUT_GAMEPADDEVICEGC_HPP
#define FLUXION_INPUT_GAMEPADDEVICEGC_HPP

#ifdef __OBJC__
#  include <GameController/GameController.h>
using GCControllerPtr = GCController*;
#else
#  include <objc/objc.h>
using GCControllerPtr = id;
#endif

#include "GamepadDeviceMacOS.hpp"
#include "../Gamepad.hpp"

namespace fluxion::input::macos
{
    class GamepadDeviceGC final: public GamepadDevice
    {
    public:
        GamepadDeviceGC(InputSystem& initInputSystem,
                        DeviceId initId,
                        GCControllerPtr initController);

        bool isAbsoluteDpadValues() const;
        void setAbsoluteDpadValues(bool absoluteDpadValues) final;

        bool isRotationAllowed() const;
        void setRotationAllowed(bool rotationAllowed) final;

        std::int32_t getPlayerIndex() const;
        void setPlayerIndex(std::int32_t playerIndex) final;

        auto getController() const noexcept { return controller; }

    private:
        GCControllerPtr controller = nil;
        bool attached = false;
    };
}

#endif // FLUXION_INPUT_GAMEPADDEVICEGC_HPP
