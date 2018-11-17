// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_NATIVEWINDOWWIN_HPP
#define OUZEL_CORE_NATIVEWINDOWWIN_HPP

#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <Windows.h>
#include <set>
#include "core/NativeWindow.hpp"

namespace ouzel
{
    class EngineWin;

    class NativeWindowWin final: public NativeWindow
    {
        friend EngineWin;
    public:
        NativeWindowWin(const std::function<void(const Event&)>& initCallback,
                        const Size2& newSize,
                        bool newResizable,
                        bool newFullscreen,
                        bool newExclusiveFullscreen,
                        const std::string& newTitle,
                        bool newHighDpi);
        ~NativeWindowWin();

        void close() override;

        void setSize(const Size2& newSize) override;
        void setTitle(const std::string& newTitle) override;
        void setFullscreen(bool newFullscreen) override;

        void handleResize(const Size2& newSize);
        void handleMove();
        void handleActivateEvent(WPARAM wParam);
        void handleKeyEvent(UINT message, WPARAM wParam, LPARAM lParam);
        void handleMouseMoveEvent(LPARAM lParam);
        void handleMouseButtonEvent(UINT message, WPARAM wParam, LPARAM lParam);
        void handleMouseWheelEvent(UINT message, WPARAM wParam, LPARAM lParam);
        void handleTouchEvent(WPARAM wParam, LPARAM lParam);

        HWND getNativeWindow() const { return window; }
        void addAccelerator(HACCEL accelerator);
        void removeAccelerator(HACCEL accelerator);

        HMONITOR getMonitor() const { return monitor; }

    private:
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

#endif // OUZEL_CORE_NATIVEWINDOWWIN_HPP
