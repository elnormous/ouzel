// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <windows.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "input/Gamepad.h"

namespace ouzel
{
    namespace input
    {
        class InputWin;

        class GamepadDI: public Gamepad
        {
            friend InputWin;
        public:
            bool update();

        protected:
            GamepadDI(const DIDEVICEINSTANCE* aInstance);

            const DIDEVICEINSTANCE* instance = nullptr;
            bool isXInputDevice = false;
            IDirectInputDevice8* device = nullptr;
        };
    } // namespace input
} // namespace ouzel
