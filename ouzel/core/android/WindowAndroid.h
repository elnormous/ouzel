// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Window.h"

namespace ouzel
{
    class WindowAndroid: public Window
    {
        friend Engine;
    public:
        void handleResize(const Size2& newSize);

    protected:
        WindowAndroid(const Size2& aSize, bool aResizable, bool aFullscreen, const std::string& aTitle);
        virtual bool init() override;
    };
}
