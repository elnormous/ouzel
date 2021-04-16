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
        void setPosition(const Vector<float, 2>& newPosition);
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

    private:
        bool handleButtonPress(Mouse::Button button, const Vector<float, 2>& pos);
        bool handleButtonRelease(Mouse::Button button, const Vector<float, 2>& pos);
        bool handleMove(const Vector<float, 2>& pos);
        bool handleRelativeMove(const Vector<float, 2>& pos);
        bool handleScroll(const Vector<float, 2>& scroll, const Vector<float, 2>& pos);
        bool handleCursorLockChange(bool locked);

        Vector<float, 2> position;
        bool buttonStates[static_cast<std::uint32_t>(Button::count)]{false};
        bool cursorVisible = true;
        bool cursorLocked = false;
        const Cursor* cursor = nullptr;
    };
}

#endif // OUZEL_INPUT_MOUSE_HPP
