// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

#include "core/Setup.h"
#include <cstdint>
#if OUZEL_SUPPORTS_X11
#  include <X11/Xlib.h>
#  include <X11/Xutil.h>
#else
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

    class NativeWindowLinux: public NativeWindow
    {
        friend EngineLinux;
    public:
        NativeWindowLinux(EventHandler& initEventHandler,
                          const Size2& newSize,
                          bool newResizable,
                          bool newFullscreen,
                          bool newExclusiveFullscreen,
                          const std::string& newTitle,
                          graphics::Renderer::Driver graphicsDriver,
                          bool depth);
        virtual ~NativeWindowLinux();

        virtual void close() override;

        virtual void setSize(const Size2& newSize) override;
        virtual void setFullscreen(bool newFullscreen) override;
        virtual void setTitle(const std::string& newTitle) override;

#if OUZEL_SUPPORTS_X11
        inline ::Window getNativeWindow() const { return window; }
        inline XVisualInfo* getVisualInfo() const { return visualInfo; }
        inline Atom getProtocolsAtom() const { return protocolsAtom; }
        inline Atom getDeleteAtom() const { return deleteAtom; }
        inline Atom getExecuteAtom() const { return executeAtom; }
#else
        const EGL_DISPMANX_WINDOW_T& getNativeWindow() const { return window; }
#endif

    private:
        virtual void toggleFullscreen();
        void handleResize(const Size2& newSize);

#if OUZEL_SUPPORTS_X11
        XVisualInfo* visualInfo = nullptr;
        Display* display = nullptr;
        ::Window window = 0;
        Atom deleteAtom;
        Atom protocolsAtom;
        Atom stateAtom;
        Atom stateFullscreenAtom;
        Atom executeAtom;
#else
        EGL_DISPMANX_WINDOW_T window;
#endif
    };
}
