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
            virtual ~GamepadDI();
            bool update();

            const DIDEVICEINSTANCE* getInstance() const { return instance; }

        protected:
            GamepadDI(const DIDEVICEINSTANCE* aInstance);

            const DIDEVICEINSTANCE* instance = nullptr;
            bool isXInputDevice = false;
            IDirectInputDevice8* device = nullptr;
            DIJOYSTATE2 diState;

            struct State
            {
                bool dpadLeft = false;
                bool dpadRight = false;
                bool dpadUp = false;
                bool dpadDown = false;
            };

            State state;
        };
    } // namespace input
} // namespace ouzel
