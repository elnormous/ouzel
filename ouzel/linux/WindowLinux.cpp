// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "WindowLinux.h"
#include "Engine.h"
#include "Renderer.h"
#include "opengl/RendererOGL.h"
#include "Utils.h"

namespace ouzel
{
    WindowLinux::WindowLinux(const Size2& pSize, bool pResizable, bool pFullscreen, uint32_t pSampleCount, const std::string& pTitle):
        Window(pSize, pResizable, pFullscreen, pSampleCount, pTitle)
    {

    }

    WindowLinux::~WindowLinux()
    {
        if (display)
        {
            if (context)
            {
                glXDestroyContext(display, context);
            }

            if (window)
            {
                XDestroyWindow(display, window);
            }

            XCloseDisplay(display);
        }
    }

    bool WindowLinux::init()
    {
        std::shared_ptr<graphics::RendererOGL> rendererOGL = std::static_pointer_cast<graphics::RendererOGL>(sharedEngine->getRenderer());

        // open a connection to the X server
        display = XOpenDisplay(nullptr);

        if (!display)
        {
            ouzel::log("Failed to open display");
            return false;
        }

        // find an OpenGL-capable RGB visual with depth buffer
        static int doubleBuffer[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};

        XVisualInfo* vi = glXChooseVisual(display, DefaultScreen(display), doubleBuffer);
        if (!vi)
        {
            ouzel::log("No RGB visual with depth buffer");
            return false;
        }
        if (vi->c_class != TrueColor)
        {
            ouzel::log("TrueColor visual required for this program");
            return false;
        }

        // make sure OpenGL's GLX extension supported
        int dummy;
        if (!glXQueryExtension(display, &dummy, &dummy))
        {
            ouzel::log("X server has no OpenGL GLX extension");
            return false;
        }

        int fbcount = 0;
        GLXFBConfig* framebufferConfig = glXChooseFBConfig(display, DefaultScreen(display), NULL, &fbcount);
        if (!framebufferConfig)
        {
            ouzel::log("Failed to get frame buffer");
        }
        else
        {
            // create an OpenGL rendering context
            static const int contextAttribs[] = {
                GLX_CONTEXT_PROFILE_MASK_ARB,
                GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
                GLX_CONTEXT_MAJOR_VERSION_ARB,
                3,
                GLX_CONTEXT_MINOR_VERSION_ARB,
                2,
                0,
            };

            PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress(reinterpret_cast<const GLubyte*>("glXCreateContextAttribsARB"));

            if (glXCreateContextAttribsARB)
            {
                context = glXCreateContextAttribsARB(display, framebufferConfig[0], NULL, GL_TRUE, contextAttribs);

                if (context)
                {
                    rendererOGL->setOpenGLVersion(3);
                    log("Using OpenGL 3.2");
                }
                else
                {
                    log("Failed to crete OpenGL 3.2 rendering context");
                }
            }
            else
            {
                log("Could not find glXCreateContextAttribsARB");
            }
        }

        if (!context)
        {
            context = glXCreateContext(display, vi, None, GL_TRUE);

            if (context)
            {
                rendererOGL->setOpenGLVersion(2);
                log("Using OpenGL 2");
            }
            else
            {
                log("Failed to crete OpenGL 2 rendering context");
                return false;
            }
        }

        // create an X colormap since probably not using default visual
        Colormap cmap = XCreateColormap(display, RootWindow(display, vi->screen), vi->visual, AllocNone);
        XSetWindowAttributes swa;
        swa.colormap = cmap;
        swa.border_pixel = 0;
        swa.event_mask = KeyPressMask | KeyRelease | ExposureMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask;
        window = XCreateWindow(display, RootWindow(display, vi->screen), 0, 0,
                               static_cast<unsigned int>(size.width), static_cast<unsigned int>(size.height), 0,
                               vi->depth, InputOutput, vi->visual,
                               CWBorderPixel | CWColormap | CWEventMask, &swa);
        XSetStandardProperties(display, window, title.c_str(), title.c_str(), None, getArgv(), getArgc(), nullptr);

        // bind the rendering context to the window
        glXMakeCurrent(display, window, context);

        // request the X window to be displayed on the screen
        XMapWindow(display, window);

        return Window::init();
    }

    void WindowLinux::setSize(const Size2& newSize)
    {
        Window::setSize(newSize);
    }

    void WindowLinux::setFullscreen(bool newFullscreen)
    {
        Window::setFullscreen(newFullscreen);
    }

    void WindowLinux::setTitle(const std::string& newTitle)
    {
        Window::setTitle(newTitle);
    }
}
