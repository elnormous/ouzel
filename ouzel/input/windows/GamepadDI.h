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
            IDirectInputDevice8* getDevice() const { return device; }

        protected:
            GamepadDI(const DIDEVICEINSTANCE* aInstance);
            void checkThumbAxisChange(const DIJOYSTATE2& oldState, const DIJOYSTATE2& newState,
                                      size_t offset, int64_t min, int64_t max,
                                      GamepadButton negativeButton, GamepadButton positiveButton);

            const DIDEVICEINSTANCE* instance = nullptr;
            bool isXInputDevice = false;
            IDirectInputDevice8* device = nullptr;
            DIJOYSTATE2 diState;
            GamepadButton buttonMap[24];

            size_t leftThumbXMap = 0xFFFFFFFF;
            size_t leftThumbYMap = 0xFFFFFFFF;
            size_t leftTriggerMap = 0xFFFFFFFF;
            size_t rightThumbXMap = 0xFFFFFFFF;
            size_t rightThumbYMap = 0xFFFFFFFF;
            size_t rightTriggerMap = 0xFFFFFFFF;

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
