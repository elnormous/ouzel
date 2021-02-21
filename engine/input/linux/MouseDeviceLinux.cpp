// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include "../../core/Setup.h"
#include "MouseDeviceLinux.hpp"
#include "InputSystemLinux.hpp"
#include "../../core/linux/EngineLinux.hpp"
#include "../../core/linux/NativeWindowLinux.hpp"

namespace ouzel::input::linux
{
    MouseDevice::MouseDevice(InputSystem& initInputSystem,
                             DeviceId initId):
        input::MouseDevice(initInputSystem, initId)
    {
    }

    void MouseDevice::setPosition(const Vector2F& position)
    {
#if OUZEL_SUPPORTS_X11
        auto engineLinux = static_cast<core::linux::Engine*>(engine);
        auto windowLinux = static_cast<core::linux::NativeWindow*>(engine->getWindow()->getNativeWindow());
        auto display = engineLinux->getDisplay();
        auto window = windowLinux->getNativeWindow();

        XWindowAttributes attributes;
        XGetWindowAttributes(display, window, &attributes);

        const auto windowLocation = engine->getWindow()->convertNormalizedToWindowLocation(position);

        XWarpPointer(display, None, window, 0, 0, 0, 0,
                     attributes.x + static_cast<int>(windowLocation.v[0]),
                     attributes.y + static_cast<int>(windowLocation.v[1]));
        XSync(display, False);
#endif
    }

    void MouseDevice::setCursorVisible(bool visible)
    {
        cursorVisible = visible;
    }

    void MouseDevice::setCursorLocked(bool locked)
    {
#if OUZEL_SUPPORTS_X11
        auto engineLinux = static_cast<core::linux::Engine*>(engine);
        auto windowLinux = static_cast<core::linux::NativeWindow*>(engine->getWindow()->getNativeWindow());
        auto display = engineLinux->getDisplay();
        auto window = windowLinux->getNativeWindow();

        if (locked)
        {
            if (XGrabPointer(display, window, False,
                             ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask,
                             GrabModeAsync, GrabModeAsync,
                             None, None, CurrentTime) != GrabSuccess)
                throw std::runtime_error("Failed to grab pointer");
        }
        else
            XUngrabPointer(display, CurrentTime);

        XSync(display, False);
#endif
    }

    void MouseDevice::setCursor(Cursor* newCursor)
    {
        cursor = newCursor;
    }
}
