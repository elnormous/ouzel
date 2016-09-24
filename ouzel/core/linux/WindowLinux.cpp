// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glext.h>
#include "WindowLinux.h"
#include "core/Application.h"
#include "core/Engine.h"
#include "graphics/Renderer.h"
#include "graphics/opengl/RendererOGL.h"
#include "utils/Utils.h"

const long _NET_WM_STATE_TOGGLE = 2;
const float DEFAULT_WIDTH = 640.0f;
const float DEFAULT_HEIGHT= 480.0f;

namespace ouzel
{
    WindowLinux::WindowLinux(const Size2& pSize, bool pResizable, bool pFullscreen, const std::string& pTitle):
        Window(pSize, pResizable, pFullscreen, pTitle)
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
        if (sharedEngine->getRenderer()->getDriver() == graphics::Renderer::Driver::OPENGL)
        {
            std::shared_ptr<graphics::RendererOGL> rendererOGL = std::static_pointer_cast<graphics::RendererOGL>(sharedEngine->getRenderer());

            // open a connection to the X server
            display = XOpenDisplay(nullptr);

            if (!display)
            {
                log(LOG_LEVEL_ERROR, "Failed to open display");
                return false;
            }

            int screen = DefaultScreen(display);

            XVisualInfo* vi = nullptr;

            // make sure OpenGL's GLX extension supported
            int dummy;
            if (!glXQueryExtension(display, &dummy, &dummy))
            {
                log(LOG_LEVEL_ERROR, "X server has no OpenGL GLX extension");
                return false;
            }

            int fbcount = 0;

            static const int attributes[] = {
                GLX_RENDER_TYPE, GLX_RGBA_BIT,
                GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
                GLX_DOUBLEBUFFER, GL_TRUE,
                GLX_RED_SIZE, 1,
                GLX_GREEN_SIZE, 1,
                GLX_BLUE_SIZE, 1,
                None
            };

            GLXFBConfig* framebufferConfig = glXChooseFBConfig(display, screen, attributes, &fbcount);
            if (!framebufferConfig)
            {
                log(LOG_LEVEL_ERROR, "Failed to get frame buffer");
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
                    None
                };

                PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress(reinterpret_cast<const GLubyte*>("glXCreateContextAttribsARB"));

                if (glXCreateContextAttribsARB)
                {
                    context = glXCreateContextAttribsARB(display, framebufferConfig[0], NULL, GL_TRUE, contextAttribs);

                    if (context)
                    {
                        rendererOGL->setAPIVersion(3, 2);
                        log(LOG_LEVEL_INFO, "Using OpenGL 3.2");

                        vi = glXGetVisualFromFBConfig(display, framebufferConfig[0]);

                        if (!vi)
                        {
                            log(LOG_LEVEL_INFO, "Failed to get OpenGL visual");
                            context = nullptr;
                        }
                    }
                    else
                    {
                        log(LOG_LEVEL_INFO, "Failed to crete OpenGL 3.2 rendering context");
                    }
                }
                else
                {
                    log(LOG_LEVEL_INFO, "Could not find glXCreateContextAttribsARB");
                }
            }

            if (!context)
            {
                // find an OpenGL-capable RGB visual with depth buffer
                static int doubleBuffer[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};

                vi = glXChooseVisual(display, screen, doubleBuffer);
                if (!vi)
                {
                    log(LOG_LEVEL_ERROR, "Failed to choose OpenGL visual");
                    return false;
                }
                if (vi->c_class != TrueColor)
                {
                    log(LOG_LEVEL_ERROR, "TrueColor visual required for this program");
                    return false;
                }

                context = glXCreateContext(display, vi, None, GL_TRUE);

                if (context)
                {
                    rendererOGL->setAPIVersion(2, 0);
                    log(LOG_LEVEL_INFO, "Using OpenGL 2");
                }
                else
                {
                    log(LOG_LEVEL_ERROR, "Failed to crete OpenGL 2 rendering context");
                    return false;
                }
            }

