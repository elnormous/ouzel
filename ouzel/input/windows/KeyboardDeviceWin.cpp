// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN
#undef NOMINMAX
#include "KeyboardDeviceWin.hpp"

namespace ouzel
{
    namespace input
    {
        std::future<bool> KeyboardDeviceWin::handleKeyPress(Keyboard::Key key)
        {
            if (key == Keyboard::Key::LEFT_SHIFT) leftShiftDown = true;
            if (key == Keyboard::Key::RIGHT_SHIFT) rightShiftDown = true;

            return KeyboardDevice::handleKeyPress(key);
        }

        void KeyboardDeviceWin::update()
        {
            if (leftShiftDown && (GetKeyState(VK_LSHIFT) & 0x8000) == 0)
            {
                leftShiftDown = false;
                handleKeyRelease(Keyboard::Key::LEFT_SHIFT);
            }

            if (rightShiftDown && (GetKeyState(VK_RSHIFT) & 0x8000) == 0)
            {
                rightShiftDown = false;
                handleKeyRelease(Keyboard::Key::RIGHT_SHIFT);
            }
        }
    } // namespace input
} // namespace ouzel
