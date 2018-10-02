// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "MouseDeviceWin.hpp"
#include "core/Engine.hpp"
#include "core/windows/NativeWindowWin.hpp"

namespace ouzel
{
    namespace input
    {
        void MouseDeviceWin::setPosition(const Vector2& position)
        {
            Vector2 windowLocation = engine->getWindow()->convertNormalizedToWindowLocation(position);
            HWND nativeWindow = static_cast<NativeWindowWin*>(engine->getWindow()->getNativeWindow())->getNativeWindow();

            POINT p;
            p.x = static_cast<LONG>(windowLocation.x);
            p.y = static_cast<LONG>(windowLocation.y);
            ClientToScreen(nativeWindow, &p);
            SetCursorPos(static_cast<int>(p.x), static_cast<int>(p.y));
        }
    } // namespace input
} // namespace ouzel
