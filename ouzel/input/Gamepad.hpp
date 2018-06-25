// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include "input/InputDevice.hpp"
#include "events/Event.hpp"

namespace ouzel
{
    namespace input
    {
        class Gamepad: public InputDevice
        {
        public:
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
            virtual bool setPlayerIndex(int32_t playerIndex);

            virtual const ButtonState& getButtonState(GamepadButton button) const;

            virtual void setVibration(Motor motor, float speed);
            virtual float getVibration(Motor motor);

        protected:
            void handleButtonValueChange(GamepadButton button, bool pressed, float value);

            ButtonState buttonStates[static_cast<uint32_t>(GamepadButton::BUTTON_COUNT)];
            std::string name;
            int32_t vendorId = 0;
            int32_t productId = 0;
            bool attached = false;
        };
    } // namespace input
} // namespace ouzel
