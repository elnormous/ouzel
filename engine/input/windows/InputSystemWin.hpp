// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

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

#include "input/InputSystem.hpp"
#include "input/windows/GamepadDeviceWin.hpp"
#include "input/windows/KeyboardDeviceWin.hpp"
#include "input/windows/MouseDeviceWin.hpp"

namespace ouzel
{
    namespace input
    {
        const std::error_category& getErrorCategory() noexcept;

        class GamepadDeviceDI;
        class GamepadDeviceXI;
        class CursorWin;

        class InputSystemWin final: public InputSystem
        {
        public:
            explicit InputSystemWin(const std::function<std::future<bool>(const Event&)>& initCallback);
            ~InputSystemWin() override;

            void executeCommand(const Command& command) final;

            inline auto getKeyboardDevice() const noexcept { return keyboardDevice.get(); }
            inline auto getMouseDevice() const noexcept { return mouseDevice.get(); }
            inline auto getTouchpadDevice() const noexcept { return touchpadDevice.get(); }

            void update();

            inline auto getDirectInput() const noexcept { return directInput; }
            void handleDeviceConnect(const DIDEVICEINSTANCEW* didInstance);

            void updateCursor() const;

        private:
            inline auto getNextDeviceId() noexcept
            {
                ++lastDeviceId.value;
                return lastDeviceId;
            }

            bool discovering = false;

            DeviceId lastDeviceId;
            std::unique_ptr<KeyboardDeviceWin> keyboardDevice;
            std::unique_ptr<MouseDeviceWin> mouseDevice;
            std::unique_ptr<TouchpadDevice> touchpadDevice;

            IDirectInput8W* directInput = nullptr;
            std::vector<std::unique_ptr<GamepadDeviceDI>> gamepadsDI;
            std::unique_ptr<GamepadDeviceXI> gamepadsXI[XUSER_MAX_COUNT];

            std::vector<std::unique_ptr<CursorWin>> cursors;

            HCURSOR defaultCursor = nullptr;
        };
    }
}

#endif // OUZEL_INPUT_INPUTSYSTEMWIN_HPP
