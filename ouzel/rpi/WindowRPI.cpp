// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowRPI.h"
#include "core/Engine.h"
#include "graphics/Renderer.h"
#include "opengl/RendererOGL.h"
#include "utils/Utils.h"

namespace ouzel
{
    WindowRPI::WindowRPI(const Size2& pSize, bool pResizable, bool pFullscreen, const std::string& pTitle):
        Window(pSize, pResizable, pFullscreen, pTitle)
    {

    }

    WindowRPI::~WindowRPI()
    {

    }

    bool WindowRPI::init()
    {
        return Window::init();
    }

    void WindowRPI::setSize(const Size2& newSize)
    {
        Window::setSize(newSize);
    }

    void WindowRPI::setFullscreen(bool newFullscreen)
    {
        Window::setFullscreen(newFullscreen);
    }

    void WindowRPI::setTitle(const std::string& newTitle)
    {
        Window::setTitle(newTitle);
    }
}
