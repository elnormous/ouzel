// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"
#include <cstdint>
#include <vector>
#if OUZEL_SUPPORTS_X11
#  include <X11/keysym.h>
#  include <X11/X.h>
#endif
#include "input/linux/EventDevice.hpp"
#include "input/InputManager.hpp"

namespace ouzel
{
    class Engine;
    class EngineLinux;

    namespace input
    {
        class InputManagerLinux: public InputManager
        {
            friend Engine;
            friend EngineLinux;
        public:
            virtual ~InputManagerLinux();

            virtual void setCursorVisible(bool visible) override;
            virtual bool isCursorVisible() const override;

            virtual void setCursorLocked(bool locked) override;
            virtual bool isCursorLocked() const override;

        private:
            InputManagerLinux();

            virtual void activateNativeCursor(NativeCursor* resource) override;
            virtual NativeCursor* createNativeCursor() override;

            bool cursorVisible = true;
            bool cursorLocked = false;
#if OUZEL_SUPPORTS_X11
            ::Cursor emptyCursor = None;
            ::Cursor currentCursor = None;
#endif
        };
    } // namespace input
} // namespace ouzel
