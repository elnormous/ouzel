// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

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
