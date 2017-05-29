// Copyright (C) 2017 Elviss Strazdins
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

        void handleResize(const Size2& newSize);
        void handleMove();

        HWND getNativeWindow() const { return window; }
        const std::set<HACCEL>& getAccelerators() const { return accelerators; }
        void addAccelerator(HACCEL accelerator);
        void removeAccelerator(HACCEL accelerator);

        HMONITOR getMonitor() const { return monitor; }
        HCURSOR getCursor() const { return cursor; }
        void setCursor(HCURSOR newCursor) { cursor = newCursor; }

    protected:
        WindowWin(const Size2& aSize,
                  bool aResizable,
                  bool aFullscreen,
                  const std::string& aTitle,
                  bool aHighDpi);
        virtual bool init() override;

        void switchFullscreen(bool newFullscreen);

        ATOM windowClass = 0;
        HWND window = 0;
        DWORD windowStyle = 0;
        DWORD windowWindowedStyle = 0;
        DWORD windowFullscreenStyle = 0;
        HMONITOR monitor = 0;
        HCURSOR cursor = 0;

        int windowX = 0;
        int windowY = 0;
        int windowWidth = 0;
        int windowHeight = 0;

        std::set<HACCEL> accelerators;
    };
}
