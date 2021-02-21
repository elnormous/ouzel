// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

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
#include "InputSystemWin.hpp"

namespace ouzel::input::windows
{
    KeyboardDevice::KeyboardDevice(InputSystem& initInputSystem,
                                   DeviceId initId):
        input::KeyboardDevice(initInputSystem, initId)
    {
    }

    std::future<bool> KeyboardDevice::handleKeyPress(Keyboard::Key key)
    {
        if (key == Keyboard::Key::leftShift) leftShiftDown = true;
        if (key == Keyboard::Key::rightShift) rightShiftDown = true;

        return input::KeyboardDevice::handleKeyPress(key);
    }

    void KeyboardDevice::update()
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
