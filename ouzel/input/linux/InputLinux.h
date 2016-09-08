// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "input/Input.h"

namespace ouzel
{
    class Engine;

    namespace input
    {
        class InputLinux: public Input
        {
            friend Engine;
        public:
            virtual ~InputLinux();
        protected:
            InputLinux();
        };
    } // namespace input
} // namespace ouzel
