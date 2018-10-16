// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <array>
#include <string>
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <Windows.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <hidusage.h>
#include "input/Gamepad.hpp"
#include "input/windows/GamepadDeviceWin.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadDeviceDI: public GamepadDeviceWin
        {
        public:
            GamepadDeviceDI(InputSystem& initInputSystem,
                            uint32_t initId,
                            const DIDEVICEINSTANCEW* aInstance, IDirectInput8W* directInput, HWND window);

            virtual ~GamepadDeviceDI();
            void update();

            const DIDEVICEINSTANCEW* getInstance() const { return instance; }

        protected:
            void checkInputBuffered();
            void checkInputPolled();
            void checkAxisChange(LONG oldValue, LONG newValue,
                                 int64_t min, int64_t range,
                                 Gamepad::Button negativeButton, Gamepad::Button positiveButton);

            std::string name;

            const DIDEVICEINSTANCEW* instance = nullptr;
            IDirectInputDevice8W* device = nullptr;
            DIJOYSTATE2 diState;

            struct Button
            {
                Gamepad::Button button = Gamepad::Button::NONE;
                DWORD offset;
            };

            Button buttons[24];

            struct Axis
            {
                Gamepad::Axis axis = Gamepad::Axis::NONE;
                DWORD offset;
                LONG min = 0;
                LONG max = 0;
                LONG range = 0;
                Gamepad::Button negativeButton = Gamepad::Button::NONE;
                Gamepad::Button positiveButton = Gamepad::Button::NONE;
            };

            Axis axis[6];

            bool hasLeftTrigger = false;
            bool hasRightTrigger = false;
            bool buffered = true;
        };
    } // namespace input
} // namespace ouzel
