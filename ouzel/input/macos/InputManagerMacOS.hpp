// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#if defined(__OBJC__)
#import <IOKit/hid/IOHIDManager.h>
#import <AppKit/NSCursor.h>
typedef NSCursor* NSCursorPtr;
#include <GameController/GameController.h>
typedef GCController* GCControllerPtr;
#else
#import <objc/NSObjCRuntime.h>
typedef void* IOHIDManagerRef;
typedef void* IOHIDDeviceRef;
typedef id NSCursorPtr;
typedef id GCControllerPtr;
#endif

#include "input/InputManager.hpp"

namespace ouzel
{
    namespace input
    {
        class GamepadGC;
        class GamepadIOKit;

        class InputManagerMacOS: public InputManager
        {
            friend Engine;
        public:
            virtual ~InputManagerMacOS();
            virtual void setCursorVisible(bool visible) override;
            virtual bool isCursorVisible() const override;

            virtual void setCursorLocked(bool locked) override;
            virtual bool isCursorLocked() const override;

            virtual void setCursorPosition(const Vector2& position) override;

            virtual void startGamepadDiscovery() override;
            virtual void stopGamepadDiscovery() override;

            void handleGamepadDiscoveryCompleted();

            void handleGamepadConnected(GCControllerPtr device);
            void handleGamepadDisconnected(GCControllerPtr device);
            void handleGamepadConnected(IOHIDDeviceRef device);
            void handleGamepadDisconnected(IOHIDDeviceRef device);

            static KeyboardKey convertKeyCode(uint16_t keyCode);
            static NSUInteger getKeyMask(uint16_t keyCode);
            static uint32_t getModifiers(NSUInteger modifierFlags, NSUInteger pressedMouseButtons);

            inline NSCursorPtr getNativeCursor() const { return currentCursor; }
            inline NSCursorPtr getEmptyCursor() const { return emptyCursor; }

        protected:
            InputManagerMacOS();
            virtual bool init() override;

            virtual void activateCursorResource(CursorResource* resource) override;
            virtual CursorResource* createCursorResource() override;

            id connectDelegate = nil;
            IOHIDManagerRef hidManager = nullptr;

            std::vector<GamepadGC*> gamepadsGC;
            std::vector<GamepadIOKit*> gamepadsIOKit;

            bool discovering = false;
            bool cursorVisible = true;
            bool cursorLocked = false;

            NSCursorPtr emptyCursor = nil;
            NSCursorPtr defaultCursor = nil;
            NSCursorPtr currentCursor = nil;
        };
    } // namespace input
} // namespace ouzel
