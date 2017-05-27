// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <emscripten.h>
#include "GamepadEm.h"

namespace ouzel
{
    namespace input
    {
        GamepadEm::GamepadEm(long aIndex):
            index(aIndex)
        {
        }

        void GamepadEm::update()
        {
        }
    } // namespace input
} // namespace ouzel
