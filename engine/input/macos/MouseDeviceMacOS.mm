// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#import <Carbon/Carbon.h>
#include "MouseDeviceMacOS.hpp"
#include "core/Engine.hpp"
#include "core/macos/NativeWindowMacOS.hpp"

namespace ouzel
{
    namespace input
    {
        void MouseDeviceMacOS::setPosition(const Vector2F& position)
        {
            ouzel::Vector2F windowLocation = engine->getWindow()->convertNormalizedToWindowLocation(position);

            const CGPoint screenOrigin = [[NSScreen mainScreen] visibleFrame].origin;

            NativeWindowMacOS* windowMacOS = static_cast<NativeWindowMacOS*>(engine->getWindow()->getNativeWindow());
            const CGPoint windowOrigin = [windowMacOS->getNativeWindow() frame].origin;

            CGWarpMouseCursorPosition(CGPointMake(screenOrigin.x + windowOrigin.x + windowLocation.v[0],
                                                  screenOrigin.y + windowOrigin.y + windowLocation.v[1]));
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
