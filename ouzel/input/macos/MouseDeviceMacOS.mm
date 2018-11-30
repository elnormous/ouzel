// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#import <Carbon/Carbon.h>
#include "MouseDeviceMacOS.hpp"
#include "core/Engine.hpp"
#include "core/macos/NativeWindowMacOS.hpp"

namespace ouzel
{
    namespace input
    {
        void MouseDeviceMacOS::setPosition(const Vector2& position)
        {
            ouzel::Vector2 windowLocation = engine->getWindow()->convertNormalizedToWindowLocation(position);

            CGPoint screenOrigin = [[NSScreen mainScreen] visibleFrame].origin;

            NativeWindowMacOS* windowMacOS = static_cast<NativeWindowMacOS*>(engine->getWindow()->getNativeWindow());
            CGPoint windowOrigin = [windowMacOS->getNativeWindow() frame].origin;

            CGWarpMouseCursorPosition(CGPointMake(screenOrigin.x + windowOrigin.x + windowLocation.x,
                                                  screenOrigin.y + windowOrigin.y + windowLocation.y));
        }

        void MouseDeviceMacOS::setCursorVisible(bool visible)
        {
            cursorVisible = visible;
        }

        void MouseDeviceMacOS::setCursorLocked(bool locked)
        {
            CGAssociateMouseAndMouseCursorPosition(!locked);
        }

        void MouseDeviceMacOS::setCursor(CursorMacOS* newCursor)
        {
            cursor = newCursor;
        }
    } // namespace input
} // namespace ouzel
