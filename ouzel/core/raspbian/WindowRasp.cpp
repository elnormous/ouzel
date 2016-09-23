// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowRasp.h"
#include "core/Engine.h"
#include "graphics/Renderer.h"
#include "graphics/opengl/RendererOGL.h"
#include "utils/Utils.h"

namespace ouzel
{
    WindowRasp::WindowRasp(const Size2& pSize, bool pResizable, bool pFullscreen, const std::string& pTitle):
        Window(pSize, pResizable, pFullscreen, pTitle)
    {
    }

    WindowRasp::~WindowRasp()
    {
    }

    bool WindowRasp::init()
    {
        return Window::init();
    }

    void WindowRasp::setSize(const Size2& newSize)
    {
        Window::setSize(newSize);
    }

    void WindowRasp::setFullscreen(bool newFullscreen)
    {
        Window::setFullscreen(newFullscreen);
    }

    void WindowRasp::setTitle(const std::string& newTitle)
    {
        Window::setTitle(newTitle);
    }
}
