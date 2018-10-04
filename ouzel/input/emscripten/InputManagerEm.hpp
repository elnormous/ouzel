// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include <map>
#include <string>
#include <emscripten/html5.h>
#include "input/InputManager.hpp"

namespace ouzel
{
    class Engine;

    namespace input
    {
        class InputManagerEm: public InputManager
        {
            friend Engine;
        public:
            virtual ~InputManagerEm();

            virtual void setCursorVisible(bool visible) override;
            virtual bool isCursorVisible() const override;

            virtual void setCursorLocked(bool locked) override;
            virtual bool isCursorLocked() const override;

            void pointerLockChanged(bool locked);

        private:
            InputManagerEm();

            bool cursorVisible = true;
            bool cursorLocked = false;
        };
    } // namespace input
} // namespace ouzel
