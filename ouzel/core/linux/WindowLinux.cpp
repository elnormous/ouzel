// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <memory>
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glext.h>
#include "WindowLinux.h"
#include "core/Application.h"
#include "core/Engine.h"
#include "graphics/Renderer.h"
#include "graphics/opengl/RendererOGL.h"
#include "utils/Log.h"

static const long _NET_WM_STATE_TOGGLE = 2;

namespace ouzel
{
    WindowLinux::WindowLinux(const Size2& aSize, bool aResizable, bool aFullscreen, const std::string& aTitle):
        Window(aSize, aResizable, aFullscreen, aTitle)
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
            graphics::RendererOGL* rendererOGL = static_cast<graphics::RendererOGL*>(sharedEngine->getRenderer());

            // open a connection to the X server
            display = XOpenDisplay(nullptr);

            if (!display)
            {
                Log(Log::Level::ERR) << "Failed to open display";
                return false;
            }

            Screen* screen = XDefaultScreenOfDisplay(display);
            int screenIndex = XScreenNumberOfScreen(screen);

            std::unique_ptr<XVisualInfo, int(*)(void*)> visualInfo(nullptr, XFree);

            // make sure OpenGL's GLX extension supported
            int dummy;
            if (!glXQueryExtension(display, &dummy, &dummy))
            {
                Log(Log::Level::ERR) << "X server has no OpenGL GLX extension";
                return false;
            }

            int fbcount = 0;

            static const int attributes[] = {
                GLX_RENDER_TYPE, GLX_RGBA_BIT,
                GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
                GLX_DOUBLEBUFFER, GL_TRUE,
                GLX_RED_SIZE, 8,
                GLX_GREEN_SIZE, 8,
                GLX_BLUE_SIZE, 8,
                GLX_ALPHA_SIZE, 8,
                None
            };

            std::unique_ptr<GLXFBConfig, int(*)(void*)> framebufferConfig(glXChooseFBConfig(display, screenIndex, attributes, &fbcount), XFree);
            if (!framebufferConfig)
            {
                Log(Log::Level::ERR) << "Failed to get frame buffer";
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

                PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = reinterpret_cast<PFNGLXCREATECONTEXTATTRIBSARBPROC>(glXGetProcAddress(reinterpret_cast<const GLubyte*>("glXCreateContextAttribsARB")));

                if (glXCreateContextAttribsARB)
                {
                    context = glXCreateContextAttribsARB(display, *framebufferConfig, NULL, GL_TRUE, contextAttribs);

                    if (context)
                    {
                        rendererOGL->setAPIVersion(3, 2);
                        Log(Log::Level::INFO) << "Using OpenGL 3.2";

                        visualInfo.reset(glXGetVisualFromFBConfig(display, *framebufferConfig));

                        if (!visualInfo)
                        {
                            Log(Log::Level::INFO) << "Failed to get OpenGL visual";
                            context = nullptr;
                        }
                    }
                    else
                    {
                        Log(Log::Level::INFO) << "Failed to crete OpenGL 3.2 rendering context";
                    }
                }
                else
                {
                    Log(Log::Level::INFO) << "Could not find glXCreateContextAttribsARB";
                }
            }

            if (!context)
            {
                // find an OpenGL-capable RGB visual
                static int doubleBuffer[] = {
                    GLX_RGBA,
                    GLX_RED_SIZE, 8,
                    GLX_GREEN_SIZE, 8,
                    GLX_BLUE_SIZE, 8,
                    GLX_ALPHA_SIZE, 8,
                    GLX_DOUBLEBUFFER,
                    None
                };

                visualInfo.reset(glXChooseVisual(display, screenIndex, doubleBuffer));
                if (!visualInfo)
                {
                    Log(Log::Level::ERR) << "Failed to choose OpenGL visual";
                    return false;
                }
                if (visualInfo->c_class != TrueColor)
                {
                    Log(Log::Level::ERR) << "TrueColor visual required for this program";
                    return false;
                }

                context = glXCreateContext(display, visualInfo.get(), None, GL_TRUE);

                if (context)
                {
                    rendererOGL->setAPIVersion(2, 0);
                    Log(Log::Level::INFO) << "Using OpenGL 2";
                }
                else
                {
                    Log(Log::Level::ERR) << "Failed to crete OpenGL 2 rendering context";
                    return false;
                }
            }

            // create an X colormap since probably not using default visual
            Colormap cmap = XCreateColormap(display, RootWindow(display, visualInfo->screen), visualInfo->visual, AllocNone);
            XSetWindowAttributes swa;
            swa.colormap = cmap;
            swa.border_pixel = 0;
            swa.event_mask = FocusChangeMask | KeyPressMask | KeyRelease | ExposureMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask;

            if (size.v[0] <= 0.0f) size.v[0] = static_cast<float>(XWidthOfScreen(screen)) * 0.8f;
            if (size.v[1] <= 0.0f) size.v[1] = static_cast<float>(XHeightOfScreen(screen)) * 0.8f;

            window = XCreateWindow(display, RootWindow(display, visualInfo->screen), 0, 0,
                                   static_cast<unsigned int>(size.v[0]), static_cast<unsigned int>(size.v[1]),
                                   0, visualInfo->depth, InputOutput, visualInfo->visual,
                                   CWBorderPixel | CWColormap | CWEventMask, &swa);
            XSetStandardProperties(display, window, title.c_str(), title.c_str(), None, sharedApplication->getArgv(), sharedApplication->getArgc(), nullptr);

            // bind the rendering context to the window
            glXMakeCurrent(display, window, context);

            // request the X window to be displayed on the screen
            XMapWindow(display, window);

            deleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
            XSetWMProtocols(display, window, &deleteMessage, 1);
            protocols = XInternAtom(display, "WM_PROTOCOLS", False);
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

    void WindowLinux::close()
    {
        Window::close();

        ouzel::sharedApplication->execute([this] {
            Display *display = display;
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
            XSendEvent(display, window, False, NoEventMask, &event);
        });
    }

    void WindowLinux::setSize(const Size2& newSize)
    {
        if (sharedEngine->getRenderer()->getDriver() == graphics::Renderer::Driver::OPENGL)
        {
            sharedApplication->execute([this, newSize] {
                XWindowChanges changes;
                changes.width = static_cast<int>(newSize.v[0]);
                changes.height = static_cast<int>(newSize.v[1]);
                XConfigureWindow(display, window, CWWidth | CWHeight, &changes);
            });
        }

        Window::setSize(newSize);
        sharedEngine->getRenderer()->setSize(size);
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
            event.xclient.data.l[2] = 0; // no second property to toggle
            event.xclient.data.l[3] = 1; // source indication: application
            event.xclient.data.l[4] = 0; // unused

            if(!XSendEvent(display, DefaultRootWindow(display), 0, SubstructureRedirectMask | SubstructureNotifyMask, &event))
            {
                Log(Log::Level::ERR) << "Failed to send fullscreen message";
                return false;
            }
        }

        return true;
    }

    void WindowLinux::handleResize(const Size2& newSize)
    {
        Window::setSize(newSize);
        sharedEngine->getRenderer()->setSize(size * getContentScale());
    }
}
