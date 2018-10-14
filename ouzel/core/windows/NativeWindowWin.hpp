// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <Windows.h>
#include <set>
#include "core/NativeWindow.hpp"

namespace ouzel
{
    class Window;
    class EngineWin;

    class NativeWindowWin : public NativeWindow
    {
        friend Window;
        friend EngineWin;
    public:
        virtual ~NativeWindowWin();

        virtual void close() override;

        virtual void setSize(const Size2& newSize) override;
        virtual void setTitle(const std::string& newTitle) override;
        virtual void setFullscreen(bool newFullscreen) override;

        void handleResize(const Size2& newSize);
        void handleMove();
        void handleKeyEvent(UINT message, WPARAM wParam, LPARAM lParam);
        void handleMouseMoveEvent(UINT, WPARAM wParam, LPARAM lParam);
        void handleMouseButtonEvent(UINT message, WPARAM wParam, LPARAM lParam);
        void handleMouseWheelEvent(UINT message, WPARAM wParam, LPARAM lParam);
        void handleTouchEvent(WPARAM wParam, LPARAM lParam);

        HWND getNativeWindow() const { return window; }
        void addAccelerator(HACCEL accelerator);
        void removeAccelerator(HACCEL accelerator);

        HMONITOR getMonitor() const { return monitor; }

    protected:
        NativeWindowWin(EventHandler& initEventHandler,
                        const Size2& newSize,
                        bool newResizable,
                        bool newFullscreen,
                        bool newExclusiveFullscreen,
                        const std::string& newTitle,
                        bool newHighDpi);

        void switchFullscreen(bool newFullscreen);

        ATOM windowClass = 0;
        HWND window = nullptr;
        DWORD windowStyle = 0;
        DWORD windowExStyle = 0;
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
