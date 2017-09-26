// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/WindowResource.hpp"

namespace ouzel
{
    class Window;

    class WindowResourceRasp: public WindowResource
    {
        friend Window;
    public:
        virtual ~WindowResourceRasp();

    protected:
        WindowResourceRasp();
        virtual bool init(const Size2& newSize,
                          bool newResizable,
                          bool newFullscreen,
                          bool newExclusiveFullscreen,
                          const std::string& newTitle,
                          bool newHighDpi,
                          bool depth) override;
    };
}
