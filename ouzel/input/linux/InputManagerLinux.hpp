// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <vector>
#include <X11/keysym.h>
#include <X11/X.h>
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
            static KeyboardKey convertKeyCode(KeySym keyCode);
            static uint32_t getModifiers(unsigned int state);

            virtual ~InputManagerLinux();

            virtual void setCursorVisible(bool visible) override;
            virtual bool isCursorVisible() const override;

            virtual void setCursorLocked(bool locked) override;
            virtual bool isCursorLocked() const override;

            virtual void setCursorPosition(const Vector2& position) override;

            void handleXInput2Event(XGenericEventCookie* cookie);

        private:
            InputManagerLinux();
            virtual void init() override;

            void update();

            virtual void activateCursorResource(CursorResource* resource) override;
            virtual CursorResource* createCursorResource() override;

            int maxFd = 0;
            std::vector<EventDevice> inputDevices;
            bool cursorVisible = true;
            bool cursorLocked = false;
            ::Cursor emptyCursor = None;
            ::Cursor currentCursor = None;
            int xInputOpCode = 0;
        };
    } // namespace input
} // namespace ouzel
