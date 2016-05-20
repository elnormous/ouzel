// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowAndroid.h"

namespace ouzel
{
    WindowAndroid::WindowAndroid(const Size2& pSize, bool pResizable, bool pFullscreen, uint32_t pSampleCount, const std::string& pTitle):
        Window(pSize, pResizable, pFullscreen, pSampleCount, pTitle)
    {

    }

    WindowAndroid::~WindowAndroid()
    {

    }

    bool WindowAndroid::init()
    {
        return Window::init();
    }
}
