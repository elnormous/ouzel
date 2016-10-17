// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Window.h"

namespace ouzel
{
    class WindowEm: public Window
    {
        friend Engine;
    public:
        virtual void setSize(const Size2& newSize) override;

        void handleResize();

    protected:
        WindowEm(const Size2& aSize, bool aResizable, bool aFullscreen, const std::string& aTitle);
        virtual bool init() override;
    };
}
