// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererOGLLinux.h"
#include "core/linux/WindowLinux.h"

namespace ouzel
{
    namespace graphics
    {
        RendererOGLLinux::~RendererOGLLinux()
        {
            WindowLinux* windowLinux = static_cast<WindowLinux*>(window);
            Display* display = windowLinux->getDisplay();

            if (display && context)
            {
                glXDestroyContext(display, context);
            }
        }

        bool RendererOGLLinux::init(Window* newWindow,
                                    const Size2& newSize,
                                    uint32_t newSampleCount,
                                    Texture::Filter newTextureFilter,
                                    PixelFormat newBackBufferFormat,
                                    bool newVerticalSync,
                                    bool newDepth)
        {
            WindowLinux* windowLinux = static_cast<WindowLinux*>(newWindow);
            Display* display = windowLinux->getDisplay();

            // make sure OpenGL's GLX extension supported
            int dummy;
            if (!glXQueryExtension(display, &dummy, &dummy))
            {
                Log(Log::Level::ERR) << "X server has no OpenGL GLX extension";
                return false;
            }

            Screen* screen = XDefaultScreenOfDisplay(display);
            int screenIndex = XScreenNumberOfScreen(screen);

            int fbcount = 0;

            static const int attributes[] = {
                GLX_X_RENDERABLE, GL_TRUE,
                GLX_RENDER_TYPE, GLX_RGBA_BIT,
                GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
                GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
                GLX_DOUBLEBUFFER, GL_TRUE,
                GLX_RED_SIZE, 8,
                GLX_GREEN_SIZE, 8,
                GLX_BLUE_SIZE, 8,
                GLX_DEPTH_SIZE, newDepth ? 24 : 0,
                GLX_SAMPLE_BUFFERS, (newSampleCount > 1) ? 1 : 0,
                GLX_SAMPLES, static_cast<int>(newSampleCount),
                0
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
                    0
                };

                PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = reinterpret_cast<PFNGLXCREATECONTEXTATTRIBSARBPROC>(glXGetProcAddress(reinterpret_cast<const GLubyte*>("glXCreateContextAttribsARB")));

                if (glXCreateContextAttribsARB)
                {
                    context = glXCreateContextAttribsARB(display, *framebufferConfig, NULL, GL_TRUE, contextAttribs);

                    if (context)
                    {
                        apiMajorVersion = 3;
                        apiMinorVersion = 2;
                        Log(Log::Level::INFO) << "Using OpenGL 3.2";
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
                context = glXCreateContext(display, windowLinux->getVisualInfo(), None, GL_TRUE);

                if (context)
                {
                    apiMajorVersion = 2;
                    apiMinorVersion = 0;
                    Log(Log::Level::INFO) << "Using OpenGL 2";
                }
                else
                {
                    Log(Log::Level::ERR) << "Failed to crete OpenGL 2 rendering context";
                    return false;
                }
            }

            // bind the rendering context to the window
            glXMakeCurrent(display, windowLinux->getNativeWindow(), context);

            PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddress(reinterpret_cast<const GLubyte*>("glXSwapIntervalEXT"));

            if (glXSwapIntervalEXT)
            {
                glXSwapIntervalEXT(display, windowLinux->getNativeWindow(), newVerticalSync ? 1 : 0);
            }

            return RendererOGL::init(newWindow, newSize, newSampleCount, newTextureFilter, newBackBufferFormat, newVerticalSync, newDepth);
        }

        bool RendererOGLLinux::swapBuffers()
        {
            WindowLinux* windowLinux = static_cast<WindowLinux*>(window);

            glXSwapBuffers(windowLinux->getDisplay(), windowLinux->getNativeWindow());

            return true;
        }
    } // namespace graphics
} // namespace ouzel
