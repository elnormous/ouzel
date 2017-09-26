// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <memory>

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL
    #define GL_GLEXT_PROTOTYPES 1
    #include <GL/glx.h>
#endif

#include "WindowLinux.hpp"
#include "EngineLinux.hpp"
#include "graphics/RenderDevice.hpp"
#include "utils/Log.hpp"

static const long _NET_WM_STATE_TOGGLE = 2;

namespace ouzel
{
    WindowLinux::WindowLinux()
    {
    }

    WindowLinux::~WindowLinux()
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

    bool WindowLinux::init(const Size2& newSize,
                           bool newResizable,
                           bool newFullscreen,
                           bool newExclusiveFullscreen,
                           const std::string& newTitle,
                           bool newHighDpi,
                           bool depth)
    {
        if (!Window::init(newSize,
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

        if (size.width <= 0.0f) size.width = static_cast<float>(XWidthOfScreen(screen)) * 0.8f;
        if (size.height <= 0.0f) size.height = static_cast<float>(XHeightOfScreen(screen)) * 0.8f;

        resolution = size;

        switch (sharedEngine->getRenderer()->getDevice()->getDriver())
        {
            case graphics::Renderer::Driver::EMPTY:
            {
                XSetWindowAttributes swa;
                swa.background_pixel = XWhitePixel(display, screenIndex);
                swa.event_mask = FocusChangeMask | KeyPressMask | KeyRelease | ExposureMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask;

                window = XCreateWindow(display, RootWindow(display, screenIndex), 0, 0,
                                       static_cast<unsigned int>(size.width), static_cast<unsigned int>(size.height),
                                       0, DefaultDepth(display, screenIndex), InputOutput, DefaultVisual(display, screenIndex),
                                       CWBackPixel | CWEventMask, &swa);

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

                window = XCreateWindow(display, RootWindow(display, visualInfo->screen), 0, 0,
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

        EngineLinux* engineLinux = static_cast<EngineLinux*>(sharedEngine);

        XSetStandardProperties(display, window, title.c_str(), title.c_str(), None, engineLinux->getArgv(), engineLinux->getArgc(), nullptr);

        // request the X window to be displayed on the screen
        XMapWindow(display, window);

        deleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(display, window, &deleteMessage, 1);
        protocols = XInternAtom(display, "WM_PROTOCOLS", False);
        state = XInternAtom(display, "_NET_WM_STATE", False);
        stateFullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);

        if (fullscreen)
        {
            toggleFullscreen();
        }

        return true;
    }

    void WindowLinux::close()
    {
        Window::close();

        sharedEngine->executeOnMainThread([this] {
            XEvent event;
            event.type = ClientMessage;
            event.xclient.window = window;
            event.xclient.message_type = protocols;
            event.xclient.format = 32; // data is set as 32-bit integers
            event.xclient.data.l[0] = deleteMessage;
            event.xclient.data.l[1] = CurrentTime;
            event.xclient.data.l[2] = 0; // unused
            event.xclient.data.l[3] = 0; // unused
            event.xclient.data.l[4] = 0; // unused
            if (!XSendEvent(display, window, False, NoEventMask, &event))
            {
                Log(Log::Level::ERR) << "Failed to send X11 delete message";
            }
        });
    }

    void WindowLinux::setSize(const Size2& newSize)
    {
        if (sharedEngine->getRenderer()->getDevice()->getDriver() == graphics::Renderer::Driver::OPENGL)
        {
            sharedEngine->executeOnMainThread([this, newSize] {
                XWindowChanges changes;
                changes.width = static_cast<int>(newSize.width);
                changes.height = static_cast<int>(newSize.height);
                XConfigureWindow(display, window, CWWidth | CWHeight, &changes);

                Event resolutionChangeEvent;
                resolutionChangeEvent.type = Event::Type::RESOLUTION_CHANGE;
                resolutionChangeEvent.windowEvent.window = this;
                resolutionChangeEvent.windowEvent.size = newSize;
                sharedEngine->getEventDispatcher()->postEvent(resolutionChangeEvent);
            });
        }

        Window::setSize(newSize);
    }

    void WindowLinux::setFullscreen(bool newFullscreen)
    {
        if (sharedEngine->getRenderer()->getDevice()->getDriver() == graphics::Renderer::Driver::OPENGL)
        {
            if (fullscreen != newFullscreen)
            {
                toggleFullscreen();
            }
        }

        Window::setFullscreen(newFullscreen);
    }

    void WindowLinux::setTitle(const std::string& newTitle)
    {
        if (sharedEngine->getRenderer()->getDevice()->getDriver() == graphics::Renderer::Driver::OPENGL)
        {
            if (title != newTitle)
            {
                sharedEngine->executeOnMainThread([this, newTitle] {
                    XStoreName(display, window, newTitle.c_str());
                });
            }
        }

        Window::setTitle(newTitle);
    }

    bool WindowLinux::toggleFullscreen()
    {
        if (sharedEngine->getRenderer()->getDevice()->getDriver() == graphics::Renderer::Driver::OPENGL)
        {
            if(!state || !stateFullscreen)
            {
                return false;
            }

            sharedEngine->executeOnMainThread([this] {
                XEvent event;
                event.type = ClientMessage;
                event.xclient.window = window;
                event.xclient.message_type = state;
                event.xclient.format = 32;
                event.xclient.data.l[0] = _NET_WM_STATE_TOGGLE;
                event.xclient.data.l[1] = stateFullscreen;
                event.xclient.data.l[2] = 0; // no second property to toggle
                event.xclient.data.l[3] = 1; // source indication: application
                event.xclient.data.l[4] = 0; // unused

                if (!XSendEvent(display, DefaultRootWindow(display), 0, SubstructureRedirectMask | SubstructureNotifyMask, &event))
                {
                    Log(Log::Level::ERR) << "Failed to send X11 fullscreen message";
                }
            });
        }

        return true;
    }

    void WindowLinux::handleResize(const Size2& newSize)
    {
        Event sizeChangeEvent;
        sizeChangeEvent.type = Event::Type::WINDOW_SIZE_CHANGE;
        sizeChangeEvent.windowEvent.window = this;
        sizeChangeEvent.windowEvent.size = newSize;
        sharedEngine->getEventDispatcher()->postEvent(sizeChangeEvent);

        Event resolutionChangeEvent;
        resolutionChangeEvent.type = Event::Type::RESOLUTION_CHANGE;
        resolutionChangeEvent.windowEvent.window = this;
        resolutionChangeEvent.windowEvent.size = newSize;
        sharedEngine->getEventDispatcher()->postEvent(resolutionChangeEvent);
    }
}
