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
#if OUZEL_SUPPORTS_X11
            static Keyboard::Key convertKeyCode(KeySym keyCode);
            static uint32_t getModifiers(unsigned int state);
#else
            static Keyboard::Key convertKeyCode(uint16_t keyCode);
#endif
            uint32_t getModifiers() const;

            virtual ~InputManagerLinux();

            virtual void setCursorVisible(bool visible) override;
            virtual bool isCursorVisible() const override;

            virtual void setCursorLocked(bool locked) override;
            virtual bool isCursorLocked() const override;

            virtual void setCursorPosition(const Vector2& position) override;

            virtual void startDeviceDiscovery() override;

        private:
            InputManagerLinux();

            void update();

            virtual void activateNativeCursor(NativeCursor* resource) override;
            virtual NativeCursor* createNativeCursor() override;

            int maxFd = 0;
            std::vector<EventDevice> inputDevices;
            bool cursorVisible = true;
            bool cursorLocked = false;
            bool keyboardKeyDown[256];
            bool mouseButtonDown[3];
#if OUZEL_SUPPORTS_X11
            ::Cursor emptyCursor = None;
            ::Cursor currentCursor = None;
#endif
        };
    } // namespace input
} // namespace ouzel
