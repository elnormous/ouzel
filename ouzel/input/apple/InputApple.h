// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <vector>
#include <memory>
#include "input/Input.h"

#if defined(__OBJC__)
@class ConnectDelegate;
typedef ConnectDelegate* ConnectDelegatePtr;
#else
#include <objc/objc.h>
#include <objc/NSObjCRuntime.h>
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
            virtual ~InputApple();

            virtual void setCursorVisible(bool visible) override;
            virtual bool isCursorVisible() const override;

            virtual void startGamepadDiscovery() override;
            virtual void stopGamepadDiscovery() override;

            virtual bool showVirtualKeyboard() override;
            virtual bool hideVirtualKeyboard() override;

            void handleGamepadDiscoveryCompleted();
            void handleGamepadConnected(id controller);
            void handleGamepadDisconnected(id controller);

#ifdef OUZEL_PLATFORM_MACOS
            static KeyboardKey convertKeyCode(unsigned short keyCode);
            static uint32_t getModifiers(NSUInteger modifierFlags, NSUInteger pressedMouseButtons);
#endif

        protected:
            InputApple();

            ConnectDelegatePtr connectDelegate = nullptr;
            std::vector<std::shared_ptr<GamepadApple>> gamepads;

            bool discovering = false;
            bool cursorVisible = true;
        };
    } // namespace input
} // namespace ouzel
