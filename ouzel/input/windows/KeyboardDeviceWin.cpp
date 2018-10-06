// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <Windows.h>
#include "KeyboardDeviceWin.hpp"

namespace ouzel
{
    namespace input
    {
        void KeyboardDeviceWin::handleKeyPress(Keyboard::Key key, uint32_t modifiers)
        {
            if (key == Keyboard::Key::LEFT_SHIFT) leftShiftDown = true;
            if (key == Keyboard::Key::RIGHT_SHIFT) rightShiftDown = true;

            KeyboardDevice::handleKeyPress(key, modifiers);
        }

        void KeyboardDeviceWin::update()
        {
            if (leftShiftDown && (GetKeyState(VK_LSHIFT) & 0x8000) == 0)
                handleKeyRelease(Keyboard::Key::LEFT_SHIFT, 0);

            if (rightShiftDown && (GetKeyState(VK_RSHIFT) & 0x8000) == 0)
                handleKeyRelease(Keyboard::Key::RIGHT_SHIFT, 0);
        }
    } // namespace input
} // namespace ouzel
