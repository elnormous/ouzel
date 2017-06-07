// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <xinput.h>
#include "input/Input.h"

namespace ouzel
{
    namespace input
    {
        class GamepadDI;
        class GamepadXI;

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

            IDirectInput8* getDirectInput() const { return directInput; }
            void handleDeviceConnect(const DIDEVICEINSTANCE* didInstance);

            void updateCursor();

        protected:
            InputWin();
            virtual bool init() override;

            virtual void activateCursorResource(CursorResource* resource) override;
            virtual CursorResource* createCursorResource() override;

            IDirectInput8* directInput = nullptr;
            std::vector<GamepadDI*> gamepadsDI;
            GamepadXI* gamepadsXI[XUSER_MAX_COUNT];

            bool cursorVisible = true;
            bool cursorLocked = false;
            HCURSOR defaultCursor = nullptr;
            HCURSOR currentCursor = nullptr;
        };
    } // namespace input
} // namespace ouzel
