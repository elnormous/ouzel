// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_INPUTSYSTEMEM_HPP
#define OUZEL_INPUT_INPUTSYSTEMEM_HPP

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

        class InputSystemEm final: public InputSystem
        {
        public:
            InputSystemEm(const std::function<std::future<bool>(const Event&)>& initCallback);
            ~InputSystemEm() {}

            void executeCommand(const Command& command) override;

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

#endif // OUZEL_INPUT_INPUTSYSTEMEM_HPP
