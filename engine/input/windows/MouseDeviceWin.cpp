// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include <system_error>
#include "MouseDeviceWin.hpp"
#include "../../core/Engine.hpp"
#include "../../core/windows/NativeWindowWin.hpp"

namespace ouzel::input
{
    void MouseDeviceWin::setPosition(const Vector2F& position)
    {
        const Vector2F windowLocation = engine->getWindow()->convertNormalizedToWindowLocation(position);
        HWND nativeWindow = static_cast<NativeWindowWin*>(engine->getWindow()->getNativeWindow())->getNativeWindow();

        POINT p;
        p.x = static_cast<LONG>(windowLocation.v[0]);
        p.y = static_cast<LONG>(windowLocation.v[1]);
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

            const LONG centerX = (rect.left + rect.right) / 2;
            const LONG centerY = (rect.top + rect.bottom) / 2;

            rect.left = centerX;
            rect.right = centerX + 1;
            rect.top = centerY;
            rect.bottom = centerY + 1;

            if (!ClipCursor(&rect))
                throw std::system_error(GetLastError(), std::system_category(), "Failed to grab pointer");
        }
        else if (!ClipCursor(nullptr))
            throw std::system_error(GetLastError(), std::system_category(), "Failed to free pointer");
    }

    void MouseDeviceWin::setCursor(CursorWin* newCursor)
    {
        cursor = newCursor;
    }
}
