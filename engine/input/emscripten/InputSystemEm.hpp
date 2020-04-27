// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_INPUTSYSTEMEM_HPP
#define OUZEL_INPUT_INPUTSYSTEMEM_HPP

#include <memory>
#include <unordered_map>
#include <emscripten/html5.h>
#include "../InputSystem.hpp"
#include "GamepadDeviceEm.hpp"
#include "MouseDeviceEm.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadDeviceEm;

        class InputSystemEm final: public InputSystem
        {
        public:
            InputSystemEm(const std::function<std::future<bool>(const Event&)>& initCallback);

            void executeCommand(const Command& command) final;

            auto getKeyboardDevice() const noexcept { return keyboardDevice.get(); }
            auto getMouseDevice() const noexcept { return mouseDevice.get(); }
            auto getTouchpadDevice() const noexcept { return touchpadDevice.get(); }

            void update();

            void handleGamepadConnected(long device);
            void handleGamepadDisconnected(long device);

        private:
            auto getNextDeviceId() noexcept
            {
                ++lastDeviceId.value;
                return lastDeviceId;
            }

            DeviceId lastDeviceId;
            std::unique_ptr<KeyboardDevice> keyboardDevice;
            std::unique_ptr<MouseDeviceEm> mouseDevice;
            std::unique_ptr<TouchpadDevice> touchpadDevice;
            std::unordered_map<long, std::unique_ptr<GamepadDeviceEm>> gamepadDevices;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_INPUTSYSTEMEM_HPP
