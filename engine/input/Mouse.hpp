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
                Unknown,
                Left, // Left mouse button
                Right, // Right mouse button
                Middle, // Middle mouse button (three-button mouse)
                X1, // First extra mouse button
                X2, // Second extra mouse button
                Count
            };

            Mouse(InputManager& initInputManager, DeviceId initDeviceId);

            inline auto& getPosition() const noexcept { return position; }
            void setPosition(const Vector2F& newPosition);
            inline auto isButtonDown(Button button) const
            {
                return buttonStates[static_cast<uint32_t>(button)];
            }
            inline auto isCursorVisible() const noexcept { return cursorVisible; }
            void setCursorVisible(bool visible);
            inline auto isCursorLocked() const noexcept { return cursorLocked; }
            void setCursorLocked(bool locked);
            inline auto getCursor() const noexcept { return cursor; }
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
            bool buttonStates[static_cast<uint32_t>(Button::Count)]{false};
            bool cursorVisible = true;
            bool cursorLocked = false;
            const Cursor* cursor = nullptr;
        };
    } // namespace input
} // namespace ouzel

#endif // OUZEL_INPUT_MOUSE_HPP
