#ifndef FLUXION_PLATFORM_X11_THREADSUPPORT_HPP
#define FLUXION_PLATFORM_X11_THREADSUPPORT_HPP

#include <stdexcept>
#include <X11/Xlib.h>

namespace fluxion::platform::x11
{
    class ThreadSupport final
    {
    public:
        ThreadSupport()
        {
            if (XInitThreads() == 0)
                throw std::runtime_error{"Failed to initialize thread support"};
        }
    };
}

#endif // FLUXION_PLATFORM_X11_THREADSUPPORT_HPP
