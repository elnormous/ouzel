// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_INPUTSYSTEMTVOS_HPP
#define OUZEL_INPUT_INPUTSYSTEMTVOS_HPP

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

#include "input/InputSystem.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadDeviceTVOS;

        class InputSystemTVOS final: public InputSystem
        {
        public:
            explicit InputSystemTVOS(const std::function<std::future<bool>(const Event&)>& initCallback);
            ~InputSystemTVOS();

            void executeCommand(const Command& command) final;

            inline auto getKeyboardDevice() const { return keyboardDevice.get(); }

            void handleGamepadDiscoveryCompleted();
            void handleGamepadConnected(GCControllerPtr controller);
            void handleGamepadDisconnected(GCControllerPtr controller);

        private:
            void startGamepadDiscovery();
            void stopGamepadDiscovery();

            void showVirtualKeyboard();
            void hideVirtualKeyboard();

            uint32_t lastDeviceId = 0;
            std::unique_ptr<KeyboardDevice> keyboardDevice;
            std::unordered_map<GCControllerPtr, std::unique_ptr<GamepadDeviceTVOS>> gamepadDevices;

            id connectDelegate = nil;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_INPUTSYSTEMTVOS_HPP
