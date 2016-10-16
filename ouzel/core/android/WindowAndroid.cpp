// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowAndroid.h"
#include "core/Engine.h"
#include "graphics/opengl/RendererOGL.h"

namespace ouzel
{
    WindowAndroid::WindowAndroid(const Size2& pSize, bool pResizable, bool pFullscreen, const std::string& pTitle):
        Window(pSize, pResizable, pFullscreen, pTitle)
    {
    }

    WindowAndroid::~WindowAndroid()
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
