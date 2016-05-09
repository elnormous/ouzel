// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowLinux.h"

namespace ouzel
{
    WindowLinux::WindowLinux(const Size2& pSize, bool pResizable, bool pFullscreen, uint32_t pSampleCount, const std::string& pTitle):
        Window(pSize, pResizable, pFullscreen, pSampleCount, pTitle)
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
