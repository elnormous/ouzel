// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_NATIVEWINDOWLINUX_HPP
#define OUZEL_CORE_NATIVEWINDOWLINUX_HPP

#include "core/Setup.h"
#include <cstdint>
#if OUZEL_SUPPORTS_X11
#  include <X11/Xlib.h>
#  include <X11/Xutil.h>
#elif OUZEL_SUPPORTS_DISPMANX
#  include <bcm_host.h>
typedef struct
{
    DISPMANX_ELEMENT_HANDLE_T element;
    int width; /* This is necessary because dispmanx elements are not queriable. */
    int height;
} EGL_DISPMANX_WINDOW_T;
#endif

#include "core/NativeWindow.hpp"
#include "graphics/Renderer.hpp"

namespace ouzel
{
    class EngineLinux;

    class NativeWindowLinux final: public NativeWindow
    {
        friend EngineLinux;
    public:
        NativeWindowLinux(const std::function<void(const Event&)>& initCallback,
                          const Size2U& newSize,
                          bool newResizable,
                          bool newFullscreen,
                          bool newExclusiveFullscreen,
                          const std::string& newTitle,
                          graphics::Driver graphicsDriver,
                          bool depth);
        ~NativeWindowLinux() override;

        void executeCommand(const Command& command) final;

        void close();

        void setSize(const Size2U& newSize);
        void setFullscreen(bool newFullscreen);
        void setTitle(const std::string& newTitle);
        void bringToFront();
        void show();
        void hide();
        void minimize();
        void maximize();
        void restore();

#if OUZEL_SUPPORTS_X11
        inline auto getNativeWindow() const noexcept { return window; }
        inline auto getVisualInfo() const noexcept { return visualInfo; }
        inline auto getProtocolsAtom() const noexcept { return protocolsAtom; }
        inline auto getDeleteAtom() const noexcept { return deleteAtom; }
#elif OUZEL_SUPPORTS_DISPMANX
        auto& getNativeWindow() const noexcept { return window; }
#endif

    private:
#if OUZEL_SUPPORTS_X11
        void handleFocusIn();
        void handleFocusOut();
        void handleResize(const Size2U& newSize);
        void handleMap();
        void handleUnmap();
        bool isMapped() const;

        XVisualInfo* visualInfo = nullptr;
        Display* display = nullptr;
        int screenNumber = 0;
        ::Window window = None;
        Atom deleteAtom = None;
        Atom protocolsAtom = None;
        Atom stateAtom = None;
        Atom stateFullscreenAtom = None;
        Atom activateWindowAtom = None;
#elif OUZEL_SUPPORTS_DISPMANX
        EGL_DISPMANX_WINDOW_T window;
#endif
    };
}

#endif // OUZEL_CORE_NATIVEWINDOWLINUX_HPP
