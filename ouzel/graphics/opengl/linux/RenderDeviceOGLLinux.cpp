// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_PLATFORM_LINUX && OUZEL_COMPILE_OPENGL

#if OUZEL_SUPPORTS_X11
#  include <X11/Xlib.h>
#  include <X11/extensions/xf86vmode.h>
#endif
#include "RenderDeviceOGLLinux.hpp"
#include "core/Engine.hpp"
#include "core/linux/NativeWindowLinux.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace graphics
    {
        RenderDeviceOGLLinux::RenderDeviceOGLLinux():
            running(false)
        {
        }

        RenderDeviceOGLLinux::~RenderDeviceOGLLinux()
        {
            running = false;
            flushCommands();
            if (renderThread.isJoinable()) renderThread.join();

#if OUZEL_OPENGL_INTERFACE_GLX
            NativeWindowLinux* windowLinux = static_cast<NativeWindowLinux*>(window->getNativeWindow());

            if (windowLinux->getDisplay() && context)
            {
                glXMakeCurrent(windowLinux->getDisplay(), None, nullptr);
                glXDestroyContext(windowLinux->getDisplay(), context);
            }
#elif OUZEL_OPENGL_INTERFACE_EGL
            if (context)
            {
                eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
                eglDestroyContext(display, context);
            }

            if (surface)
                eglDestroySurface(display, surface);

            if (display)
                eglTerminate(display);
#endif
        }

        void RenderDeviceOGLLinux::init(Window* newWindow,
                                        const Size2& newSize,
                                        uint32_t newSampleCount,
                                        Texture::Filter newTextureFilter,
                                        uint32_t newMaxAnisotropy,
                                        bool newVerticalSync,
                                        bool newDepth,
                                        bool newDebugRenderer)
        {
            NativeWindowLinux* windowLinux = static_cast<NativeWindowLinux*>(newWindow->getNativeWindow());

#if OUZEL_OPENGL_INTERFACE_GLX
            // make sure OpenGL's GLX extension supported
            int dummy;
            if (!glXQueryExtension(windowLinux->getDisplay(), &dummy, &dummy))
                throw SystemError("X server has no OpenGL GLX extension");

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

                    context = glXCreateContextAttribsProc(windowLinux->getDisplay(), *framebufferConfig, nullptr, True, contextAttribs.data());

                    if (context)
                    {
                        apiMajorVersion = 3;
                        apiMinorVersion = 2;
                        Log(Log::Level::INFO) << "GLX OpenGL 3.2 context created";
                    }
                }
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
                    throw SystemError("Failed to create GLX context");
            }

            // bind the rendering context to the window
            if (!glXMakeCurrent(windowLinux->getDisplay(), windowLinux->getNativeWindow(), context))
                throw SystemError("Failed to make GLX context current");

            PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = reinterpret_cast<PFNGLXSWAPINTERVALEXTPROC>(glXGetProcAddress(reinterpret_cast<const GLubyte*>("glXSwapIntervalEXT")));

            if (glXSwapIntervalEXT)
                glXSwapIntervalEXT(windowLinux->getDisplay(), windowLinux->getNativeWindow(), newVerticalSync ? 1 : 0);
