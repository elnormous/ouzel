// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL && OUZEL_OPENGL_INTERFACE_GLX
#  include <GL/glx.h>
#endif

#include "NativeWindowLinux.hpp"
#include "EngineLinux.hpp"
#include "graphics/RenderDevice.hpp"
#include "utils/Errors.hpp"

#if OUZEL_SUPPORTS_X11
static const long _NET_WM_STATE_TOGGLE = 2;
#endif

namespace ouzel
{
    NativeWindowLinux::NativeWindowLinux(const std::function<void(const Event&)>& initCallback,
                                         const Size2& newSize,
                                         bool newResizable,
                                         bool newFullscreen,
                                         bool newExclusiveFullscreen,
                                         const std::string& newTitle,
                                         graphics::Driver graphicsDriver,
                                         bool depth):
        NativeWindow(initCallback,
                     newSize,
                     newResizable,
                     newFullscreen,
                     newExclusiveFullscreen,
                     newTitle,
                     true)
    {
#if OUZEL_SUPPORTS_X11
        EngineLinux* engineLinux = static_cast<EngineLinux*>(engine);
        display = engineLinux->getDisplay();

        Screen* screen = XDefaultScreenOfDisplay(display);
        int screenIndex = XScreenNumberOfScreen(screen);

        if (size.width <= 0.0F) size.width = static_cast<float>(XWidthOfScreen(screen)) * 0.8F;
        if (size.height <= 0.0F) size.height = static_cast<float>(XHeightOfScreen(screen)) * 0.8F;

        resolution = size;

        int x = XWidthOfScreen(screen) / 2 - static_cast<int>(size.width / 2.0f);
        int y = XHeightOfScreen(screen) / 2 - static_cast<int>(size.height / 2.0f);

        switch (graphicsDriver)
        {
            case graphics::Driver::EMPTY:
            {
                XSetWindowAttributes swa;
                swa.background_pixel = XWhitePixel(display, screenIndex);
                swa.border_pixel = 0;
                swa.event_mask = FocusChangeMask | KeyPressMask | KeyRelease | ExposureMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask;

                window = XCreateWindow(display, RootWindow(display, screenIndex), x, y,
                                       static_cast<unsigned int>(size.width), static_cast<unsigned int>(size.height),
                                       0, DefaultDepth(display, screenIndex), InputOutput, DefaultVisual(display, screenIndex),
                                       CWBorderPixel | CWBackPixel | CWEventMask, &swa);

                break;
            }
#  if OUZEL_COMPILE_OPENGL
            case graphics::Driver::OPENGL:
            {
                // find an OpenGL-capable RGB visual
                static int doubleBuffer[] = {
                    GLX_RGBA,
                    GLX_RED_SIZE, 8,
                    GLX_GREEN_SIZE, 8,
                    GLX_BLUE_SIZE, 8,
                    GLX_DEPTH_SIZE, depth ? 24 : 0,
                    GLX_DOUBLEBUFFER,
                    None
                };

                visualInfo = glXChooseVisual(display, screenIndex, doubleBuffer);
                if (!visualInfo)
                    throw SystemError("Failed to choose visual");

                if (visualInfo->c_class != TrueColor)
                    throw SystemError("TrueColor visual required for this program");

                // create an X colormap since probably not using default visual
                Colormap cmap = XCreateColormap(display, RootWindow(display, visualInfo->screen), visualInfo->visual, AllocNone);
                XSetWindowAttributes swa;
                swa.colormap = cmap;
                swa.border_pixel = 0;
                swa.event_mask = FocusChangeMask | KeyPressMask | KeyRelease | ExposureMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask;

                window = XCreateWindow(display, RootWindow(display, visualInfo->screen), x, y,
                                       static_cast<unsigned int>(size.width), static_cast<unsigned int>(size.height),
                                       0, visualInfo->depth, InputOutput, visualInfo->visual,
                                       CWBorderPixel | CWColormap | CWEventMask, &swa);
                break;
            }
#  endif
            default:
                throw SystemError("Unsupported render driver");
        }

        XSetStandardProperties(display, window, title.c_str(), title.c_str(), None, nullptr, 0, nullptr);

        if (!resizable)
        {
            XSizeHints sizeHints;
            sizeHints.flags = PPosition | PMinSize | PMaxSize;
            sizeHints.x = x;
            sizeHints.y = y;
            sizeHints.min_width = static_cast<int>(size.width);
            sizeHints.max_width = static_cast<int>(size.width);
            sizeHints.min_height = static_cast<int>(size.height);
            sizeHints.max_height = static_cast<int>(size.height);
            XSetWMNormalHints(display, window, &sizeHints);
        }
        else
        {
            XSizeHints sizeHints;
            sizeHints.flags = PPosition;
            sizeHints.x = x;
            sizeHints.y = y;
            XSetWMNormalHints(display, window, &sizeHints);
        }

        // request the X window to be displayed on the screen
        XMapWindow(display, window);

        protocolsAtom = XInternAtom(display, "WM_PROTOCOLS", False);
        deleteAtom = XInternAtom(display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(display, window, &deleteAtom, 1);
        stateAtom = XInternAtom(display, "_NET_WM_STATE", False);
        stateFullscreenAtom = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);
        executeAtom = XInternAtom(display, "OUZEL_EXECUTE", False);

        if (fullscreen) toggleFullscreen();
#else
        EngineLinux* engineLinux = static_cast<EngineLinux*>(engine);
        DISPMANX_DISPLAY_HANDLE_T display = engineLinux->getDisplay();

        DISPMANX_MODEINFO_T modeInfo;
        int32_t success = vc_dispmanx_display_get_info(display, &modeInfo);

        if (success < 0)
            throw SystemError("Failed to get display size");

        VC_RECT_T dstRect;
        dstRect.x = 0;
        dstRect.y = 0;
        dstRect.width = modeInfo.width;
        dstRect.height = modeInfo.height;

        VC_RECT_T srcRect;
        srcRect.x = 0;
        srcRect.y = 0;
        srcRect.width = modeInfo.width;
        srcRect.height = modeInfo.height;

        DISPMANX_UPDATE_HANDLE_T dispmanUpdate = vc_dispmanx_update_start(0);

        if (dispmanUpdate == DISPMANX_NO_HANDLE)
            throw SystemError("Failed to start display update");

        DISPMANX_ELEMENT_HANDLE_T dispmanElement = vc_dispmanx_element_add(dispmanUpdate, display,
                                                                           0, &dstRect, 0,
                                                                           &srcRect, DISPMANX_PROTECTION_NONE,
                                                                           0, 0, DISPMANX_NO_ROTATE);

        if (dispmanElement == DISPMANX_NO_HANDLE)
            throw SystemError("Failed to add display element");

        window.element = dispmanElement;
        window.width = modeInfo.width;
        window.height = modeInfo.height;
        vc_dispmanx_update_submit_sync(dispmanUpdate);

        size.width = static_cast<float>(modeInfo.width);
        size.height = static_cast<float>(modeInfo.height);
        resolution = size;
#endif
    }

