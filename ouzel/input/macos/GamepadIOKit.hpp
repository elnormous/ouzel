// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <unordered_map>
#include <IOKit/hid/IOHIDManager.h>

#include "input/Gamepad.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadIOKit: public Gamepad
        {
        public:
            explicit GamepadIOKit(IOHIDDeviceRef initDevice);

            inline IOHIDDeviceRef getDevice() const { return device; }

            void handleInput(IOHIDValueRef value);

        private:
            void handleThumbAxisChange(int64_t oldValue, int64_t newValue,
                                       int64_t min, int64_t max,
                                       GamepadButton negativeButton, GamepadButton positiveButton);

            IOHIDDeviceRef device = nullptr;

            struct Element
            {
                IOHIDElementRef element = nullptr;
                uint32_t type = 0;
                uint32_t usagePage = 0;
                uint32_t usage = 0;
                int64_t min = 0;
                int64_t max = 0;
                int64_t value = 0;
                GamepadButton button = GamepadButton::NONE;
            };

            std::unordered_map<IOHIDElementRef, Element> elements;

            IOHIDElementRef leftThumbX = nullptr;
            IOHIDElementRef leftThumbY = nullptr;
            IOHIDElementRef leftTrigger = nullptr;
            IOHIDElementRef rightThumbX = nullptr;
            IOHIDElementRef rightThumbY = nullptr;
            IOHIDElementRef rightTrigger = nullptr;
        };
    } // namespace input
} // namespace ouzel
