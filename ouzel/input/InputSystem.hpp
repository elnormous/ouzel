// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <cstdint>
#include "input/InputDevice.hpp"
#include "math/Vector2.hpp"

namespace ouzel
{
    namespace input
    {
        class InputDevice;

        class InputSystem
        {
        public:
            struct Event
            {
                enum class Type
                {
                    GAMEPAD_CONNECT,
                    GAMEPAD_DISCONNECT,
                    GAMEPAD_BUTTON_CHANGE,

                    KEY_PRESS,
                    KEY_RELEASE,
                    KEY_REPEAT,

                    MOUSE_PRESS,
                    MOUSE_RELEASE,
                    MOUSE_SCROLL,
                    MOUSE_MOVE,

                    TOUCH_BEGIN,
                    TOUCH_MOVE,
                    TOUCH_END,
                    TOUCH_CANCEL
                };

                Type type;

                input::InputDevice* inputDevice = nullptr;
                union
                {
                    input::KeyboardKey keyboardKey;
                    input::MouseButton mouseButton;
                    input::GamepadButton gamepadButton;
                    uint64_t touchId;
                };
                bool pressed = false;
                bool previousPressed = false;
                float value = 0.0F; // force for touch events
                float previousValue = 0.0F;
                Vector2 difference;
                Vector2 position;
            };

            InputSystem();
        };
    } // namespace input
} // namespace ouzel
