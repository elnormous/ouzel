// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL
#define GL_GLEXT_PROTOTYPES 1
#include <GL/glx.h>
#endif

#include "WindowResourceLinux.hpp"
#include "EngineLinux.hpp"
#include "graphics/RenderDevice.hpp"
#include "thread/Lock.hpp"
#include "utils/Log.hpp"

static const long _NET_WM_STATE_TOGGLE = 2;

namespace ouzel
{
    WindowResourceLinux::WindowResourceLinux()
    {
    }

    WindowResourceLinux::~WindowResourceLinux()
    {
        if (visualInfo)
        {
            XFree(visualInfo);
        }

        if (display)
        {
            if (window)
            {
                XDestroyWindow(display, window);
            }

            XCloseDisplay(display);
        }
    }

    bool WindowResourceLinux::init(const Size2& newSize,
                                   bool newResizable,
                                   bool newFullscreen,
                                   bool newExclusiveFullscreen,
                                   const std::string& newTitle,
                                   bool newHighDpi,
                                   bool depth)
    {
        if (!WindowResource::init(newSize,
                                  newResizable,
                                  newFullscreen,
                                  newExclusiveFullscreen,
                                  newTitle,
                                  newHighDpi,
                                  depth))
        {
            return false;
        }

        // open a connection to the X server
        display = XOpenDisplay(nullptr);

        if (!display)
        {
            Log(Log::Level::ERR) << "Failed to open display";
            return false;
        }

        Screen* screen = XDefaultScreenOfDisplay(display);
        int screenIndex = XScreenNumberOfScreen(screen);

        if (size.width <= 0.0F) size.width = static_cast<float>(XWidthOfScreen(screen)) * 0.8F;
        if (size.height <= 0.0F) size.height = static_cast<float>(XHeightOfScreen(screen)) * 0.8F;

        resolution = size;

        int x = XWidthOfScreen(screen) / 2 - static_cast<int>(size.width / 2.0f);
        int y = XHeightOfScreen(screen) / 2 - static_cast<int>(size.height / 2.0f);

        switch (engine->getRenderer()->getDevice()->getDriver())
        {
            case graphics::Renderer::Driver::EMPTY:
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
#if OUZEL_COMPILE_OPENGL
            case graphics::Renderer::Driver::OPENGL:
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
                {
                    Log(Log::Level::ERR) << "Failed to choose visual";
                    return false;
                }
                if (visualInfo->c_class != TrueColor)
                {
                    Log(Log::Level::ERR) << "TrueColor visual required for this program";
                    return false;
                }

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
#endif
            default:
                Log(Log::Level::ERR) << "Unsupported render driver";
                return false;
        }

        EngineLinux* engineLinux = static_cast<EngineLinux*>(engine);

        XSetStandardProperties(display, window, title.c_str(), title.c_str(), None, engineLinux->getArgv(), engineLinux->getArgc(), nullptr);

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

        return true;
    }

    void WindowResourceLinux::close()
    {
        WindowResource::close();

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
        {
            Log(Log::Level::ERR) << "Failed to send X11 delete message";
        }
    }

    void WindowResourceLinux::setSize(const Size2& newSize)
    {
        WindowResource::setSize(newSize);

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

        Lock lock(listenerMutex);
        if (listener)
        {
            listener->onResolutionChange(resolution);
        }
    }

    void WindowResourceLinux::setFullscreen(bool newFullscreen)
    {
        if (fullscreen != newFullscreen) toggleFullscreen();

        WindowResource::setFullscreen(newFullscreen);
    }

    void WindowResourceLinux::setTitle(const std::string& newTitle)
    {
        if (title != newTitle) XStoreName(display, window, newTitle.c_str());

        WindowResource::setTitle(newTitle);
    }

    bool WindowResourceLinux::toggleFullscreen()
    {
        if (!stateAtom || !stateFullscreenAtom) return false;

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
        {
            Log(Log::Level::ERR) << "Failed to send X11 fullscreen message";
        }

        return true;
    }

    void WindowResourceLinux::handleResize(const Size2& newSize)
    {
        size = newSize;
        resolution = size;

        Lock lock(listenerMutex);
        if (listener)
        {
            listener->onSizeChange(size);
            listener->onResolutionChange(resolution);
        }
    }
}
