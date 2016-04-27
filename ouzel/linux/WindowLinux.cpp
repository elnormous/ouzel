// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowLinux.h"

namespace ouzel
{
    WindowLinux::WindowLinux(const Size2& pSize, bool pResizable, bool pFullscreen, const std::string& pTitle, graphics::Renderer::Driver pDriver):
        Window(pSize, pResizable, pFullscreen, pTitle, pDriver)
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
