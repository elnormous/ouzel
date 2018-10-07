// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"
#include <unordered_map>
#if OUZEL_SUPPORTS_X11
#  include <X11/keysym.h>
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
        class InputSystemLinux: public InputSystem
        {
        public:
#if OUZEL_SUPPORTS_X11
            static Keyboard::Key convertKeyCode(KeySym keyCode);
            static uint32_t getModifiers(unsigned int state);
#else
            static Keyboard::Key convertKeyCode(uint16_t keyCode);
#endif
            uint32_t getModifiers() const;

            InputSystemLinux();
            virtual ~InputSystemLinux();

            virtual void executeCommand(Command command) override;

            KeyboardDeviceLinux* getKeyboardDevice() const { return keyboardDevice; }
            MouseDeviceLinux* getMouseDevice() const { return mouseDevice; }
            TouchpadDevice* getTouchpadDevice() const { return touchpadDevice; }

            void update();

        private:
            void discoverDevices();

            int maxFd = 0;
            std::unordered_map<int, EventDevice> eventDevices;

            uint32_t lastDeviceId = 0;
            KeyboardDeviceLinux* keyboardDevice = nullptr;
            MouseDeviceLinux* mouseDevice = nullptr;
            TouchpadDevice* touchpadDevice = nullptr;
        };
    }
}
