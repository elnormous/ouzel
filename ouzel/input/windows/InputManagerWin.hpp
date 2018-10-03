// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Xinput.h>
#include "input/InputManager.hpp"

namespace ouzel
{
    namespace input
    {
        class InputManagerWin : public InputManager
        {
            friend Engine;
        public:
            virtual ~InputManagerWin();

            virtual void setCursorVisible(bool visible) override;
            virtual bool isCursorVisible() const override;

            virtual void setCursorLocked(bool locked) override;
            virtual bool isCursorLocked() const override;

            void updateCursor();

        private:
            InputManagerWin();

            virtual void activateNativeCursor(NativeCursor* resource) override;
            virtual NativeCursor* createNativeCursor() override;

            bool cursorVisible = true;
            bool cursorLocked = false;
            HCURSOR defaultCursor = nullptr;
            HCURSOR currentCursor = nullptr;
        };
    } // namespace input
} // namespace ouzel
