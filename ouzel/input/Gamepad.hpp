// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include <string>
#include "input/InputDevice.hpp"

namespace ouzel
{
    namespace input
    {
        class InputManager;

        class Gamepad: public InputDevice
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
                FACE_BOTTOM, // A on Xbox controller, cross on PS controller
                FACE_RIGHT, // B on Xbox controller, circle on PS controller
                FACE_LEFT, // X on Xbox controller, square on PS controller
                FACE_TOP, // Y on Xbox controller, triangle on PS controller
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

                BUTTON_COUNT
            };

            enum class Motor
            {
                ALL,
                LEFT,
                RIGHT
            };

            struct ButtonState
            {
                bool pressed = false;
                float value = 0.0F;
            };

            Gamepad();
            virtual ~Gamepad();

            Gamepad(const Gamepad&) = delete;
            Gamepad& operator=(const Gamepad&) = delete;

            Gamepad(Gamepad&&) = delete;
            Gamepad& operator=(Gamepad&&) = delete;

            inline const std::string& getName() const { return name; }
            inline int32_t getVendorId() const { return vendorId; }
            inline int32_t getProductId() const { return productId; }
            inline bool isAttached() const { return attached; }

            virtual void setAbsoluteDpadValues(bool absoluteDpadValues);
            virtual bool isAbsoluteDpadValues() const;

            virtual int32_t getPlayerIndex() const;
            virtual void setPlayerIndex(int32_t playerIndex);

            virtual const ButtonState& getButtonState(Gamepad::Button button) const;

            virtual void setVibration(Motor motor, float speed);
            virtual float getVibration(Motor motor);

        protected:
            void handleButtonValueChange(Gamepad::Button button, bool pressed, float value);

            ButtonState buttonStates[static_cast<uint32_t>(Gamepad::Button::BUTTON_COUNT)];
            std::string name;
            int32_t vendorId = 0;
            int32_t productId = 0;
            bool attached = false;
        };
    } // namespace input
} // namespace ouzel
