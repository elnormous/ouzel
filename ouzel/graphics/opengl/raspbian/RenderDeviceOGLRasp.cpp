// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_PLATFORM_RASPBIAN && OUZEL_COMPILE_OPENGL

#include "RenderDeviceOGLRasp.hpp"
#include "core/Engine.hpp"
#include "core/raspbian/WindowResourceRasp.hpp"
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
            if (renderThread.joinable()) renderThread.join();

            if (context)
            {
                if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
                {
                    Log(Log::Level::ERR) << "Failed to unset EGL context, error: " << eglGetError();
                }

                if (!eglDestroyContext(display, context))
                {
                    Log(Log::Level::ERR) << "Failed to destroy EGL context, error: " << eglGetError();
                }
            }

            if (surface)
            {
                if (!eglDestroySurface(display, surface))
                {
                    Log(Log::Level::ERR) << "Failed to destroy EGL surface, error: " << eglGetError();
                }
            }

            if (display)
            {
                if (!eglTerminate(display))
                {
                    Log(Log::Level::ERR) << "Failed to terminate EGL";
                }
            }
        }

        bool RenderDeviceOGLRasp::init(Window* newWindow,
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
            {
                Log(Log::Level::ERR) << "Failed to get display, error: " << eglGetError();
                return false;
            }

            if (!eglInitialize(display, nullptr, nullptr))
            {
                Log(Log::Level::ERR) << "Failed to initialize EGL, error: " << eglGetError();
                return false;
            }

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
            {
                Log(Log::Level::ERR) << "Failed to choose EGL config, error: " << eglGetError();
                return false;
            }

            if (!eglBindAPI(EGL_OPENGL_ES_API))
            {
                Log(Log::Level::ERR) << "Failed to bind OpenGL ES API, error: " << eglGetError();
                return false;
            }

            uint32_t width = static_cast<uint32_t>(newSize.width);
            uint32_t height = static_cast<uint32_t>(newSize.height);

            VC_RECT_T dstRect;
            dstRect.x = 0;
            dstRect.y = 0;
            dstRect.width = width;
            dstRect.height = height;

            VC_RECT_T srcRect;
            srcRect.x = 0;
            srcRect.y = 0;
            srcRect.width = width;
            srcRect.height = height;

            DISPMANX_DISPLAY_HANDLE_T dispmanDisplay = vc_dispmanx_display_open(0);
            DISPMANX_UPDATE_HANDLE_T dispmanUpdate = vc_dispmanx_update_start(0);

            DISPMANX_ELEMENT_HANDLE_T dispmanElement = vc_dispmanx_element_add(dispmanUpdate, dispmanDisplay,
                                                                               0, &dstRect, 0,
                                                                               &srcRect, DISPMANX_PROTECTION_NONE,
                                                                               0, 0, DISPMANX_NO_ROTATE);

            nativewindow.element = dispmanElement;
            nativewindow.width = width;
            nativewindow.height = height;
            vc_dispmanx_update_submit_sync(dispmanUpdate);

            surface = eglCreateWindowSurface(display, config, reinterpret_cast<EGLNativeWindowType>(&nativewindow), nullptr);
            if (surface == EGL_NO_SURFACE)
            {
                Log(Log::Level::ERR) << "Failed to create EGL window surface, error: " << eglGetError();
                return false;
            }

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
            {
                Log(Log::Level::ERR) << "Failed to create EGL context, error: " << eglGetError();
                return false;
            }

            if (!eglMakeCurrent(display, surface, surface, context))
            {
                Log(Log::Level::ERR) << "Failed to set current EGL context, error: " << eglGetError();
                return false;
            }

            if (!eglSwapInterval(display, newVerticalSync ? 1 : 0))
            {
                Log(Log::Level::ERR) << "Failed to set EGL frame interval, error: " << eglGetError();
                return false;
            }

            if (!RenderDeviceOGL::init(newWindow,
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

            if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
            {
                Log(Log::Level::ERR) << "Failed to unset EGL context, error: " << eglGetError();
                return false;
            }

            running = true;
            renderThread = std::thread(&RenderDeviceOGLRasp::main, this);

            return true;
        }

        bool RenderDeviceOGLRasp::lockContext()
        {
            if (!eglMakeCurrent(display, surface, surface, context))
            {
                Log(Log::Level::ERR) << "Failed to set current EGL context, error: " << eglGetError();
                return false;
            }

            return true;
        }

        bool RenderDeviceOGLRasp::swapBuffers()
        {
            if (eglSwapBuffers(display, surface) != EGL_TRUE)
            {
                Log(Log::Level::ERR) << "Failed to swap buffers, error: " << eglGetError();
                return false;
            }

            return true;
        }

        void RenderDeviceOGLRasp::main()
        {
            sharedEngine->setCurrentThreadName("Render");

            while (running)
            {
                process();
            }

            if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
            {
                Log(Log::Level::ERR) << "Failed to unset EGL context, error: " << eglGetError();
            }
        }
    } // namespace graphics
} // namespace ouzel

#endif
