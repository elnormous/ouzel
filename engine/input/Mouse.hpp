// Ouzel by Elviss Strazdins

#ifndef OUZEL_INPUT_MOUSE_HPP
#define OUZEL_INPUT_MOUSE_HPP

#include <cstdint>
#include "Controller.hpp"
#include "../math/Vector.hpp"

namespace ouzel::input
{
    class Cursor;

    class Mouse final: public Controller
    {
        friend InputManager;
    public:
        enum class Button
        {
            none,
            left, // Left mouse button
            right, // Right mouse button
            middle, // Middle mouse button (three-button mouse)
            x1, // First extra mouse button
            x2, // Second extra mouse button
            count
        };

        Mouse(InputManager& initInputManager, DeviceId initDeviceId);

        auto& getPosition() const noexcept { return position; }
        void setPosition(const Vector2F& newPosition);
        auto isButtonDown(Button button) const
        {
            return buttonStates[static_cast<std::uint32_t>(button)];
        }
        auto isCursorVisible() const noexcept { return cursorVisible; }
        void setCursorVisible(bool visible);
        auto isCursorLocked() const noexcept { return cursorLocked; }
        void setCursorLocked(bool locked);
        auto getCursor() const noexcept { return cursor; }
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
        bool buttonStates[static_cast<std::uint32_t>(Button::count)]{false};
        bool cursorVisible = true;
        bool cursorLocked = false;
        const Cursor* cursor = nullptr;
    };
}

#endif // OUZEL_INPUT_MOUSE_HPP
