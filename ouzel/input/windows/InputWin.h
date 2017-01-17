// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
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
            static KeyboardKey convertKeyCode(WPARAM wParam);
            static uint32_t getKeyboardModifiers(WPARAM wParam);
            static uint32_t getMouseModifiers(WPARAM wParam);

            void update();

            virtual void setCursorVisible(bool visible) override;
            virtual bool isCursorVisible() const override;

            virtual void setCursorPosition(const Vector2& position) override;

        protected:
            InputWin();

            GamepadWin* gamepadsWin[XUSER_MAX_COUNT];

            bool cursorVisible = true;
        };
    } // namespace input
} // namespace ouzel
