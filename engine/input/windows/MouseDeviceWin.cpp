// Ouzel by Elviss Strazdins

#include <system_error>
#include "MouseDeviceWin.hpp"
#include "InputSystemWin.hpp"
#include "../../core/Engine.hpp"
#include "../../core/windows/NativeWindowWin.hpp"

namespace ouzel::input::windows
{
    MouseDevice::MouseDevice(InputSystem& initInputSystem,
                             DeviceId initId):
        input::MouseDevice{initInputSystem, initId}
    {
    }

    void MouseDevice::setPosition(const math::Vector<float, 2>& position)
    {
        const auto windowLocation = engine->getWindow().convertNormalizedToWindowLocation(position);
        const auto nativeWindow = static_cast<core::windows::NativeWindow*>(engine->getWindow().getNativeWindow())->getNativeWindow();

        POINT p;
        p.x = static_cast<LONG>(windowLocation.v[0]);
        p.y = static_cast<LONG>(windowLocation.v[1]);
        if (!ClientToScreen(nativeWindow, &p))
            throw std::system_error(GetLastError(), std::system_category(), "Failed to convert client to screen coordinates");

        if (!SetCursorPos(static_cast<int>(p.x), static_cast<int>(p.y)))
            throw std::system_error(GetLastError(), std::system_category(), "Failed to set cursor position");
    }

    void MouseDevice::setCursorVisible(bool visible)
    {
        cursorVisible = visible;
    }

    void MouseDevice::setCursorLocked(bool locked)
    {
        if (locked)
        {
            HWND nativeWindow = static_cast<core::windows::NativeWindow*>(engine->getWindow().getNativeWindow())->getNativeWindow();

            RECT windowRect;
            if (!GetWindowRect(nativeWindow, &windowRect))
                throw std::system_error(GetLastError(), std::system_category(), "Failed to get window rectangle");

            const LONG centerX = (windowRect.left + windowRect.right) / 2;
            const LONG centerY = (windowRect.top + windowRect.bottom) / 2;

            windowRect.left = centerX;
            windowRect.right = centerX + 1;
            windowRect.top = centerY;
            windowRect.bottom = centerY + 1;

            if (!ClipCursor(&windowRect))
                throw std::system_error(GetLastError(), std::system_category(), "Failed to grab pointer");
        }
        else if (!ClipCursor(nullptr))
            throw std::system_error(GetLastError(), std::system_category(), "Failed to free pointer");
    }

    void MouseDevice::setCursor(Cursor* newCursor)
    {
        cursor = newCursor;
    }
}
