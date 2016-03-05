// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <windows.h>
#include "Window.h"

namespace ouzel
{
    class WindowWin : public Window
    {
        friend Engine;
    public:
        virtual ~WindowWin();

        virtual void setSize(const Size2& size) override;
        virtual void setTitle(const std::string& title) override;

        void handleResize(INT width, INT height);

        HWND getWindow() const { return _window; }
        HMONITOR getMonitor() const;

    protected:
        WindowWin(const Size2& size, bool resizable, bool fullscreen, const std::string& title);
        virtual bool init() override;

        ATOM _windowClass = 0;
        HWND _window = 0;
        DWORD _windowStyle;
    };
}
