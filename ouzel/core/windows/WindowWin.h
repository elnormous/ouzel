// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#define NOMINMAX
#include <windows.h>
#include <set>
#include "core/Window.h"

namespace ouzel
{
    class WindowWin : public Window
    {
        friend Engine;
    public:
        virtual ~WindowWin();

        virtual void close() override;

        virtual void setSize(const Size2& newSize) override;
        virtual void setTitle(const std::string& newTitle) override;
        virtual void setFullscreen(bool newFullscreen) override;

        void handleResize(INT width, INT height);

        HWND getNativeWindow() const { return window; }
        const std::set<HACCEL>& getAccelerators() const { return accelerators; }
        void addAccelerator(HACCEL accelerator);
        void removeAccelerator(HACCEL accelerator);
        HMONITOR getMonitor() const;

    protected:
        WindowWin(const Size2& pSize, bool pResizable, bool pFullscreen, const std::string& pTitle);
        virtual bool init() override;

        ATOM windowClass = 0;
        HWND window = 0;
        DWORD windowStyle = 0;
        std::set<HACCEL> accelerators;
    };
}
