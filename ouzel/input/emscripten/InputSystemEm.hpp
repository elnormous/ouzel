// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <unordered_map>
#include <emscripten/html5.h>
#include "input/InputSystem.hpp"
#include "input/emscripten/MouseDeviceEm.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadDeviceEm;

        class InputSystemEm: public InputSystem
        {
        public:
            static Keyboard::Key convertKeyCode(const EM_UTF8 key[32]);
            static uint32_t getKeyboardModifiers(const EmscriptenKeyboardEvent* keyboardEvent);
            static uint32_t getMouseModifiers(const EmscriptenMouseEvent* mouseEvent);

            InputSystemEm();
            virtual ~InputSystemEm() {}

            virtual void executeCommand(Command command) override;

            KeyboardDevice* getKeyboardDevice() const { return keyboardDevice; }
            MouseDeviceEm* getMouseDevice() const { return mouseDevice; }
            TouchpadDevice* getTouchpadDevice() const { return touchpadDevice; }

            void update();

            void handleGamepadConnected(long device);
            void handleGamepadDisconnected(long device);

        private:
            uint32_t lastDeviceId = 0;
            KeyboardDevice* keyboardDevice = nullptr;
            MouseDeviceEm* mouseDevice = nullptr;
            TouchpadDevice* touchpadDevice = nullptr;
            std::unordered_map<long, GamepadDeviceEm*> gamepadDevices;
        };
    } // namespace input
} // namespace ouzel
