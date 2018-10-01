// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Xinput.h>
#include "input/InputSystem.hpp"
#include "input/windows/GamepadDeviceWin.hpp"
#include "input/windows/MouseDeviceWin.hpp"

namespace ouzel
{
    namespace input
    {
        class InputSystemWin: public InputSystem
        {
        public:
            virtual ~InputSystemWin() {}

            static Keyboard::Key convertKeyCode(UINT keyCode);
            static uint32_t getModifiers(WPARAM wParam);

            InputSystemWin();

            KeyboardDevice* getKeyboardDevice() const { return keyboardDevice; }
            MouseDevice* getMouseDevice() const { return mouseDevice; }
            TouchpadDevice* getTouchpadDevice() const { return touchpadDevice; }

        private:
            uint32_t lastDeviceId = 0;
            KeyboardDevice* keyboardDevice = nullptr;
            MouseDeviceWin* mouseDevice = nullptr;
            TouchpadDevice* touchpadDevice = nullptr;
        };
    }
}
