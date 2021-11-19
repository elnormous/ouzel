// Ouzel by Elviss Strazdins

#include "Window.hpp"

namespace ouzel::platform::cocoa
{
    Window::~Window()
    {
        if (window) [window release];
    }
}
