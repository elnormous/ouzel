// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

namespace ouzel
{
    namespace input
    {
        class Input;

        class InputSystem
        {
        public:
            InputSystem(Input& initInput);

        private:
            Input& input;
        };
    } // namespace input
} // namespace ouzel
