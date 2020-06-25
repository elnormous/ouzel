// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <Windows.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")
#include "KeyboardDeviceWin.hpp"

namespace ouzel::input
{
    std::future<bool> KeyboardDeviceWin::handleKeyPress(Keyboard::Key key)
    {
        if (key == Keyboard::Key::leftShift) leftShiftDown = true;
        if (key == Keyboard::Key::rightShift) rightShiftDown = true;

        return KeyboardDevice::handleKeyPress(key);
    }

    void KeyboardDeviceWin::update()
    {
        if (leftShiftDown && (GetKeyState(VK_LSHIFT) & 0x8000) == 0)
        {
            leftShiftDown = false;
            handleKeyRelease(Keyboard::Key::leftShift);
        }

        if (rightShiftDown && (GetKeyState(VK_RSHIFT) & 0x8000) == 0)
        {
            rightShiftDown = false;
            handleKeyRelease(Keyboard::Key::rightShift);
        }
    }
}
