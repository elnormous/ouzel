// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/WindowResource.hpp"

namespace ouzel
{
    class Window;

    class WindowResourceEm: public WindowResource
    {
        friend Window;
    public:
        virtual void setSize(const Size2& newSize) override;

        void handleResize();

    protected:
        WindowResourceEm();
        virtual void init(const Size2& newSize,
                          bool newResizable,
                          bool newFullscreen,
                          bool newExclusiveFullscreen,
                          const std::string& newTitle,
                          bool newHighDpi,
                          bool depth) override;
    };
}
