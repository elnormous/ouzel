// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_PLATFORM_X11_THREADSUPPORT_HPP
#define OUZEL_PLATFORM_X11_THREADSUPPORT_HPP

#include <stdexcept>
#include <X11/Xlib.h>

namespace ouzel::platform::x11
{
    class ThreadSupport final
    {
    public:
        ThreadSupport()
        {
            if (!XInitThreads())
                throw std::runtime_error("Failed to initialize thread support");
        }
    };
}

#endif // OUZEL_PLATFORM_X11_THREADSUPPORT_HPP
