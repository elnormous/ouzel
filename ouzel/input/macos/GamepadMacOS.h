// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <map>

#if defined(__OBJC__)
#import <IOKit/hid/IOHIDManager.h>
#else
typedef void* IOHIDDeviceRef;
typedef void* IOHIDElementRef;
#endif

#include "input/Gamepad.h"

namespace ouzel
{
    namespace input
    {
        class InputMacOS;

        class GamepadMacOS: public Gamepad
        {
            friend InputMacOS;
        public:
            IOHIDDeviceRef getDevice() const { return device; }

            void handleInput(IOHIDValueRef value);

        protected:
            GamepadMacOS(IOHIDDeviceRef aDevice);
            void handleThumbAxisChange(int64_t oldValue, int64_t newValue,
                                       int64_t min, int64_t max,
                                       GamepadButton negativeButton, GamepadButton positiveButton);

            IOHIDDeviceRef device = nullptr;
            GamepadButton usageMap[24];
            uint32_t leftThumbXMap = 0;
            uint32_t leftThumbYMap = 0;
            uint32_t leftTriggerMap = 0;
            uint32_t rightThumbXMap = 0;
            uint32_t rightThumbYMap = 0;
            uint32_t rightTriggerMap = 0;

            struct Element
            {
                IOHIDElementRef element = nullptr;
                uint32_t type = 0;
                uint32_t usagePage = 0;
                uint32_t usage = 0;
                int64_t min = 0;
                int64_t max = 0;
                int64_t value = 0;
            };

            std::map<IOHIDElementRef, Element> elements;
        };
    } // namespace input
} // namespace ouzel
