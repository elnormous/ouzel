// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_INPUTSYSTEMWIN_HPP
#define OUZEL_INPUT_INPUTSYSTEMWIN_HPP

#include <memory>
#include <system_error>

#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Xinput.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

#include "../InputSystem.hpp"
#include "GamepadDeviceWin.hpp"
#include "KeyboardDeviceWin.hpp"
#include "MouseDeviceWin.hpp"

namespace ouzel::input::windows
{
    const std::error_category& getErrorCategory() noexcept;

    class GamepadDeviceDI;
    class GamepadDeviceXI;
    class Cursor;

    class InputSystem final: public input::InputSystem
    {
    public:
        explicit InputSystem(const std::function<std::future<bool>(const Event&)>& initCallback);
        ~InputSystem() override;

        auto getKeyboardDevice() const noexcept { return keyboardDevice.get(); }
        auto getMouseDevice() const noexcept { return mouseDevice.get(); }
        auto getTouchpadDevice() const noexcept { return touchpadDevice.get(); }

        void update();

        auto getDirectInput() const noexcept { return directInput; }
        void handleDeviceConnect(const DIDEVICEINSTANCEW* didInstance);

        void updateCursor() const;

    private:
        void executeCommand(const Command& command) final;

        auto getNextDeviceId() noexcept
        {
            ++lastDeviceId.value;
            return lastDeviceId;
        }

        bool discovering = false;

        DeviceId lastDeviceId;
        std::unique_ptr<KeyboardDevice> keyboardDevice;
        std::unique_ptr<MouseDevice> mouseDevice;
        std::unique_ptr<TouchpadDevice> touchpadDevice;

        IDirectInput8W* directInput = nullptr;
        std::vector<std::unique_ptr<GamepadDeviceDI>> gamepadsDI;
        std::unique_ptr<GamepadDeviceXI> gamepadsXI[XUSER_MAX_COUNT];

        std::vector<std::unique_ptr<Cursor>> cursors;

        HCURSOR defaultCursor = nullptr;
    };
}

#endif // OUZEL_INPUT_INPUTSYSTEMWIN_HPP
