// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <memory>
#include <unordered_map>
#include <emscripten/html5.h>
#include "input/InputSystem.hpp"
#include "input/emscripten/GamepadDeviceEm.hpp"
#include "input/emscripten/MouseDeviceEm.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadDeviceEm;

        class InputSystemEm: public InputSystem
        {
        public:
            static Keyboard::Key convertKeyCode(const EM_UTF8 key[32]);

            InputSystemEm(EventHandler& initEventHandler);
            virtual ~InputSystemEm() {}

            virtual void executeCommand(const Command& command) override;

            KeyboardDevice* getKeyboardDevice() const { return keyboardDevice.get(); }
            MouseDeviceEm* getMouseDevice() const { return mouseDevice.get(); }
            TouchpadDevice* getTouchpadDevice() const { return touchpadDevice.get(); }

            void update();

            void handleGamepadConnected(long device);
            void handleGamepadDisconnected(long device);

        private:
            uint32_t lastDeviceId = 0;
            std::unique_ptr<KeyboardDevice> keyboardDevice;
            std::unique_ptr<MouseDeviceEm> mouseDevice;
            std::unique_ptr<TouchpadDevice> touchpadDevice;
            std::unordered_map<long, std::unique_ptr<GamepadDeviceEm>> gamepadDevices;
        };
    } // namespace input
} // namespace ouzel
