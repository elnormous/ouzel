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
                    GAMEPAD_CONNECTED,
                    GAMEPAD_DISCONNECTED,
                    GAMEPAD_BUTTON_CHANGED,

                    KEY_PRESSED,
                    KEY_RELEASEED,
                    KEY_REPEATED,

                    MOUSE_PRESSED,
                    MOUSE_RELEASED,
                    MOUSE_SCROLLED,
                    MOUSE_MOVED,

                    TOUCH_BEGAN,
                    TOUCH_MOVED,
                    TOUCH_ENDED,
                    TOUCH_CANCELLED,
                };
            };

            InputSystem(InputManager& initInputManager);

        private:
            InputManager& inputManager;
        };
    } // namespace input
} // namespace ouzel
