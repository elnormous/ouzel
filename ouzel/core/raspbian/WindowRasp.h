// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Window.h"

namespace ouzel
{
    class WindowRasp: public Window
    {
        friend Engine;
    public:
        virtual ~WindowRasp();

    protected:
        WindowRasp(const Size2& pSize, bool pResizable, bool pFullscreen, const std::string& pTitle);
        virtual bool init() override;
    };
}
