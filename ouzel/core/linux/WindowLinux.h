// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
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

        virtual void close() override;

        virtual void setSize(const Size2& newSize) override;
        virtual void setFullscreen(bool newFullscreen) override;
        virtual void setTitle(const std::string& newTitle) override;

        Display* getDisplay() const { return display; }
        ::Window getNativeWindow() const { return window; }
        XVisualInfo* getVisualInfo() const { return visualInfo; }
        Atom getDeleteMessage() const { return deleteMessage; }

    protected:
        WindowLinux(const Size2& aSize, bool aResizable, bool aFullscreen, const std::string& aTitle);
        virtual bool init() override;
        virtual bool toggleFullscreen();
        void handleResize(const Size2& newSize);

        XVisualInfo* visualInfo = nullptr;
        Display* display = nullptr;
        ::Window window = 0;
        Atom deleteMessage;
        Atom protocols;
        Atom state;
        Atom stateFullscreen;
    };
}
