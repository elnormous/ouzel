// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/NativeWindow.hpp"

namespace ouzel
{
    class Window;

    class NativeWindowEm: public NativeWindow
    {
        friend Window;
    public:
        virtual void setSize(const Size2& newSize) override;

        void handleResize();

    protected:
        NativeWindowEm(const Size2& newSize,
                       bool newFullscreen,
                       const std::string& newTitle);
    };
}
