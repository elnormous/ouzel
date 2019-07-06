// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_INPUT_MOUSE_HPP
#define OUZEL_INPUT_MOUSE_HPP

#include <cstdint>
#include "input/Controller.hpp"
#include "math/Vector.hpp"

namespace ouzel
{
    namespace input
    {
        class Cursor;

        class Mouse final: public Controller
        {
            friend InputManager;
        public:
            enum class Button
            {
                NONE,
                LEFT, // Left mouse button
                RIGHT, // Right mouse button
                MIDDLE, // Middle mouse button (three-button mouse)
                X1, // First extra mouse button
                X2, // Second extra mouse button
                COUNT
            };

            Mouse(InputManager& initInputManager, uint32_t initDeviceId);

            inline const Vector2F& getPosition() const { return position; }
            void setPosition(const Vector2F& newPosition);
            inline auto isButtonDown(Button button) const { return buttonStates[static_cast<uint32_t>(button)]; }
            inline auto isCursorVisible() const { return cursorVisible; }
            void setCursorVisible(bool visible);
            inline auto isCursorLocked() const { return cursorLocked; }
            void setCursorLocked(bool locked);
            inline auto getCursor() const { return cursor; }
            void setCursor(const Cursor* newCursor);

        protected:
            bool handleButtonPress(Mouse::Button button, const Vector2F& pos);
            bool handleButtonRelease(Mouse::Button button, const Vector2F& pos);
            bool handleMove(const Vector2F& pos);
            bool handleRelativeMove(const Vector2F& pos);
            bool handleScroll(const Vector2F& scroll, const Vector2F& pos);
            bool handleCursorLockChange(bool locked);

        private:
            Vector2F position;
            bool buttonStates[static_cast<uint32_t>(Button::COUNT)]{false};
            bool cursorVisible = true;
            bool cursorLocked = false;
            const Cursor* cursor = nullptr;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_MOUSE_HPP
