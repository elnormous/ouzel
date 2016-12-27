// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowAndroid.h"
#include "core/Engine.h"

namespace ouzel
{
    WindowAndroid::WindowAndroid(const Size2& aSize, bool aResizable, bool aFullscreen, const std::string& aTitle):
        Window(aSize, aResizable, aFullscreen, aTitle)
    {
    }

    void WindowAndroid::handleResize(const Size2& newSize)
    {
        Window::setSize(newSize);
        sharedEngine->getRenderer()->setSize(newSize);
    }
}
