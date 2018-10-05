// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include "input/Controller.hpp"
#include "math/Vector2.hpp"

namespace ouzel
{
    namespace input
    {
        class Cursor;

        class Mouse: public Controller
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

            Mouse(InputManager& initInputManager, uint32_t initDeviceId);

            inline const Vector2& getPosition() const { return position; }
            void setPosition(const Vector2& newPosition);
            inline bool isButtonDown(Button button) const { return buttonStates[static_cast<uint32_t>(button)]; }
            bool isVisible() const { return visible; }
            void setVisible(bool newVisible);

        protected:
            void handleButtonPress(Mouse::Button button, const Vector2& pos, uint32_t modifiers);
            void handleButtonRelease(Mouse::Button button, const Vector2& pos, uint32_t modifiers);
            void handleMove(const Vector2& pos, uint32_t modifiers);
            void handleRelativeMove(const Vector2& pos, uint32_t modifiers);
            void handleScroll(const Vector2& scroll, const Vector2& pos, uint32_t modifiers);

        private:
            Vector2 position;
            bool buttonStates[static_cast<uint32_t>(Button::COUNT)];
            bool visible = true;
            bool locked = false;
        };
    } // namespace input
} // namespace ouzel

