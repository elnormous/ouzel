// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_INPUTSYSTEMIOS_HPP
#define OUZEL_INPUT_INPUTSYSTEMIOS_HPP

#include <memory>
#include <unordered_map>
#if defined(__OBJC__)
#  include <GameController/GameController.h>
typedef GCController* GCControllerPtr;
#else
#  include <objc/objc.h>
#  include <objc/NSObjCRuntime.h>
typedef id GCControllerPtr;
#endif

#include "../InputSystem.hpp"

namespace ouzel::input::ios
{
    class GamepadDevice;

    class InputSystem final: public input::InputSystem
    {
    public:
        InputSystem(const std::function<std::future<bool>(const Event&)>& initCallback);
        ~InputSystem() override;

        auto getKeyboardDevice() const noexcept { return keyboardDevice.get(); }
        auto getTouchpadDevice() const noexcept { return touchpadDevice.get(); }

        void handleGamepadDiscoveryCompleted();
        void handleGamepadConnected(GCControllerPtr controller);
        void handleGamepadDisconnected(GCControllerPtr controller);

    private:
        void executeCommand(const Command& command) final;

        auto getNextDeviceId() noexcept
        {
            ++lastDeviceId.value;
            return lastDeviceId;
        }

        void startGamepadDiscovery();
        void stopGamepadDiscovery();

        void showVirtualKeyboard();
        void hideVirtualKeyboard();

        DeviceId lastDeviceId;
        std::unique_ptr<KeyboardDevice> keyboardDevice;
        std::unique_ptr<TouchpadDevice> touchpadDevice;
        std::unordered_map<GCControllerPtr, std::unique_ptr<GamepadDevice>> gamepadDevices;

        id connectDelegate = nil;
    };
}

#endif // OUZEL_INPUT_INPUTSYSTEMIOS_HPP
