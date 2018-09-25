// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "input/InputDevice.hpp"
#include "math/Vector2.hpp"

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

            Mouse();
            virtual ~Mouse() {}

            inline const Vector2& getPosition() const { return position; }
            inline bool isButtonDown(Button button) const { return buttonStates[static_cast<uint32_t>(button)]; }

        private:
            Vector2 position;
            bool buttonStates[static_cast<uint32_t>(Button::BUTTON_COUNT)];
        };
    }
}
