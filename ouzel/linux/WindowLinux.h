// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Window.h"

namespace ouzel
{
    class WindowLinux: public Window
    {
        friend Engine;
    public:
        virtual ~WindowLinux();

    protected:
        WindowLinux(const Size2& size, bool resizable, bool fullscreen, const std::string& title, graphics::Renderer::Driver driver);
        virtual bool init() override;
    };
}
