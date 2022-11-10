// Ouzel by Elviss Strazdins

#include "Window.hpp"

namespace ouzel::platform::cocoa
{
    Window::~Window()
    {
        if (window) [window release];
    }

    Window& Window::operator=(NSWindowPtr a) noexcept
    {
        [window release];
        window = a;
        return *this;
    }

    Window::Window(const Window& other) noexcept: window{other.window}
    {
        [window retain];
    }

    Window& Window::operator=(const Window& other) noexcept
    {
        if (&other == this) return *this;
        [other.window retain];
        [window release];
        window = other.window;
        return *this;
    }

    Window::Window(Window&& other) noexcept: window{other.window}
    {
        other.window = nil;
    }

    Window& Window::operator=(Window&& other) noexcept
    {
        if (&other == this) return *this;
        [window release];

        window = other.window;
        other.window = nil;
        return *this;
    }
}
