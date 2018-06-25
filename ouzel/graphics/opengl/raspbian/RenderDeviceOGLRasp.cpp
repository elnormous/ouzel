// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_PLATFORM_RASPBIAN && OUZEL_COMPILE_OPENGL

#include "RenderDeviceOGLRasp.hpp"
#include "core/Engine.hpp"
#include "core/raspbian/WindowResourceRasp.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace graphics
    {
        RenderDeviceOGLRasp::RenderDeviceOGLRasp():
            running(false)
        {
        }

        RenderDeviceOGLRasp::~RenderDeviceOGLRasp()
        {
            running = false;
            flushCommands();
            if (renderThread.isJoinable()) renderThread.join();

            if (context)
            {
                eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
                eglDestroyContext(display, context);
            }

            if (surface)
                eglDestroySurface(display, surface);

            if (display)
                eglTerminate(display);
        }

        void RenderDeviceOGLRasp::init(Window* newWindow,
                                       const Size2& newSize,
                                       uint32_t newSampleCount,
                                       Texture::Filter newTextureFilter,
                                       uint32_t newMaxAnisotropy,
                                       bool newVerticalSync,
                                       bool newDepth,
                                       bool newDebugRenderer)
        {
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

            WindowResourceRasp* windowRasp = static_cast<WindowResourceRasp*>(newWindow->getResource());

            surface = eglCreateWindowSurface(display, config, reinterpret_cast<EGLNativeWindowType>(&windowRasp->getNativeWindow()), nullptr);
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

            RenderDeviceOGL::init(newWindow,
                                  newSize,
                                  newSampleCount,
                                  newTextureFilter,
                                  newMaxAnisotropy,
                                  newVerticalSync,
                                  newDepth,
                                  newDebugRenderer);

            if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
                throw SystemError("Failed to unset EGL context, error: " + std::to_string(eglGetError()));

            running = true;
            renderThread = Thread(std::bind(&RenderDeviceOGLRasp::main, this), "Render");
        }

        void RenderDeviceOGLRasp::lockContext()
        {
            if (!eglMakeCurrent(display, surface, surface, context))
                throw SystemError("Failed to set current EGL context, error: " + std::to_string(eglGetError()));
        }

        void RenderDeviceOGLRasp::swapBuffers()
        {
            if (eglSwapBuffers(display, surface) != EGL_TRUE)
                throw SystemError("Failed to swap buffers, error: " + std::to_string(eglGetError()));
        }

        void RenderDeviceOGLRasp::main()
        {
            while (running) process();

            if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
                Log(Log::Level::ERR) << "Failed to unset EGL context, error: " << eglGetError();
        }
    } // namespace graphics
} // namespace ouzel

#endif
