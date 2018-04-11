// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Xinput.h>
#include "input/Input.hpp"

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

            static KeyboardKey convertKeyCode(UINT keyCode);
            static uint32_t getModifiers(WPARAM wParam);

            void update();

            virtual void setCursorVisible(bool visible) override;
            virtual bool isCursorVisible() const override;

            virtual void setCursorLocked(bool locked) override;
            virtual bool isCursorLocked() const override;

            virtual void setCursorPosition(const Vector2& position) override;
            virtual void startGamepadDiscovery() override;

            IDirectInput8W* getDirectInput() const { return directInput; }
            void handleDeviceConnect(const DIDEVICEINSTANCEW* didInstance);

            void updateCursor();

        protected:
            InputWin();
            virtual bool init() override;

            virtual void activateCursorResource(CursorResource* resource) override;
            virtual CursorResource* createCursorResource() override;

            IDirectInput8W* directInput = nullptr;
            std::vector<GamepadDI*> gamepadsDI;
            GamepadXI* gamepadsXI[XUSER_MAX_COUNT];

            bool cursorVisible = true;
            bool cursorLocked = false;
            HCURSOR defaultCursor = nullptr;
            HCURSOR currentCursor = nullptr;
        };
    } // namespace input
} // namespace ouzel
