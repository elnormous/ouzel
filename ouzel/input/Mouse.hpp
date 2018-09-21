// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class Mouse: public InputDevice
        {
        public:
            enum class Button
            {
                NONE,
                LEFT,           // Left mouse button
                RIGHT,          // Right mouse button
                MIDDLE,         // Middle mouse button (three-button mouse)
                X1,             // Windows 2000/XP: X1 mouse button
                X2,              // Windows 2000/XP: X2 mouse button

                BUTTON_COUNT
            };

            Mouse(): InputDevice(InputDevice::Type::MOUSE) {}
            virtual ~Mouse() {}
        };
    }
}
