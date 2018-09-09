// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if OUZEL_PLATFORM_ANDROID && OUZEL_COMPILE_OPENGL

#include "RenderDeviceOGLAndroid.hpp"
#include "core/android/NativeWindowAndroid.hpp"
#include "core/Engine.hpp"
#include "utils/Errors.hpp"
#include "utils/Log.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    namespace graphics
    {
        RenderDeviceOGLAndroid::~RenderDeviceOGLAndroid()
        {
            running = false;
            flushCommands();
            if (renderThread.joinable()) renderThread.join();

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

        void RenderDeviceOGLAndroid::init(Window* newWindow,
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
                throw SystemError("Failed to get display");

            if (!eglInitialize(display, nullptr, nullptr))
                throw SystemError("Failed to initialize EGL");

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
                throw SystemError("Failed to choose EGL config");

            if (!eglBindAPI(EGL_OPENGL_ES_API))
                throw SystemError("Failed to bind OpenGL ES API");

            EGLint format;
            if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format))
                throw SystemError("Failed to get config attribute " + std::to_string(eglGetError()));

            NativeWindowAndroid* windowAndroid = static_cast<NativeWindowAndroid*>(newWindow->getNativeWindow());

            ANativeWindow_setBuffersGeometry(windowAndroid->getNativeWindow(), 0, 0, format);

            surface = eglCreateWindowSurface(display, config, windowAndroid->getNativeWindow(), nullptr);
            if (surface == EGL_NO_SURFACE)
                throw SystemError("Failed to create EGL window surface");

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
                throw SystemError("Failed to create EGL context");

            if (!eglMakeCurrent(display, surface, surface, context))
                throw SystemError("Failed to set current EGL context");

            if (!eglSwapInterval(display, newVerticalSync ? 1 : 0))
                throw SystemError("Failed to set EGL frame interval");

            EGLint surfaceWidth;
            EGLint surfaceHeight;

            if (!eglQuerySurface(display, surface, EGL_WIDTH, &surfaceWidth) ||
                !eglQuerySurface(display, surface, EGL_HEIGHT, &surfaceHeight))
                throw SystemError("Failed to get query window size, error: " + std::to_string(eglGetError()));

            frameBufferWidth = surfaceWidth;
            frameBufferHeight = surfaceHeight;

            Size2 backBufferSize = Size2(static_cast<float>(frameBufferWidth),
                                         static_cast<float>(frameBufferHeight));

            RenderDeviceOGL::init(newWindow,
                                  backBufferSize,
                                  newSampleCount,
                                  newTextureFilter,
                                  newMaxAnisotropy,
                                  newVerticalSync,
                                  newDepth,
                                  newDebugRenderer);

            if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
                throw SystemError("Failed to unset EGL context");

            running = true;
            renderThread = std::thread(&RenderDeviceOGLAndroid::main, this);
        }

        void RenderDeviceOGLAndroid::reload()
        {
            running = false;
            flushCommands();
            if (renderThread.joinable()) renderThread.join();

            const EGLint attributeList[] =
            {
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_ALPHA_SIZE, 8,
                EGL_DEPTH_SIZE, depth ? 24 : 0,
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_SAMPLE_BUFFERS, (sampleCount > 1) ? 1 : 0,
                EGL_SAMPLES, static_cast<int>(sampleCount),
                EGL_NONE
            };
            EGLConfig config;
            EGLint numConfig;
            if (!eglChooseConfig(display, attributeList, &config, 1, &numConfig))
                throw SystemError("Failed to choose EGL config");

            if (!eglBindAPI(EGL_OPENGL_ES_API))
                throw SystemError("Failed to bind OpenGL ES API");

            EGLint format;
            if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format))
                throw SystemError("Failed to get config attribute " + std::to_string(eglGetError()));

            NativeWindowAndroid* windowAndroid = static_cast<NativeWindowAndroid*>(window->getNativeWindow());

            ANativeWindow_setBuffersGeometry(windowAndroid->getNativeWindow(), 0, 0, format);

            surface = eglCreateWindowSurface(display, config, windowAndroid->getNativeWindow(), nullptr);
            if (surface == EGL_NO_SURFACE)
                throw SystemError("Failed to create EGL window surface");

            for (EGLint version = 3; version >= 2; --version)
            {
                std::vector<EGLint> contextAttributes =
                {
                    EGL_CONTEXT_CLIENT_VERSION, version
                };

                if (debugRenderer)
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
                throw SystemError("Failed to create EGL context");

            if (!eglMakeCurrent(display, surface, surface, context))
                throw SystemError("Failed to set current EGL context");

            if (!eglSwapInterval(display, verticalSync ? 1 : 0))
                throw SystemError("Failed to set EGL frame interval");

            EGLint surfaceWidth;
            EGLint surfaceHeight;

            if (!eglQuerySurface(display, surface, EGL_WIDTH, &surfaceWidth) ||
                !eglQuerySurface(display, surface, EGL_HEIGHT, &surfaceHeight))
                throw SystemError("Failed to get query window size " + std::to_string(eglGetError()));

            frameBufferWidth = surfaceWidth;
            frameBufferHeight = surfaceHeight;

            stateCache = StateCache();

            glDisable(GL_DITHER);
            glDepthFunc(GL_LEQUAL);

            GLenum error;

            if ((error = glGetError()) != GL_NO_ERROR)
                throw SystemError("Failed to set depth function, error: " + std::to_string(error));

            if (glGenVertexArraysProc) glGenVertexArraysProc(1, &vertexArrayId);

            {
                std::unique_lock<std::mutex> lock(resourceMutex);

                for (const std::unique_ptr<RenderResource>& resource : resources)
                    static_cast<RenderResourceOGL*>(resource.get())->reload();
            }

            if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
                throw SystemError("Failed to unset EGL context");

            running = true;
            renderThread = std::thread(&RenderDeviceOGLAndroid::main, this);
        }

        void RenderDeviceOGLAndroid::destroy()
        {
            running = false;
            flushCommands();
            if (renderThread.joinable()) renderThread.join();

            if (context)
            {
                if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
                    Log(Log::Level::ERR) << "Failed to unset EGL context";

                if (!eglDestroyContext(display, context))
                    Log(Log::Level::ERR) << "Failed to destroy EGL context";

                context = nullptr;
            }

            if (surface)
            {
                if (!eglDestroySurface(display, surface))
                    Log(Log::Level::ERR) << "Failed to destroy EGL surface";

                surface = nullptr;
            }
        }

        void RenderDeviceOGLAndroid::present()
        {
            if (eglSwapBuffers(display, surface) != EGL_TRUE)
                throw SystemError("Failed to swap buffers, error: " + std::to_string(eglGetError()));
        }

        void RenderDeviceOGLAndroid::main()
        {
            setCurrentThreadName("Render");

            if (!eglMakeCurrent(display, surface, surface, context))
                throw SystemError("Failed to set current EGL context, error: " + std::to_string(eglGetError()));

            while (running)
            {
                try
                {
                    process();
                }
                catch (const std::exception& e)
                {
                    Log(Log::Level::ERR) << e.what();
                }
                catch (...)
                {
                    Log(Log::Level::ERR) << "Unknown error occurred";
                }
            }

            if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
                Log(Log::Level::ERR) << "Failed to unset EGL context, error: " << eglGetError();
        }
    } // namespace graphics
} // namespace ouzel

#endif
