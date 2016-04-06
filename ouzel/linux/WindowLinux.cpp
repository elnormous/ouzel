// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowLinux.h"

namespace ouzel
{
    WindowLinux::WindowLinux(const Size2& size, bool resizable, bool fullscreen, const std::string& title, video::Renderer::Driver driver):
        Window(size, resizable, fullscreen, title, driver)
    {

    }

    WindowLinux::~WindowLinux()
    {

    }

    bool WindowLinux::init()
    {
        return Window::init();
    }
}
