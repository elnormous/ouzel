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
            InputSystem(InputManager& initInputManager);

        private:
            InputManager& inputManager;
        };
    } // namespace input
} // namespace ouzel
