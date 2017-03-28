// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "input/Input.h"

#if defined(__OBJC__)
#include <GameController/GameController.h>
typedef GCController* GCControllerPtr;
@class ConnectDelegate;
typedef ConnectDelegate* ConnectDelegatePtr;
#else
#include <objc/objc.h>
#include <objc/NSObjCRuntime.h>
typedef id GCControllerPtr;
typedef id ConnectDelegatePtr;
#endif

namespace ouzel
{
    namespace input
    {
        class GamepadApple;

        class InputApple: public Input
        {
            friend Engine;
        public:
            virtual void setCursorVisible(bool visible) override;
            virtual bool isCursorVisible() const override;

            virtual void setCursorLocked(bool locked) override;
            virtual bool isCursorLocked() const override;

            virtual void setCursorPosition(const Vector2& position) override;

            virtual void startGamepadDiscovery() override;
            virtual void stopGamepadDiscovery() override;

            virtual bool showVirtualKeyboard() override;
            virtual bool hideVirtualKeyboard() override;

            void handleGamepadDiscoveryCompleted();
            void handleGamepadConnected(GCControllerPtr controller);
            void handleGamepadDisconnected(GCControllerPtr controller);

#ifdef OUZEL_PLATFORM_MACOS
            static KeyboardKey convertKeyCode(unsigned short keyCode);
            static uint32_t getModifiers(NSUInteger modifierFlags, NSUInteger pressedMouseButtons);
#endif

        protected:
            InputApple();

            ConnectDelegatePtr connectDelegate = nullptr;

            bool discovering = false;
            bool cursorVisible = true;
            bool cursorLocked = false;
        };
    } // namespace input
} // namespace ouzel
