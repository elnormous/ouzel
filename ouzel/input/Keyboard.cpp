// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include <iterator>
#include "Keyboard.hpp"

namespace ouzel
{
    namespace input
    {
        Keyboard::Keyboard():
            InputDevice(InputDevice::Type::KEYBOARD)
        {
            std::fill(std::begin(keyStates), std::end(keyStates), false);
        }
    } // namespace input
} // namespace ouzel
