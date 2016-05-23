// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowAndroid.h"
#include "Engine.h"
#include "opengl/RendererOGL.h"

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
        std::shared_ptr<graphics::RendererOGL> rendererOGL = std::static_pointer_cast<graphics::RendererOGL>(sharedEngine->getRenderer());
        
        rendererOGL->setAPIVersion(2);

        return Window::init();
    }
}
