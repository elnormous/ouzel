// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <unordered_map>
#include <IOKit/hid/IOHIDManager.h>

#include "input/macos/GamepadDeviceMacOS.hpp"
#include "input/Gamepad.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadDeviceIOKit: public GamepadDeviceMacOS
        {
        public:
            GamepadDeviceIOKit(InputSystem& initInputSystem,
                               uint32_t initId,
                               IOHIDDeviceRef initDevice);

            inline IOHIDDeviceRef getDevice() const { return device; }

            void handleInput(IOHIDValueRef value);

        private:
            void handleAxisChange(int64_t oldValue, int64_t newValue,
                                  int64_t min, int64_t range,
                                  Gamepad::Button negativeButton,
                                  Gamepad::Button positiveButton);

            IOHIDDeviceRef device = nullptr;

            struct Element
            {
                IOHIDElementRef element = nullptr;
                uint32_t type = 0;
                uint32_t usagePage = 0;
                uint32_t usage = 0;
                int64_t min = 0;
                int64_t max = 0;
                int64_t range = 0;
                int64_t value = 0;
                Gamepad::Button button = Gamepad::Button::NONE;
            };

            std::unordered_map<IOHIDElementRef, Element> elements;

            IOHIDElementRef leftThumbX = nullptr;
            IOHIDElementRef leftThumbY = nullptr;
            IOHIDElementRef rightThumbX = nullptr;
            IOHIDElementRef rightThumbY = nullptr;
            IOHIDElementRef leftTrigger = nullptr;
            IOHIDElementRef rightTrigger = nullptr;
        };
    } // namespace input
} // namespace ouzel
