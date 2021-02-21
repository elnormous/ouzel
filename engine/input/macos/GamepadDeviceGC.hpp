// Ouzel by Elviss Strazdins

#ifndef OUZEL_INPUT_GAMEPADDEVICEGC_HPP
#define OUZEL_INPUT_GAMEPADDEVICEGC_HPP

#if defined(__OBJC__)
#  include <GameController/GameController.h>
typedef GCController* GCControllerPtr;
#else
#  include <objc/objc.h>
typedef id GCControllerPtr;
#endif

#include "GamepadDeviceMacOS.hpp"
#include "../Gamepad.hpp"

namespace ouzel::input::macos
{
    class GamepadDeviceGC final: public GamepadDevice
    {
    public:
        GamepadDeviceGC(InputSystem& initInputSystem,
                        DeviceId initId,
                        GCControllerPtr initController);

        void setAbsoluteDpadValues(bool absoluteDpadValues);
        bool isAbsoluteDpadValues() const;

        std::int32_t getPlayerIndex() const;
        void setPlayerIndex(std::int32_t playerIndex) final;

        auto getController() const noexcept { return controller; }

    private:
        GCControllerPtr controller = nil;
        bool attached = false;
    };
}

#endif // OUZEL_INPUT_GAMEPADDEVICEGC_HPP
