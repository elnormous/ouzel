#ifndef FLUXION_PLATFORM_COCOA_WINDOW_HPP
#define FLUXION_PLATFORM_COCOA_WINDOW_HPP

#ifdef __OBJC__
#  import <Cocoa/Cocoa.h>
using NSWindowPtr = NSWindow*;
#else
#  include <objc/NSObjCRuntime.h>
using NSWindowPtr = id;
#endif

namespace fluxion::platform::cocoa
{
    class Window final
    {
    public:
        ~Window();

        Window& operator=(NSWindowPtr a) noexcept;
        Window(const Window& other) noexcept;
        Window& operator=(const Window& other) noexcept;
        Window(Window&& other) noexcept;
        Window& operator=(Window&& other) noexcept;

    private:
        NSWindowPtr window = nil;
    };
}

#endif // FLUXION_PLATFORM_COCOA_WINDOW_HPP
