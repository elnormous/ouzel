// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include "core/Setup.h"
#include "MouseDeviceLinux.hpp"
#include "core/linux/EngineLinux.hpp"
#include "core/linux/NativeWindowLinux.hpp"

namespace ouzel
{
    namespace input
    {
        void MouseDeviceLinux::setPosition(const Vector2& position)
        {
#if OUZEL_SUPPORTS_X11
            EngineLinux* engineLinux = static_cast<EngineLinux*>(engine);
            NativeWindowLinux* windowLinux = static_cast<NativeWindowLinux*>(engine->getWindow()->getNativeWindow());
            Display* display = engineLinux->getDisplay();
            ::Window window = windowLinux->getNativeWindow();

            XWindowAttributes attributes;
            XGetWindowAttributes(display, window, &attributes);

            Vector2 windowLocation = engine->getWindow()->convertNormalizedToWindowLocation(position);

            XWarpPointer(display, None, window, 0, 0, 0, 0,
                         attributes.x + static_cast<int>(windowLocation.v[0]),
                         attributes.y + static_cast<int>(windowLocation.v[1]));
            XSync(display, False);
#endif
        }

        void MouseDeviceLinux::setCursorVisible(bool visible)
        {
            cursorVisible = visible;
        }

        void MouseDeviceLinux::setCursorLocked(bool locked)
        {
#if OUZEL_SUPPORTS_X11
            EngineLinux* engineLinux = static_cast<EngineLinux*>(engine);
            NativeWindowLinux* windowLinux = static_cast<NativeWindowLinux*>(engine->getWindow()->getNativeWindow());
            Display* display = engineLinux->getDisplay();
            ::Window window = windowLinux->getNativeWindow();

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

        void MouseDeviceLinux::setCursor(CursorLinux* newCursor)
        {
            cursor = newCursor;
        }
    } // namespace input
} // namespace ouzel
