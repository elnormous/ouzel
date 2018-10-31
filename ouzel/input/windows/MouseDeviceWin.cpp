// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "MouseDeviceWin.hpp"
#include "core/Engine.hpp"
#include "core/windows/NativeWindowWin.hpp"
#include "utils/Errors.hpp"

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

        void MouseDeviceWin::setCursorVisible(bool visible)
        {
            cursorVisible = visible;
        }

        void MouseDeviceWin::setCursorLocked(bool locked)
        {
            if (locked)
            {
                HWND nativeWindow = static_cast<NativeWindowWin*>(engine->getWindow()->getNativeWindow())->getNativeWindow();

                RECT rect;
                GetWindowRect(nativeWindow, &rect);

                LONG centerX = (rect.left + rect.right) / 2;
                LONG centerY = (rect.top + rect.bottom) / 2;

                rect.left = centerX;
                rect.right = centerX + 1;
                rect.top = centerY;
                rect.bottom = centerY + 1;

                if (!ClipCursor(&rect))
                    throw SystemError("Failed to grab pointer");
            }
            else
                ClipCursor(nullptr);
        }

        void MouseDeviceWin::setCursor(NativeCursorWin* newCursor)
        {
            cursor = newCursor;
        }
    } // namespace input
} // namespace ouzel
