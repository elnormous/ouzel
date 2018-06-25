// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

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

            void update();

            static KeyboardKey convertKeyCode(const EM_UTF8 key[32]);
            static uint32_t getKeyboardModifiers(const EmscriptenKeyboardEvent* keyboardEvent);
            static uint32_t getMouseModifiers(const EmscriptenMouseEvent* mouseEvent);

            virtual void setCursorVisible(bool visible) override;
            virtual bool isCursorVisible() const override;

            virtual void setCursorLocked(bool locked) override;
            virtual bool isCursorLocked() const override;

            void pointerLockChanged(bool locked);

            void handleGamepadConnected(long device);
            void handleGamepadDisconnected(long device);

        private:
            InputManagerEm();
            virtual void init() override;

            bool cursorVisible = true;
            bool cursorLocked = false;
        };
    } // namespace input
} // namespace ouzel
