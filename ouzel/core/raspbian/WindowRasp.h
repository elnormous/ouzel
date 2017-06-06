// Copyright (C) 2017 Elviss Strazdins
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
        WindowRasp();
        virtual bool init(const Size2& newSize,
                          bool newResizable,
                          bool newFullscreen,
                          const std::string& newTitle,
                          bool newHighDpi,
                          bool depth) override;
    };
}
