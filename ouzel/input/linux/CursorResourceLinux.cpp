// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <X11/cursorfont.h>
#include "CursorResourceLinux.h"

namespace ouzel
{
    namespace input
    {
        CursorResourceLinux::~CursorResourceLinux()
        {
        }

        bool CursorResourceLinux::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (dirty)
            {
                dirty = false;
            }

            return true;
        }
    } // namespace input
} // namespace ouzel