#elif OUZEL_OPENGL_INTERFACE_EGL
            display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

            if (!display)
                throw SystemError("Failed to get display, error: " + std::to_string(eglGetError()));

            if (!eglInitialize(display, nullptr, nullptr))
                throw SystemError("Failed to initialize EGL, error: " + std::to_string(eglGetError()));

            const EGLint attributeList[] =
            {
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_ALPHA_SIZE, 8,
                EGL_DEPTH_SIZE, newDepth ? 24 : 0,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_SAMPLE_BUFFERS, (newSampleCount > 1) ? 1 : 0,
                EGL_SAMPLES, static_cast<int>(newSampleCount),
                EGL_NONE
            };
            EGLConfig config;
            EGLint numConfig;
            if (!eglChooseConfig(display, attributeList, &config, 1, &numConfig))
                throw SystemError("Failed to choose EGL config, error: " + std::to_string(eglGetError()));

            if (!eglBindAPI(EGL_OPENGL_ES_API))
                throw SystemError("Failed to bind OpenGL ES API, error: " + std::to_string(eglGetError()));

            surface = eglCreateWindowSurface(display, config, reinterpret_cast<EGLNativeWindowType>(&windowLinux->getNativeWindow()), nullptr);
            if (surface == EGL_NO_SURFACE)
                throw SystemError("Failed to create EGL window surface, error: " + std::to_string(eglGetError()));

            for (EGLint version = 3; version >= 2; --version)
            {
                std::vector<EGLint> contextAttributes =
                {
                    EGL_CONTEXT_CLIENT_VERSION, version
                };

                if (newDebugRenderer)
                {
                    contextAttributes.push_back(EGL_CONTEXT_FLAGS_KHR);
                    contextAttributes.push_back(EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR);
                }

                contextAttributes.push_back(EGL_NONE);

                context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttributes.data());

                if (context != EGL_NO_CONTEXT)
                {
                    apiMajorVersion = version;
                    apiMinorVersion = 0;
                    Log(Log::Level::INFO) << "EGL OpenGL ES " << version << " context created";
                    break;
                }
            }

            if (context == EGL_NO_CONTEXT)
                throw SystemError("Failed to create EGL context, error: " + std::to_string(eglGetError()));

            if (!eglMakeCurrent(display, surface, surface, context))
                throw SystemError("Failed to set current EGL context, error: " + std::to_string(eglGetError()));

            if (!eglSwapInterval(display, newVerticalSync ? 1 : 0))
                throw SystemError("Failed to set EGL frame interval, error: " + std::to_string(eglGetError()));            
#endif

            RenderDeviceOGL::init(newWindow,
                                  newSize,
                                  newSampleCount,
                                  newTextureFilter,
                                  newMaxAnisotropy,
                                  newVerticalSync,
                                  newDepth,
                                  newDebugRenderer);

#if OUZEL_OPENGL_INTERFACE_EGL
            if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
                throw SystemError("Failed to unset EGL context, error: " + std::to_string(eglGetError()));
#endif

            running = true;
            renderThread = Thread(std::bind(&RenderDeviceOGLLinux::main, this), "Render");
        }

        std::vector<Size2> RenderDeviceOGLLinux::getSupportedResolutions() const
        {
            std::vector<Size2> result;

#if OUZEL_OPENGL_INTERFACE_GLX
            NativeWindowLinux* windowLinux = static_cast<NativeWindowLinux*>(window->getNativeWindow());

            int modeCount;
            XF86VidModeModeInfo** modeInfo;

            XF86VidModeGetAllModeLines(windowLinux->getDisplay(), 0, &modeCount, &modeInfo);

            for (int i = 0; i < modeCount; ++i)
            {
                result.push_back(Size2(static_cast<float>(modeInfo[i]->hdisplay),
                                       static_cast<float>(modeInfo[i]->vdisplay)));
            }

            XFree(modeInfo);
#elif OUZEL_OPENGL_INTERFACE_EGL
            // TODO: return screen resolution
#endif

            return result;
        }

        void RenderDeviceOGLLinux::lockContext()
        {
#if OUZEL_OPENGL_INTERFACE_GLX
            NativeWindowLinux* windowLinux = static_cast<NativeWindowLinux*>(window->getNativeWindow());

            if (!glXMakeCurrent(windowLinux->getDisplay(), windowLinux->getNativeWindow(), context))
                throw SystemError("Failed to make GLX context current");
#elif OUZEL_OPENGL_INTERFACE_EGL
            if (!eglMakeCurrent(display, surface, surface, context))
                throw SystemError("Failed to set current EGL context, error: " + std::to_string(eglGetError()));
#endif
        }

        void RenderDeviceOGLLinux::swapBuffers()
        {
#if OUZEL_OPENGL_INTERFACE_GLX
            NativeWindowLinux* windowLinux = static_cast<NativeWindowLinux*>(window->getNativeWindow());

            glXSwapBuffers(windowLinux->getDisplay(), windowLinux->getNativeWindow());
#elif OUZEL_OPENGL_INTERFACE_EGL
            if (eglSwapBuffers(display, surface) != EGL_TRUE)
                throw SystemError("Failed to swap buffers, error: " + std::to_string(eglGetError()));
#endif
        }

        void RenderDeviceOGLLinux::main()
        {
            try
            {
                while (running) process();
            }
            catch (const std::exception& e)
            {
                Log(Log::Level::ERR) << e.what();
            }

#if OUZEL_OPENGL_INTERFACE_EGL
            if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
                Log(Log::Level::ERR) << "Failed to unset EGL context, error: " << eglGetError();
#endif
        }
    } // namespace graphics
} // namespace ouzel

#endif
