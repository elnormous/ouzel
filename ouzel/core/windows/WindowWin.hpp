// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <windows.h>
#include <set>
#include "core/Window.hpp"

namespace ouzel
{
    class EngineWin;

    class WindowWin : public Window
    {
        friend Engine;
        friend EngineWin;
    public:
        virtual ~WindowWin();

        virtual void close() override;

        virtual void setSize(const Size2& newSize) override;
        virtual void setTitle(const std::string& newTitle) override;
        virtual void setFullscreen(bool newFullscreen) override;

        void handleResize(const Size2& newSize);
        void handleMove();

        HWND getNativeWindow() const { return window; }
        void addAccelerator(HACCEL accelerator);
        void removeAccelerator(HACCEL accelerator);

        HMONITOR getMonitor() const { return monitor; }

    protected:
        WindowWin();
        virtual bool init(const Size2& newSize,
                          bool newResizable,
                          bool newFullscreen,
                          const std::string& newTitle,
                          bool newHighDpi,
                          bool depth) override;

        void switchFullscreen(bool newFullscreen);

        ATOM windowClass = 0;
        HWND window = nullptr;
        DWORD windowStyle = 0;
        DWORD windowWindowedStyle = 0;
        DWORD windowFullscreenStyle = 0;
        HMONITOR monitor = nullptr;

        int windowX = 0;
        int windowY = 0;
        int windowWidth = 0;
        int windowHeight = 0;

        std::set<HACCEL> accelerators;
    };
}
