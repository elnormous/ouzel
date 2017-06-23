// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENGL

#include "RendererOGLAndroid.h"
#include "core/android/WindowAndroid.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        RendererOGLAndroid::~RendererOGLAndroid()
        {
            if (context)
            {
                if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
                {
                    Log(Log::Level::ERR) << "Failed to unset EGL context";
                }

                if (!eglDestroyContext(display, context))
                {
                    Log(Log::Level::ERR) << "Failed to destroy EGL context";
                }
            }

            if (surface)
            {
                if (!eglDestroySurface(display, surface))
                {
                    Log(Log::Level::ERR) << "Failed to destroy EGL surface";
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

        bool RendererOGLAndroid::init(Window* newWindow,
                                      const Size2&,
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
                Log(Log::Level::ERR) << "Failed to get display";
                return false;
            }

            if (!eglInitialize(display, nullptr, nullptr))
            {
                Log(Log::Level::ERR) << "Failed to initialize EGL";
                return false;
            }

            const EGLint attributeList[] =
            {
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_ALPHA_SIZE, 8,
                EGL_DEPTH_SIZE, newDepth ? 24 : 0,
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_SAMPLE_BUFFERS, (newSampleCount > 1) ? 1 : 0,
                EGL_SAMPLES, static_cast<int>(newSampleCount),
                EGL_NONE
            };
            EGLConfig config;
            EGLint numConfig;
            if (!eglChooseConfig(display, attributeList, &config, 1, &numConfig))
            {
                Log(Log::Level::ERR) << "Failed to choose EGL config";
                return false;
            }

            if (!eglBindAPI(EGL_OPENGL_ES_API))
            {
                Log(Log::Level::ERR) << "Failed to bind OpenGL ES API";
                return false;
            }

            EGLint format;
            if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format))
            {
                Log(Log::Level::ERR) << "Failed to get config attribute " << eglGetError();
                return false;
            }

            WindowAndroid* windowAndroid = static_cast<WindowAndroid*>(newWindow);

            ANativeWindow_setBuffersGeometry(windowAndroid->getNativeWindow(), 0, 0, format);

            surface = eglCreateWindowSurface(display, config, windowAndroid->getNativeWindow(), nullptr);
            if (surface == EGL_NO_SURFACE)
            {
                Log(Log::Level::ERR) << "Failed to create EGL window surface";
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
                Log(Log::Level::ERR) << "Failed to create EGL context";
                return false;
            }

            if (!eglMakeCurrent(display, surface, surface, context))
            {
                Log(Log::Level::ERR) << "Failed to set current EGL context";
                return false;
            }

            if (!eglSwapInterval(display, newVerticalSync ? 1 : 0))
            {
                Log(Log::Level::ERR) << "Failed to set EGL frame interval";
                return false;
            }

            EGLint frameBufferWidth;
            EGLint frameBufferHeight;

            if (!eglQuerySurface(display, surface, EGL_WIDTH, &frameBufferWidth) ||
                !eglQuerySurface(display, surface, EGL_HEIGHT, &frameBufferHeight))
            {
                Log(Log::Level::ERR) << "Failed to get query window size " <<  eglGetError();
                return false;
            }

            Size2 backBufferSize = Size2(static_cast<float>(frameBufferWidth),
                                         static_cast<float>(frameBufferHeight));

            newWindow->setSize(backBufferSize / newWindow->getContentScale());

            return RendererOGL::init(newWindow,
                                     backBufferSize,
                                     newSampleCount,
                                     newTextureFilter,
                                     newMaxAnisotropy,
                                     newVerticalSync,
                                     newDepth,
                                     newDebugRenderer);
        }

        bool RendererOGLAndroid::swapBuffers()
        {
            if (eglSwapBuffers(display, surface) != EGL_TRUE)
            {
                Log(Log::Level::ERR) << "Failed to swap buffers " << eglGetError();
                return false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
