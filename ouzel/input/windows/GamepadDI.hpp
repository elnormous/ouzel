// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <Windows.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <hidusage.h>
#include "input/Gamepad.hpp"

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

            const DIDEVICEINSTANCEW* getInstance() const { return instance; }

            void handleObject(const DIDEVICEOBJECTINSTANCEW* didObjectInstance);

        protected:
            explicit GamepadDI(const DIDEVICEINSTANCEW* aInstance);
            bool checkInputBuffered();
            bool checkInputPolled();
            void checkThumbAxisChange(LONG oldValue, LONG newValue,
                                      int64_t min, int64_t max,
                                      GamepadButton negativeButton, GamepadButton positiveButton);
            void checkTriggerChange(LONG oldValue, LONG newValue,
                                    int64_t min, int64_t max,
                                    GamepadButton button);

            const DIDEVICEINSTANCEW* instance = nullptr;
            bool isXInputDevice = false;
            IDirectInputDevice8W* device = nullptr;
            DIJOYSTATE2 diState;
            GamepadButton buttonMap[24];

            struct Axis
            {
                USAGE usage = 0;
                size_t offset = 0xFFFFFFFF;
                LONG min = 0;
                LONG max = 0;
            };

            Axis leftThumbX;
            Axis leftThumbY;
            Axis leftTrigger;
            Axis rightThumbX;
            Axis rightThumbY;
            Axis rightTrigger;

            bool hasLeftTrigger = false;
            bool hasRightTrigger = false;
            bool buffered = true;
        };
    } // namespace input
} // namespace ouzel
