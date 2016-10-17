// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowAndroid.h"
#include "core/Engine.h"
#include "graphics/opengl/RendererOGL.h"

namespace ouzel
{
    WindowAndroid::WindowAndroid(const Size2& aSize, bool aResizable, bool aFullscreen, const std::string& aTitle):
        Window(aSize, aResizable, aFullscreen, aTitle)
    {
    }

    bool WindowAndroid::init()
    {
        graphics::RendererOGL* rendererOGL = static_cast<graphics::RendererOGL*>(sharedEngine->getRenderer());
        rendererOGL->setAPIVersion(2, 0);

        return Window::init();
    }

    void WindowAndroid::handleResize(const Size2& newSize)
    {
        Window::setSize(newSize);
        sharedEngine->getRenderer()->setSize(newSize);
    }
}
