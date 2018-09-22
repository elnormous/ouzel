// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#if defined(__OBJC__)
#import <AppKit/NSCursor.h>
typedef NSCursor* NSCursorPtr;
#else
#include <objc/NSObjCRuntime.h>
typedef id NSCursorPtr;
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

            inline NSCursorPtr getNativeCursor() const { return currentCursor; }
            inline NSCursorPtr getEmptyCursor() const { return emptyCursor; }

        private:
            InputManagerMacOS();

            virtual void activateNativeCursor(NativeCursor* resource) override;
            virtual NativeCursor* createNativeCursor() override;

            unsigned char emptyCursorData[4] = {0, 0, 0, 0};
            NSCursorPtr emptyCursor = nil;
            NSCursorPtr defaultCursor = nil;
            NSCursorPtr currentCursor = nil;
        };
    } // namespace input
} // namespace ouzel
