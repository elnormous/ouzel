// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <string>
#include "input/Controller.hpp"

namespace ouzel
{
    namespace input
    {
        class InputManager;

        class Gamepad: public Controller
        {
            friend InputManager;
        public:
            enum class Button
            {
                NONE,
                DPAD_LEFT,
                DPAD_RIGHT,
                DPAD_UP,
                DPAD_DOWN,
                FACE_1, // bottom face button, A on Xbox controller, cross on PS controller
                FACE_2, // left face button, X on Xbox controller, square on PS controller
                FACE_3, // right face button, B on Xbox controller, circle on PS controller
                FACE_4, // top face button, Y on Xbox controller, triangle on PS controller
                FACE_5, // right most face button
                FACE_6, // top most face button
                LEFT_SHOULDER,  // L1 for Apple and PS controller
                LEFT_TRIGGER,   // L2 for Apple and PS controller
                RIGHT_SHOULDER, // R1 for Apple and PS controller
                RIGHT_TRIGGER,  // R2 for Apple and PS controller
                LEFT_THUMB,
                RIGHT_THUMB,
                START,
                BACK,
                PAUSE,
                LEFT_THUMB_LEFT,
                LEFT_THUMB_RIGHT,
                LEFT_THUMB_UP,
                LEFT_THUMB_DOWN,
                RIGHT_THUMB_LEFT,
                RIGHT_THUMB_RIGHT,
                RIGHT_THUMB_UP,
                RIGHT_THUMB_DOWN,
                COUNT
            };

            enum class Motor
            {
                ALL,
                LEFT,
                RIGHT,
                COUNT
            };

            struct ButtonState
            {
                bool pressed = false;
                float value = 0.0F;
            };

            Gamepad(uint32_t initDeviceId);
            virtual ~Gamepad() {}

            Gamepad(const Gamepad&) = delete;
            Gamepad& operator=(const Gamepad&) = delete;

            Gamepad(Gamepad&&) = delete;
            Gamepad& operator=(Gamepad&&) = delete;

            inline bool isAttached() const { return attached; }

            inline bool isAbsoluteDPadValues() const { return absoluteDPadValues; }
            void setAbsoluteDPadValues(bool newAbsoluteDPadValues);

            inline int32_t getPlayerIndex() const { return playerIndex; }
            void setPlayerIndex(int32_t newPlayerIndex);

            inline const ButtonState& getButtonState(Button button) const
            {
                return buttonStates[static_cast<uint32_t>(button)];
            }

            inline float getVibration(Motor motor)  { return vibration[static_cast<uint32_t>(motor)]; }
            void setVibration(Motor motor, float speed);

        protected:
            void handleButtonValueChange(Gamepad::Button button, bool pressed, float value);

        private:
            ButtonState buttonStates[static_cast<uint32_t>(Button::COUNT)];
            int32_t playerIndex = -1;
            bool absoluteDPadValues = false;
            bool attached = false;
            float vibration[static_cast<uint32_t>(Motor::COUNT)];
        };
    } // namespace input
} // namespace ouzel
