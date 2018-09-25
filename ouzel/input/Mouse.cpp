// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "Mouse.hpp"

namespace ouzel
{
    namespace input
    {
        Mouse::Mouse():
            InputDevice(InputDevice::Type::MOUSE)
        {
            std::fill(std::begin(buttonStates), std::end(buttonStates), false);
        }
    } // namespace input
} // namespace ouzel
