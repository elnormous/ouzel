// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <windows.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <hidusage.h>
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

            void handleObject(const DIDEVICEOBJECTINSTANCE* didObjectInstance);

        protected:
            GamepadDI(const DIDEVICEINSTANCE* aInstance);
            void checkThumbAxisChange(const DIJOYSTATE2& oldState, const DIJOYSTATE2& newState,
                                      size_t offset, int64_t min, int64_t max,
                                      GamepadButton negativeButton, GamepadButton positiveButton);
            void checkTriggerChange(const DIJOYSTATE2& oldState, const DIJOYSTATE2& newState,
                                    size_t offset, int64_t min, int64_t max,
                                    GamepadButton button);

            const DIDEVICEINSTANCE* instance = nullptr;
            bool isXInputDevice = false;
            IDirectInputDevice8* device = nullptr;
            DIJOYSTATE2 diState;
            GamepadButton buttonMap[24];

            USAGE leftThumbX = 0;
            USAGE leftThumbY = 0;
            USAGE leftTrigger = 0;
            USAGE rightThumbX = 0;
            USAGE rightThumbY = 0;
            USAGE rightTrigger = 0;

            bool hasLeftTrigger = false;
            bool hasRightTrigger = false;
            bool buffered = true;

            size_t leftThumbXOffset = 0xFFFFFFFF;
            size_t leftThumbYOffset = 0xFFFFFFFF;
            size_t leftTriggerOffset = 0xFFFFFFFF;
            size_t rightThumbXOffset = 0xFFFFFFFF;
            size_t rightThumbYOffset = 0xFFFFFFFF;
            size_t rightTriggerOffset = 0xFFFFFFFF;
        };
    } // namespace input
} // namespace ouzel
