// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.hpp"

#if OUZEL_PLATFORM_LINUX && OUZEL_SUPPORTS_OPENGL

#include "RendererOGLLinux.hpp"
#include "core/linux/WindowLinux.hpp"

namespace ouzel
{
    namespace graphics
    {
        RendererOGLLinux::RendererOGLLinux():
            running(false)
        {
        }

        RendererOGLLinux::~RendererOGLLinux()
        {
            running = false;
            flushCommands();
            if (renderThread.joinable()) renderThread.join();

            WindowLinux* windowLinux = static_cast<WindowLinux*>(window);

            if (windowLinux->getDisplay() && context)
            {
                if (!glXMakeCurrent(windowLinux->getDisplay(), None, nullptr))
                {
                    Log(Log::Level::ERR) << "Failed to unset GLX context";
                }

                glXDestroyContext(windowLinux->getDisplay(), context);
            }
        }

        bool RendererOGLLinux::init(Window* newWindow,
                                    const Size2& newSize,
                                    uint32_t newSampleCount,
                                    Texture::Filter newTextureFilter,
                                    uint32_t newMaxAnisotropy,
                                    bool newVerticalSync,
                                    bool newDepth,
                                    bool newDebugRenderer)
        {
            WindowLinux* windowLinux = static_cast<WindowLinux*>(newWindow);

            // make sure OpenGL's GLX extension supported
            int dummy;
            if (!glXQueryExtension(windowLinux->getDisplay(), &dummy, &dummy))
            {
                Log(Log::Level::ERR) << "X server has no OpenGL GLX extension";
                return false;
            }

            Screen* screen = XDefaultScreenOfDisplay(windowLinux->getDisplay());
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
                GLX_ALPHA_SIZE, 8,
                GLX_DEPTH_SIZE, newDepth ? 24 : 0,
                GLX_SAMPLE_BUFFERS, (newSampleCount > 1) ? 1 : 0,
                GLX_SAMPLES, static_cast<int>(newSampleCount),
                0
            };

            std::unique_ptr<GLXFBConfig, int(*)(void*)> framebufferConfig(glXChooseFBConfig(windowLinux->getDisplay(), screenIndex, attributes, &fbcount), XFree);
            if (framebufferConfig)
            {
                PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsProc = reinterpret_cast<PFNGLXCREATECONTEXTATTRIBSARBPROC>(glXGetProcAddress(reinterpret_cast<const GLubyte*>("glXCreateContextAttribsARB")));

                if (glXCreateContextAttribsProc)
                {
                    // create an OpenGL rendering context
                    std::vector<int> contextAttribs = {
                        GLX_CONTEXT_PROFILE_MASK_ARB,
                        GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
                        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
                        GLX_CONTEXT_MINOR_VERSION_ARB, 2
                    };

                    if (newDebugRenderer)
                    {
                        contextAttribs.push_back(GL_CONTEXT_FLAGS);
                        contextAttribs.push_back(GL_CONTEXT_FLAG_DEBUG_BIT);
                    }

                    contextAttribs.push_back(0);

                    context = glXCreateContextAttribsProc(windowLinux->getDisplay(), *framebufferConfig, NULL, True, contextAttribs.data());

                    if (context)
                    {
                        apiMajorVersion = 3;
                        apiMinorVersion = 2;
                        Log(Log::Level::INFO) << "GLX OpenGL 3.2 context created";
                    }
                }
            }
            else
            {
                Log(Log::Level::ERR) << "Failed to get frame buffer";
            }

            if (!context)
            {
                context = glXCreateContext(windowLinux->getDisplay(), windowLinux->getVisualInfo(), None, GL_TRUE);

                if (context)
                {
                    apiMajorVersion = 2;
                    apiMinorVersion = 0;
                    Log(Log::Level::INFO) << "GLX OpenGL 2 context created";
                }
                else
                {
                    Log(Log::Level::ERR) << "Failed to create GLX context";
                    return false;
                }
            }

            // bind the rendering context to the window
            if (!glXMakeCurrent(windowLinux->getDisplay(), windowLinux->getNativeWindow(), context))
            {
                Log(Log::Level::ERR) << "Failed to make GLX context current";
                return false;
            }

            PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = reinterpret_cast<PFNGLXSWAPINTERVALEXTPROC>(glXGetProcAddress(reinterpret_cast<const GLubyte*>("glXSwapIntervalEXT")));

            if (glXSwapIntervalEXT)
            {
                glXSwapIntervalEXT(windowLinux->getDisplay(), windowLinux->getNativeWindow(), newVerticalSync ? 1 : 0);
            }

            if (!RendererOGL::init(newWindow,
                                   newSize,
                                   newSampleCount,
                                   newTextureFilter,
                                   newMaxAnisotropy,
                                   newVerticalSync,
                                   newDepth,
                                   newDebugRenderer))
            {
                return false;
            }

            running = true;
            renderThread = std::thread(&RendererOGLLinux::main, this);

            return true;
        }

        bool RendererOGLLinux::lockContext()
        {
            WindowLinux* windowLinux = static_cast<WindowLinux*>(window);

            if (!glXMakeCurrent(windowLinux->getDisplay(), windowLinux->getNativeWindow(), context))
            {
                Log(Log::Level::ERR) << "Failed to make GLX context current";
                return false;
            }

            return true;
        }

        bool RendererOGLLinux::swapBuffers()
        {
            WindowLinux* windowLinux = static_cast<WindowLinux*>(window);

            glXSwapBuffers(windowLinux->getDisplay(), windowLinux->getNativeWindow());

            return true;
        }

        void RendererOGLLinux::main()
        {
            while (running)
            {
                process();
            }
        }
    } // namespace graphics
} // namespace ouzel

#endif
