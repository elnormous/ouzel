// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_INPUTSYSTEMLINUX_HPP
#define OUZEL_INPUT_INPUTSYSTEMLINUX_HPP

#include <memory>
#include <unordered_map>
#if OUZEL_SUPPORTS_X11
#  include <X11/X.h>
#endif
#include "input/InputSystem.hpp"
#include "input/Keyboard.hpp"
#include "input/linux/EventDevice.hpp"
#include "input/linux/KeyboardDeviceLinux.hpp"
#include "input/linux/MouseDeviceLinux.hpp"

namespace ouzel
{
    namespace input
    {
        class CursorLinux;

        class InputSystemLinux final: public InputSystem
        {
        public:
            explicit InputSystemLinux(const std::function<std::future<bool>(const Event&)>& initCallback);
            ~InputSystemLinux();

            void executeCommand(const Command& command) override;

            KeyboardDeviceLinux* getKeyboardDevice() const { return keyboardDevice.get(); }
            MouseDeviceLinux* getMouseDevice() const { return mouseDevice.get(); }
            TouchpadDevice* getTouchpadDevice() const { return touchpadDevice.get(); }

            uint32_t getNextDeviceId() { return ++lastDeviceId; }

            void update();

        private:
#if OUZEL_SUPPORTS_X11
            void updateCursor() const;
#endif

            bool discovering = false;

            uint32_t lastDeviceId = 0;
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
