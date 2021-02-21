// Ouzel by Elviss Strazdins

#ifndef OUZEL_INPUT_GAMEPAD_HPP
#define OUZEL_INPUT_GAMEPAD_HPP

#include <cstdint>
#include <string>
#include "Controller.hpp"

namespace ouzel::input
{
    class InputManager;

    class Gamepad final: public Controller
    {
        friend InputManager;
    public:
        enum class Axis
        {
            none,
            leftThumbX,
            leftThumbY,
            rightThumbX,
            rightThumbY,
            leftTrigger,
            rightTrigger
        };

        enum class Button
        {
            none,
            dPadLeft,
            dPadRight,
            dPadUp,
            dPadDown,
            faceBottom, // A on Xbox controller, cross on PS controller, B on Nintendo controller
            faceRight, // B on Xbox controller, circle on PS controller, A on Nintendo controller
            faceLeft, // X on Xbox controller, square on PS controller, Y on Nintendo controller
            faceTop, // Y on Xbox controller, triangle on PS controller, X on Nintendo controller
            leftShoulder, // L1 for Apple and PS controller
            leftTrigger, // L2 for Apple and PS controller
            rightShoulder, // R1 for Apple and PS controller
            rightTrigger, // R2 for Apple and PS controller
            leftThumb,
            rightThumb,
            start,
            back,
            pause,
            leftThumbLeft,
            leftThumbRight,
            leftThumbUp,
            leftThumbDown,
            rightThumbLeft,
            rightThumbRight,
            rightThumbUp,
            rightThumbDown,
            count
        };

        enum class Motor
        {
            all,
            left,
            right,
            count
        };

        struct ButtonState final
        {
            bool pressed = false;
            float value = 0.0F;
        };

        Gamepad(InputManager& initInputManager, DeviceId initDeviceId);

        Gamepad(const Gamepad&) = delete;
        Gamepad& operator=(const Gamepad&) = delete;

        Gamepad(Gamepad&&) = delete;
        Gamepad& operator=(Gamepad&&) = delete;

        auto isAttached() const noexcept { return attached; }

        auto isAbsoluteDpadValues() const noexcept { return absoluteDpadValues; }
        void setAbsoluteDpadValues(bool newAbsoluteDpadValues);

        auto getPlayerIndex() const noexcept { return playerIndex; }
        void setPlayerIndex(std::int32_t newPlayerIndex);

        auto& getButtonState(Button button) const
        {
            return buttonStates[static_cast<std::uint32_t>(button)];
        }

        auto getVibration(Motor motor) { return vibration[static_cast<std::uint32_t>(motor)]; }
        void setVibration(Motor motor, float speed);

    protected:
        bool handleButtonValueChange(Gamepad::Button button, bool pressed, float value);

    private:
        ButtonState buttonStates[static_cast<std::uint32_t>(Button::count)];
        std::int32_t playerIndex = -1;
        bool absoluteDpadValues = false;
        bool attached = false;
        float vibration[static_cast<std::uint32_t>(Motor::count)]{};
    };
}

#endif // OUZEL_INPUT_GAMEPAD_HPP