    NativeWindowLinux::~NativeWindowLinux()
    {
#if OUZEL_SUPPORTS_X11
        if (visualInfo)
            XFree(visualInfo);

        if (display && window)
            XDestroyWindow(display, window);
#endif
    }

    void NativeWindowLinux::close()
    {
        NativeWindow::close();

#if OUZEL_SUPPORTS_X11
        if (!protocolsAtom || !deleteAtom) return;

        XEvent event;
        event.type = ClientMessage;
        event.xclient.window = window;
        event.xclient.message_type = protocolsAtom;
        event.xclient.format = 32; // data is set as 32-bit integers
        event.xclient.data.l[0] = deleteAtom;
        event.xclient.data.l[1] = CurrentTime;
        event.xclient.data.l[2] = 0; // unused
        event.xclient.data.l[3] = 0; // unused
        event.xclient.data.l[4] = 0; // unused
        if (!XSendEvent(display, window, False, NoEventMask, &event))
            throw SystemError("Failed to send X11 delete message");
#endif
    }

    void NativeWindowLinux::setSize(const Size2& newSize)
    {
        NativeWindow::setSize(newSize);

#if OUZEL_SUPPORTS_X11
        XWindowChanges changes;
        changes.width = static_cast<int>(size.width);
        changes.height = static_cast<int>(size.height);
        XConfigureWindow(display, window, CWWidth | CWHeight, &changes);

        if (!resizable)
        {
            XSizeHints sizeHints;
            sizeHints.flags = PMinSize | PMaxSize;
            sizeHints.min_width = static_cast<int>(size.width);
            sizeHints.max_width = static_cast<int>(size.width);
            sizeHints.min_height = static_cast<int>(size.height);
            sizeHints.max_height = static_cast<int>(size.height);
            XSetWMNormalHints(display, window, &sizeHints);
        }

        resolution = size;

        Event resolutionChangeEvent(Event::Type::RESOLUTION_CHANGE);
        resolutionChangeEvent.resolution = resolution;
        postEvent(resolutionChangeEvent);
#endif
    }

    void NativeWindowLinux::setFullscreen(bool newFullscreen)
    {
        if (fullscreen != newFullscreen) toggleFullscreen();

        NativeWindow::setFullscreen(newFullscreen);
    }

    void NativeWindowLinux::setTitle(const std::string& newTitle)
    {
#if OUZEL_SUPPORTS_X11
        if (title != newTitle) XStoreName(display, window, newTitle.c_str());
#endif

        NativeWindow::setTitle(newTitle);
    }

    void NativeWindowLinux::toggleFullscreen()
    {
#if OUZEL_SUPPORTS_X11
        if (!stateAtom)
            throw SystemError("State atom is null");

        if (!stateFullscreenAtom)
            throw SystemError("Fullscreen state atom is null");

        XEvent event;
        event.type = ClientMessage;
        event.xclient.window = window;
        event.xclient.message_type = stateAtom;
        event.xclient.format = 32;
        event.xclient.data.l[0] = _NET_WM_STATE_TOGGLE;
        event.xclient.data.l[1] = stateFullscreenAtom;
        event.xclient.data.l[2] = 0; // no second property to toggle
        event.xclient.data.l[3] = 1; // source indication: application
        event.xclient.data.l[4] = 0; // unused

        if (!XSendEvent(display, DefaultRootWindow(display), 0, SubstructureRedirectMask | SubstructureNotifyMask, &event))
            throw SystemError("Failed to send X11 fullscreen message");
#endif
    }

    void NativeWindowLinux::handleResize(const Size2& newSize)
    {
        size = newSize;
        resolution = size;

        Event sizeChangeEvent(Event::Type::SIZE_CHANGE);
        sizeChangeEvent.size = size;
        postEvent(sizeChangeEvent);

        Event resolutionChangeEvent(Event::Type::RESOLUTION_CHANGE);
        resolutionChangeEvent.resolution = resolution;
        postEvent(resolutionChangeEvent);
    }
}
