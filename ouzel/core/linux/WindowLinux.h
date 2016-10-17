// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define GL_GLEXT_PROTOTYPES 1
#include <GL/glx.h>
#include <X11/X.h>
#include "core/Window.h"

namespace ouzel
{
    class ApplicationLinux;

    class WindowLinux: public Window
    {
        friend Engine;
        friend ApplicationLinux;
    public:
        virtual ~WindowLinux();

        virtual void setSize(const Size2& newSize) override;
        virtual void setFullscreen(bool newFullscreen) override;
        virtual void setTitle(const std::string& newTitle) override;

        Display* getDisplay() const { return display; }
        GLXContext getContext() const { return context; }
        ::Window getNativeWindow() const { return window; }
        Atom getDeleteMessage() const { return deleteMessage; }

    protected:
        WindowLinux(const Size2& aSize, bool aResizable, bool aFullscreen, const std::string& aTitle);
        virtual bool init() override;
        virtual bool toggleFullscreen();
        void handleResize(int width, int height);

        Display* display = nullptr;
        GLXContext context = 0;
        ::Window window = 0;
        Atom deleteMessage;
        Atom state;
        Atom stateFullscreen;
    };
}
