// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

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
                          const Size2<uint32_t>& newSize,
                          bool newResizable,
                          bool newFullscreen,
                          bool newExclusiveFullscreen,
                          const std::string& newTitle,
                          graphics::Driver graphicsDriver,
                          bool depth);
        ~NativeWindowLinux();

        void executeCommand(const Command& command) override;

        void close();

        void setSize(const Size2<uint32_t>& newSize);
        void setFullscreen(bool newFullscreen);
        void setTitle(const std::string& newTitle);

#if OUZEL_SUPPORTS_X11
        inline ::Window getNativeWindow() const { return window; }
        inline XVisualInfo* getVisualInfo() const { return visualInfo; }
        inline Atom getProtocolsAtom() const { return protocolsAtom; }
        inline Atom getDeleteAtom() const { return deleteAtom; }
#elif OUZEL_SUPPORTS_DISPMANX
        const EGL_DISPMANX_WINDOW_T& getNativeWindow() const { return window; }
#endif

    private:
        void handleFocusIn();
        void handleFocusOut();
        void handleResize(const Size2<uint32_t>& newSize);

#if OUZEL_SUPPORTS_X11
        XVisualInfo* visualInfo = nullptr;
        Display* display = nullptr;
        ::Window window = None;
        Atom deleteAtom = None;
        Atom protocolsAtom = None;
        Atom stateAtom = None;
        Atom stateFullscreenAtom = None;
#elif OUZEL_SUPPORTS_DISPMANX
        EGL_DISPMANX_WINDOW_T window;
#endif
    };
}

#endif // OUZEL_CORE_NATIVEWINDOWLINUX_HPP
