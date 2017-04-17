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

            const GamepadButton* getButtonMap() const { return usageMap; }

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
                uint32_t type;
                uint32_t usagePage;
                uint32_t usage;
                int64_t min;
                int64_t max;
            };

            std::map<IOHIDElementRef, Element> elements;

            enum
            {
                STATE_DPAD_LEFT,
                STATE_DPAD_RIGHT,
                STATE_DPAD_UP,
                STATE_DPAD_DOWN,
                STATE_LEFT_THUMB_X,
                STATE_LEFT_THUMB_Y,
                STATE_RIGHT_THUMB_X,
                STATE_RIGHT_THUMB_Y,
            };

            int64_t states[8];
        };
    } // namespace input
} // namespace ouzel
