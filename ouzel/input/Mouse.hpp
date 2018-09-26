// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include "input/InputDevice.hpp"
#include "math/Vector2.hpp"

namespace ouzel
{
    namespace input
    {
        class InputManager;

        class Mouse: public InputDevice
        {
            friend InputManager;
        public:
            enum class Button
            {
                NONE,
                LEFT,           // Left mouse button
                RIGHT,          // Right mouse button
                MIDDLE,         // Middle mouse button (three-button mouse)
                X1,             // Windows 2000/XP: X1 mouse button
                X2,             // Windows 2000/XP: X2 mouse button
                COUNT
            };

            Mouse(uint32_t initDeviceId);
            virtual ~Mouse() {}

            inline const Vector2& getPosition() const { return position; }
            void setPosition(const Vector2& newPosition);
            inline bool isButtonDown(Button button) const { return buttonStates[static_cast<uint32_t>(button)]; }

        protected:
            void handleMouseButtonPress(Mouse::Button button, const Vector2& pos, uint32_t modifiers);
            void handleMouseButtonRelease(Mouse::Button button, const Vector2& pos, uint32_t modifiers);
            void handleMouseMove(const Vector2& pos, uint32_t modifiers);
            void handleMouseRelativeMove(const Vector2& pos, uint32_t modifiers);
            void handleMouseScroll(const Vector2& scroll, const Vector2& pos, uint32_t modifiers);

        private:
            Vector2 position;
            bool buttonStates[static_cast<uint32_t>(Button::COUNT)];
        };
    } // namespace input
} // namespace ouzel

