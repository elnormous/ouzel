// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <memory>
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
        class NativeCursorWin;

        class InputSystemWin final: public InputSystem
        {
        public:
            explicit InputSystemWin(EventHandler& initEventHandler);
            ~InputSystemWin();

            void executeCommand(const Command& command) override;

            KeyboardDeviceWin* getKeyboardDevice() const { return keyboardDevice.get(); }
            MouseDeviceWin* getMouseDevice() const { return mouseDevice.get(); }
            TouchpadDevice* getTouchpadDevice() const { return touchpadDevice.get(); }

            void update();

            IDirectInput8W* getDirectInput() const { return directInput; }
            void handleDeviceConnect(const DIDEVICEINSTANCEW* didInstance);

        private:
            bool discovering = false;

            uint32_t lastDeviceId = 0;
            std::unique_ptr<KeyboardDeviceWin> keyboardDevice;
            std::unique_ptr<MouseDeviceWin> mouseDevice;
            std::unique_ptr<TouchpadDevice> touchpadDevice;

            IDirectInput8W* directInput = nullptr;
            std::vector<std::unique_ptr<GamepadDeviceDI>> gamepadsDI;
            std::unique_ptr<GamepadDeviceXI> gamepadsXI[XUSER_MAX_COUNT];

            std::vector<std::unique_ptr<NativeCursorWin>> cursors;
        };
    }
}