            // create an X colormap since probably not using default visual
            Colormap cmap = XCreateColormap(display, RootWindow(display, vi->screen), vi->visual, AllocNone);
            XSetWindowAttributes swa;
            swa.colormap = cmap;
            swa.border_pixel = 0;
            swa.event_mask = FocusChangeMask | KeyPressMask | KeyRelease | ExposureMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask;

            if (size.width <= 0.0f) size.width = DEFAULT_WIDTH;
            if (size.height <= 0.0f) size.height = DEFAULT_HEIGHT;

            window = XCreateWindow(display, RootWindow(display, vi->screen), 0, 0,
                                   static_cast<unsigned int>(size.width), static_cast<unsigned int>(size.height), 0,
                                   vi->depth, InputOutput, vi->visual,
                                   CWBorderPixel | CWColormap | CWEventMask, &swa);
            XSetStandardProperties(display, window, title.c_str(), title.c_str(), None, sharedApplication->getArgv(), sharedApplication->getArgc(), nullptr);

            // bind the rendering context to the window
            glXMakeCurrent(display, window, context);

            // request the X window to be displayed on the screen
            XMapWindow(display, window);

            deleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
            XSetWMProtocols(display, window, &deleteMessage, 1);

            state = XInternAtom(display, "_NET_WM_STATE", False);
            stateFullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);

            PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddress(reinterpret_cast<const GLubyte*>("glXSwapIntervalEXT"));

            if (glXSwapIntervalEXT)
            {
                glXSwapIntervalEXT(display, window, sharedEngine->getSettings().verticalSync ? 1 : 0);
            }

            if (fullscreen)
            {
                toggleFullscreen();
            }
        }

        return Window::init();
    }

    void WindowLinux::setSize(const Size2& newSize)
    {
        if (sharedEngine->getRenderer()->getDriver() == graphics::Renderer::Driver::OPENGL)
        {
            sharedApplication->execute([this, newSize] {
                XWindowChanges changes;
                changes.width = static_cast<int>(newSize.width);
                changes.height = static_cast<int>(newSize.height);
                XConfigureWindow(display, window, CWWidth | CWHeight, &changes);
            });
        }

        Window::setSize(newSize);
    }

    void WindowLinux::setFullscreen(bool newFullscreen)
    {
        if (sharedEngine->getRenderer()->getDriver() == graphics::Renderer::Driver::OPENGL)
        {
            if (fullscreen != newFullscreen)
            {
                sharedApplication->execute([this, newFullscreen] {
                    toggleFullscreen();
                });
            }
        }

        Window::setFullscreen(newFullscreen);
    }

    void WindowLinux::setTitle(const std::string& newTitle)
    {
        if (sharedEngine->getRenderer()->getDriver() == graphics::Renderer::Driver::OPENGL)
        {
            if (title != newTitle)
            {
                sharedApplication->execute([this, newTitle] {
                    XStoreName(display, window, newTitle.c_str());
                });
            }
        }

        Window::setTitle(newTitle);
    }

    bool WindowLinux::toggleFullscreen()
    {
        if (sharedEngine->getRenderer()->getDriver() == graphics::Renderer::Driver::OPENGL)
        {
            if(!state || !stateFullscreen)
            {
                return false;
            }

            XEvent event;

            event.type = ClientMessage;
            event.xclient.window = window;
            event.xclient.message_type = state;
            event.xclient.format = 32;
            event.xclient.data.l[0] = _NET_WM_STATE_TOGGLE;
            event.xclient.data.l[1] = stateFullscreen;
            event.xclient.data.l[2] = 0;  /* no second property to toggle */
            event.xclient.data.l[3] = 1;  /* source indication: application */
            event.xclient.data.l[4] = 0;  /* unused */

            if(!XSendEvent(display, DefaultRootWindow(display), 0, SubstructureRedirectMask | SubstructureNotifyMask, &event))
            {
                log(LOG_LEVEL_ERROR, "Failed to send fullscreen message");
                return false;
            }
        }

        return true;
    }

    void WindowLinux::handleResize(int width, int height)
    {
        Window::setSize(Size2(width, height));
    }
}
