// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef INPUTSYSTEMTVOS_HPP
#define INPUTSYSTEMTVOS_HPP

#include <memory>
#include <unordered_map>
#if defined(__OBJC__)
#include <GameController/GameController.h>
typedef GCController* GCControllerPtr;
#else
#include <objc/objc.h>
#include <objc/NSObjCRuntime.h>
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
            explicit InputSystemTVOS(EventHandler& initEventHandler);
            ~InputSystemTVOS();

            void executeCommand(const Command& command) override;

            KeyboardDevice* getKeyboardDevice() const { return keyboardDevice.get(); }

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

#endif // INPUTSYSTEMTVOS_HPP
