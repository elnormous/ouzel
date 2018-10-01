// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

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

            void handleObject(const DIDEVICEOBJECTINSTANCEW* didObjectInstance);

        protected:
            void checkInputBuffered();
            void checkInputPolled();
            void checkThumbAxisChange(LONG oldValue, LONG newValue,
                                      int64_t min, int64_t max,
                                      Gamepad::Button negativeButton, Gamepad::Button positiveButton);
            void checkTriggerChange(LONG oldValue, LONG newValue,
                                    int64_t min, int64_t max,
                                    Gamepad::Button button);

            int32_t vendorId = 0;
            int32_t productId = 0;
            std::string name;

            const DIDEVICEINSTANCEW* instance = nullptr;
            IDirectInputDevice8W* device = nullptr;
            DIJOYSTATE2 diState;
            Gamepad::Button buttonMap[24];

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
