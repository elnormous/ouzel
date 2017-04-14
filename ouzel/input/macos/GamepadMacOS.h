// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#if defined(__OBJC__)
#import <IOKit/hid/IOHIDManager.h>
#else
typedef void* IOHIDDeviceRef;
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

            const GamepadButton* getButtonMap() const { return usageMap; }

        protected:
            GamepadMacOS(IOHIDDeviceRef aDevice);

            IOHIDDeviceRef device = nullptr;
            uint64_t vendorId = 0;
            uint64_t productId = 0;
            GamepadButton usageMap[24];
            uint32_t leftAnalogXMap = 0;
            uint32_t leftAnalogYMap = 0;
            uint32_t leftTriggerAnalogMap = 0;
            uint32_t rightAnalogXMap = 0;
            uint32_t rightAnalogYMap = 0;
            uint32_t rightTriggerAnalogMap = 0;
        };
    } // namespace input
} // namespace ouzel
