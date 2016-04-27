// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowAndroid.h"

namespace ouzel
{
    WindowAndroid::WindowAndroid(const Size2& pSize, bool pResizable, bool pFullscreen, const std::string& pTitle, graphics::Renderer::Driver pDriver):
        Window(pSize, pResizable, pFullscreen, pTitle, pDriver)
    {

    }

    WindowAndroid::~WindowAndroid()
    {

    }

    bool WindowTVOS::init()
    {
        return Window::init();
    }
}
