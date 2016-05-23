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
        WindowAndroid(const Size2& pSize, bool pResizable, bool pFullscreen, const std::string& pTitle);
        virtual bool init() override;
    };
}
