// Ouzel by Elviss Strazdins

#ifndef OUZEL_INPUT_INPUTSYSTEMEM_HPP
#define OUZEL_INPUT_INPUTSYSTEMEM_HPP

#include <memory>
#include <unordered_map>
#include <emscripten/html5.h>
#include "../InputSystem.hpp"
#include "GamepadDeviceEm.hpp"
#include "MouseDeviceEm.hpp"

namespace ouzel::input::emscripten
{
    class InputSystem final: public input::InputSystem
    {
    public:
        InputSystem(const std::function<std::future<bool>(const Event&)>& initCallback);

        auto getKeyboardDevice() const noexcept { return keyboardDevice.get(); }
        auto getMouseDevice() const noexcept { return mouseDevice.get(); }
        auto getTouchpadDevice() const noexcept { return touchpadDevice.get(); }

        void update();

        void handleGamepadConnected(long device);
        void handleGamepadDisconnected(long device);

    private:
        void executeCommand(const Command& command) final;

        auto getNextDeviceId() noexcept
        {
            ++lastDeviceId.value;
            return lastDeviceId;
        }

        DeviceId lastDeviceId;
        std::unique_ptr<KeyboardDevice> keyboardDevice;
        std::unique_ptr<MouseDevice> mouseDevice;
        std::unique_ptr<TouchpadDevice> touchpadDevice;
        std::unordered_map<long, std::unique_ptr<GamepadDevice>> gamepadDevices;
    };
}

#endif // OUZEL_INPUT_INPUTSYSTEMEM_HPP
