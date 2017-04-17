// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <memory>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <xinput.h>
#include "input/Input.h"

namespace ouzel
{
    namespace input
    {
        class GamepadWin;

        class InputWin : public Input
        {
            friend Engine;
        public:
            virtual ~InputWin();

            static KeyboardKey convertKeyCode(WPARAM wParam);
            static uint32_t getKeyboardModifiers(WPARAM wParam);
            static uint32_t getMouseModifiers(WPARAM wParam);

            void update();

            virtual void setCursorVisible(bool visible) override;
            virtual bool isCursorVisible() const override;

            virtual void setCursorLocked(bool locked) override;
            virtual bool isCursorLocked() const override;

            virtual void setCursorPosition(const Vector2& position) override;
            virtual void startGamepadDiscovery() override;

            void handleDeviceConnect(const DIDEVICEINSTANCE* didInstance);

        protected:
            InputWin();
            virtual bool init() override;

            IDirectInput* directInput = nullptr;
            GamepadWin* gamepadsWin[XUSER_MAX_COUNT];

            bool cursorVisible = true;
            bool cursorLocked = false;
        };
    } // namespace input
} // namespace ouzel
