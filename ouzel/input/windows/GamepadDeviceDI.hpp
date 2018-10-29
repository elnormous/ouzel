// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <string>
#include <unordered_map>
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
        class GamepadDeviceDI final: public GamepadDeviceWin
        {
        public:
            GamepadDeviceDI(InputSystem& initInputSystem,
                            uint32_t initId,
                            const DIDEVICEINSTANCEW* aInstance, IDirectInput8W* directInput, HWND window);

            ~GamepadDeviceDI();
            void update();

            const DIDEVICEINSTANCEW* getInstance() const { return instance; }

        protected:
            void checkInputBuffered();
            void checkInputPolled();
            void handleAxisChange(LONG oldValue, LONG newValue,
                                  LONG min, LONG range,
                                  Gamepad::Button negativeButton, Gamepad::Button positiveButton);

            std::string name;

            const DIDEVICEINSTANCEW* instance = nullptr;
            IDirectInputDevice8W* device = nullptr;
            DWORD hatValue = 0xffffffff;

            struct Button
            {
                Gamepad::Button button = Gamepad::Button::NONE;
                BYTE value = 0;
            };

            std::unordered_map<DWORD, Button> buttons;

            struct Axis
            {
                Gamepad::Axis axis = Gamepad::Axis::NONE;
                LONG min = 0;
                LONG max = 0;
                LONG range = 0;
                LONG value = 0;
                Gamepad::Button negativeButton = Gamepad::Button::NONE;
                Gamepad::Button positiveButton = Gamepad::Button::NONE;
            };

            std::unordered_map<DWORD, Axis> axes;

            bool hasLeftTrigger = false;
            bool hasRightTrigger = false;
            bool buffered = true;
        };
    } // namespace input
} // namespace ouzel
