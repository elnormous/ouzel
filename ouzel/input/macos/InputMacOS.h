// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#if defined(__OBJC__)
#import <IOKit/hid/IOHIDManager.h>
#import <AppKit/NSCursor.h>
typedef NSCursor* NSCursorPtr;
#else
typedef void* IOHIDManagerRef;
typedef void* IOHIDDeviceRef;
typedef id NSCursorPtr;
#endif

#include "input/Input.h"

namespace ouzel
{
    namespace input
    {
        class InputMacOS: public Input
        {
            friend Engine;
        public:
            virtual ~InputMacOS();
            virtual void setCursorVisible(bool visible) override;
            virtual bool isCursorVisible() const override;

            virtual void setCursorLocked(bool locked) override;
            virtual bool isCursorLocked() const override;

            virtual void setCursorPosition(const Vector2& position) override;

            void handleGamepadConnected(IOHIDDeviceRef device);
            void handleGamepadDisconnected(IOHIDDeviceRef device);
            
            static KeyboardKey convertKeyCode(unsigned short keyCode);
            static uint32_t getModifiers(NSUInteger modifierFlags, NSUInteger pressedMouseButtons);

        protected:
            InputMacOS();
            virtual bool init() override;

            virtual void activateCursorResource(CursorResource* resource) override;
            virtual CursorResource* createCursorResource() override;

            IOHIDManagerRef hidManager = nullptr;

            bool discovering = false;
            bool cursorVisible = true;
            bool cursorLocked = false;

            NSCursorPtr emptyCursor = Nil;
            NSCursorPtr currentCursor = Nil;
        };
    } // namespace input
} // namespace ouzel
