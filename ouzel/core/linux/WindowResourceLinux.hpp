// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <cstdint>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "core/WindowResource.hpp"

namespace ouzel
{
    class Window;
    class EngineLinux;

    class WindowResourceLinux: public WindowResource
    {
        friend Window;
        friend EngineLinux;
    public:
        virtual ~WindowResourceLinux();

        virtual void close() override;

        virtual void setSize(const Size2& newSize) override;
        virtual void setFullscreen(bool newFullscreen) override;
        virtual void setTitle(const std::string& newTitle) override;

        Display* getDisplay() const { return display; }
        ::Window getNativeWindow() const { return window; }
        XVisualInfo* getVisualInfo() const { return visualInfo; }
        Atom getDeleteMessage() const { return deleteMessage; }

    protected:
        WindowResourceLinux();
        virtual bool init(const Size2& newSize,
                          bool newResizable,
                          bool newFullscreen,
                          bool newExclusiveFullscreen,
                          const std::string& newTitle,
                          bool newHighDpi,
                          bool depth) override;
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
