// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "KeyboardDeviceWin.hpp"

namespace ouzel
{
    namespace input
    {
        void KeyboardDeviceWin::update()
        {
            // TODO: implement
            /*if (keyboardKeyStates[static_cast<uint32_t>(Keyboard::Key::LEFT_SHIFT)] && (GetKeyState(VK_LSHIFT) & 0x8000) == 0)
                keyboardDevice->handleKeyRelease(Keyboard::Key::LEFT_SHIFT, 0);

            if (keyboardKeyStates[static_cast<uint32_t>(Keyboard::Key::RIGHT_SHIFT)] && (GetKeyState(VK_RSHIFT) & 0x8000) == 0)
                keyboardDevice->handleKeyRelease(Keyboard::Key::RIGHT_SHIFT, 0);*/
        }
    } // namespace input
} // namespace ouzel
