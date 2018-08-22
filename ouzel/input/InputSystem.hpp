// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

namespace ouzel
{
    namespace input
    {
        class InputManager;

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
            };

            InputSystem(InputManager& initInputManager);

        private:
            InputManager& inputManager;
        };
    } // namespace input
} // namespace ouzel
