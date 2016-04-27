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
        WindowLinux(const Size2& pSize, bool pResizable, bool pFullscreen, const std::string& pTitle, graphics::Renderer::Driver pDriver);
        virtual bool init() override;
    };
}
