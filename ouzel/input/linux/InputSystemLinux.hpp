// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <memory>
#include <unordered_map>
#include "input/InputSystem.hpp"
#include "input/Keyboard.hpp"
#include "input/linux/EventDevice.hpp"
#include "input/linux/KeyboardDeviceLinux.hpp"
#include "input/linux/MouseDeviceLinux.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemLinux: public InputSystem
        {
        public:
            InputSystemLinux(EventHandler& initEventHandler);
            virtual ~InputSystemLinux();

            virtual void executeCommand(const Command& command) override;

            KeyboardDeviceLinux* getKeyboardDevice() const { return keyboardDevice.get(); }
            MouseDeviceLinux* getMouseDevice() const { return mouseDevice.get(); }
            TouchpadDevice* getTouchpadDevice() const { return touchpadDevice.get(); }

            uint32_t getNextDeviceId() { return ++lastDeviceId; }

            void update();

        private:
            bool discovering = false;

            uint32_t lastDeviceId = 0;
            std::unique_ptr<KeyboardDeviceLinux> keyboardDevice;
            std::unique_ptr<MouseDeviceLinux> mouseDevice;
            std::unique_ptr<TouchpadDevice> touchpadDevice;

            std::unordered_map<int, std::unique_ptr<EventDevice>> eventDevices;
        };
    }
}
