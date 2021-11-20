// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_COCOA_WINDOW_HPP
#define OUZEL_PLATFORM_COCOA_WINDOW_HPP

#ifdef __OBJC__
#  import <Cocoa/Cocoa.h>
typedef NSWindow* NSWindowPtr;
#else
#  include <objc/NSObjCRuntime.h>
using NSWindowPtr = id;
#endif

namespace ouzel::platform::cocoa
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

#endif // OUZEL_PLATFORM_COCOA_WINDOW_HPP
