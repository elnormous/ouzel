// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_INPUTSYSTEMLINUX_HPP
#define OUZEL_INPUT_INPUTSYSTEMLINUX_HPP

#include <memory>
#include <unordered_map>
#include "../../core/Setup.h"
#if OUZEL_SUPPORTS_X11
#  include <X11/X.h>
#endif
#include "../InputSystem.hpp"
#include "../Keyboard.hpp"
#include "EventDevice.hpp"
#include "KeyboardDeviceLinux.hpp"
#include "MouseDeviceLinux.hpp"

namespace ouzel
{
    namespace input
    {
        class CursorLinux;

        class InputSystemLinux final: public InputSystem
        {
        public:
            explicit InputSystemLinux(const std::function<std::future<bool>(const Event&)>& initCallback);
            ~InputSystemLinux() override;

            void executeCommand(const Command& command) final;

            auto getKeyboardDevice() const noexcept { return keyboardDevice.get(); }
            auto getMouseDevice() const noexcept { return mouseDevice.get(); }
            auto getTouchpadDevice() const noexcept { return touchpadDevice.get(); }

            auto getNextDeviceId() noexcept
            {
                ++lastDeviceId.value;
                return lastDeviceId;
            }

            void update();

        private:
#if OUZEL_SUPPORTS_X11
            void updateCursor() const;
#endif

            bool discovering = false;

            DeviceId lastDeviceId;
            std::unique_ptr<KeyboardDeviceLinux> keyboardDevice;
            std::unique_ptr<MouseDeviceLinux> mouseDevice;
            std::unique_ptr<TouchpadDevice> touchpadDevice;

            std::unordered_map<int, std::unique_ptr<EventDevice>> eventDevices;
            std::vector<std::unique_ptr<CursorLinux>> cursors;

#if OUZEL_SUPPORTS_X11
            ::Cursor emptyCursor = None;
#endif
        };
    }
}

#endif // OUZEL_INPUT_INPUTSYSTEMLINUX_HPP
