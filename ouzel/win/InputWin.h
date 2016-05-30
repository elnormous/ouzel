// Copyright (C) 2016 Elviss Strazdins
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

            virtual ~InputWin();

            virtual void update() override;

            virtual void setCursorVisible(bool visible) override;
            virtual bool isCursorVisible() const override;

            static KeyboardKey convertKeyCode(WPARAM wParam);
            static uint32_t getKeyboardModifiers(WPARAM wParam);
            static uint32_t getMouseModifiers(WPARAM wParam);

        protected:
            InputWin();

            std::shared_ptr<GamepadWin> gamepads[XUSER_MAX_COUNT];

            bool cursorVisible = true;
        };
    } // namespace input
} // namespace ouzel
