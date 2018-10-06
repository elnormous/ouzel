// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Xinput.h>
#include "input/InputSystem.hpp"
#include "input/windows/GamepadDeviceWin.hpp"
#include "input/windows/KeyboardDeviceWin.hpp"
#include "input/windows/MouseDeviceWin.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadDeviceDI;
        class GamepadDeviceXI;

        class InputSystemWin: public InputSystem
        {
        public:
            static Keyboard::Key convertKeyCode(UINT keyCode);
            static uint32_t getModifiers(WPARAM wParam);

            InputSystemWin();
            virtual ~InputSystemWin();

            virtual void executeCommand(Command command) override;

            KeyboardDeviceWin* getKeyboardDevice() const { return keyboardDevice; }
            MouseDeviceWin* getMouseDevice() const { return mouseDevice; }
            TouchpadDevice* getTouchpadDevice() const { return touchpadDevice; }

            void update();

            IDirectInput8W* getDirectInput() const { return directInput; }
            void handleDeviceConnect(const DIDEVICEINSTANCEW* didInstance);

        private:
            void discoverGamepads();

            IDirectInput8W* directInput = nullptr;
            std::vector<GamepadDeviceDI*> gamepadsDI;
            GamepadDeviceXI* gamepadsXI[XUSER_MAX_COUNT];

            uint32_t lastDeviceId = 0;
            KeyboardDeviceWin* keyboardDevice = nullptr;
            MouseDeviceWin* mouseDevice = nullptr;
            TouchpadDevice* touchpadDevice = nullptr;
        };
    }
}
