// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include "utils/Noncopyable.h"
#include "events/Event.h"

namespace ouzel
{
    namespace input
    {
        class Gamepad: public Noncopyable
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
                float value = 0.0f;
            };

            virtual ~Gamepad();

            const std::string& getName() const { return name; }
            uint64_t getVendorId() const { return vendorId; }
            uint64_t getProductId() const { return productId; }
            bool isAttached() const { return attached; }
            
            virtual void setAbsoluteDpadValues(bool absoluteDpadValues);
            virtual bool isAbsoluteDpadValues() const;

            virtual int32_t getPlayerIndex() const;
            virtual bool setPlayerIndex(int32_t playerIndex);

            virtual const ButtonState& getButtonState(GamepadButton button) const;

            virtual void setVibration(Motor motor, float speed);
            virtual float getVibration(Motor motor);

        protected:
            Gamepad();

            void handleButtonValueChange(GamepadButton button, bool pressed, float value);

            ButtonState buttonStates[static_cast<uint32_t>(GamepadButton::BUTTON_COUNT)];
            std::string name;
            int32_t vendorId = 0;
            int32_t productId = 0;
            bool attached = false;
        };
    } // namespace input
} // namespace ouzel
