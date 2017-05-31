// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <X11/cursorfont.h>
#include <X11/Xcursor/Xcursor.h>
#include "CursorResourceLinux.h"
#include "core/Engine.h"
#include "core/linux/WindowLinux.h"

namespace ouzel
{
    namespace input
    {
        CursorResourceLinux::~CursorResourceLinux()
        {
            if (sharedEngine)
            {
                WindowLinux* windowLinux = static_cast<WindowLinux*>(sharedEngine->getWindow());
                Display* display = windowLinux->getDisplay();
                if (cursor != None) XFreeCursor(display, cursor);
            }
        }

        bool CursorResourceLinux::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (dirty)
            {
                if (sharedEngine)
                {
                    WindowLinux* windowLinux = static_cast<WindowLinux*>(sharedEngine->getWindow());
                    Display* display = windowLinux->getDisplay();

                    if (data.empty())
                    {
                        if (cursor != None)
                        {
                            XFreeCursor(display, cursor);
                            cursor = None;
                        }

                        switch (systemCursor)
                        {
                            case SystemCursor::DEFAULT:
                                cursor = XcursorLibraryLoadCursor(display, "arrow");
                                break;
                            case SystemCursor::ARROW:
                                cursor = XcursorLibraryLoadCursor(display, "arrow");
                                break;
                            case SystemCursor::HAND:
                                cursor = XcursorLibraryLoadCursor(display, "hand1");
                                break;
                            case SystemCursor::HORIZONTAL_RESIZE:
                                cursor = XcursorLibraryLoadCursor(display, "sb_h_double_arrow");
                                break;
                            case SystemCursor::VERTICAL_RESIZE:
                                cursor = XcursorLibraryLoadCursor(display, "sb_v_double_arrow");
                                break;
                            case SystemCursor::CROSS:
                                cursor = XcursorLibraryLoadCursor(display, "crosshair");
                                break;
                            case SystemCursor::I_BEAM:
                                cursor = XcursorLibraryLoadCursor(display, "xterm");
                                break;
                        }
                    }
                }

                dirty = false;
            }

            return true;
        }
    } // namespace input
} // namespace ouzel
