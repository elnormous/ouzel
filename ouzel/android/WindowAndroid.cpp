// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowAndroid.h"

namespace ouzel
{
    WindowAndroid::WindowAndroid(const Size2& size, bool resizable, bool fullscreen, const std::string& title, graphics::Renderer::Driver driver):
        Window(size, resizable, fullscreen, title, driver)
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
