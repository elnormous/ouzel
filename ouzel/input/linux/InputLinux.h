// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <X11/keysym.h>
#include <X11/X.h>
#include "input/Input.h"

namespace ouzel
{
    class Engine;

    namespace input
    {
        class InputLinux: public Input
        {
            friend Engine;
        public:
            static KeyboardKey convertKeyCode(KeySym keyCode);
            static uint32_t getModifiers(unsigned int state);

            virtual ~InputLinux();

            virtual void setCursorVisible(bool visible) override;
            virtual bool isCursorVisible() const override;

            virtual void setCursorLocked(bool locked) override;
            virtual bool isCursorLocked() const override;

            virtual void setCursorPosition(const Vector2& position) override;

        protected:
            InputLinux();
            virtual bool init() override;

            bool cursorVisible = true;
            bool cursorLocked = false;
            ::Cursor emptyCursor = None;
        };
    } // namespace input
} // namespace ouzel
