// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_NATIVEWINDOWLINUX_HPP
#define OUZEL_CORE_NATIVEWINDOWLINUX_HPP

#include "../Setup.h"
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

#include "../NativeWindow.hpp"

namespace ouzel::core::linux
{
    class Engine;

    class NativeWindow final: public core::NativeWindow
    {
        friend Engine;
    public:
        NativeWindow(const std::function<void(const Event&)>& initCallback,
                     const Size2U& newSize,
                     bool newResizable,
                     bool newFullscreen,
                     bool newExclusiveFullscreen,
                     const std::string& newTitle);
        ~NativeWindow() override;

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

        auto& getNativeWindow() const noexcept { return window; }

#if OUZEL_SUPPORTS_X11
        auto& getDisplay() const noexcept { return display; }
        auto getProtocolsAtom() const noexcept { return protocolsAtom; }
        auto getDeleteAtom() const noexcept { return deleteAtom; }
#endif

    private:
        void executeCommand(const Command& command) final;

#if OUZEL_SUPPORTS_X11
        void handleFocusIn();
        void handleFocusOut();
        void handleResize(const Size2U& newSize);
        void handleMap();
        void handleUnmap();
        bool isMapped() const;

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
