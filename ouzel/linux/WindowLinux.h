// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <GL/glx.h>
#include <X11/X.h>
#include <X11/keysym.h>
#include "Window.h"

namespace ouzel
{
    class WindowLinux: public Window
    {
        friend Engine;
    public:
        virtual ~WindowLinux();

        virtual void setSize(const Size2& newSize) override;
        virtual void setFullscreen(bool newFullscreen) override;
        virtual void setTitle(const std::string& newTitle) override;

        Display* getDisplay() const { return display; }
        GLXContext getContext() const { return context; }
        ::Window getNativeWindow() const { return window; }

    protected:
        WindowLinux(const Size2& pSize, bool pResizable, bool pFullscreen, const std::string& pTitle);
        virtual bool init() override;

        Display* display = nullptr;
        GLXContext context = 0;
        ::Window window = 0;
    };
}
