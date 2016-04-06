// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Window.h"

namespace ouzel
{
    class WindowAndroid: public Window
    {
        friend Engine;
    public:
        virtual ~WindowAndroid();

    protected:
        WindowAndroid(const Size2& size, bool resizable, bool fullscreen, const std::string& title, video::Renderer::Driver driver);
        virtual bool init() override;
    };
}
