// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "../../../core/Setup.h"

#if defined(__ANDROID__) && OUZEL_COMPILE_OPENGL

#include "OGLRenderDeviceAndroid.hpp"
#include "../../../core/Engine.hpp"
#include "../../../core/Window.hpp"
#include "../../../core/android/NativeWindowAndroid.hpp"
#include "../../../utils/Log.hpp"

namespace ouzel::graphics::opengl
{
    namespace
    {
        class EGLErrorCategory final: public std::error_category
        {
        public:
            const char* name() const noexcept final
            {
                return "EGL";
            }

            std::string message(int condition) const final
            {
                switch (condition)
                {
                    case EGL_NOT_INITIALIZED: return "EGL_NOT_INITIALIZED";
                    case EGL_BAD_ACCESS: return "EGL_BAD_ACCESS";
                    case EGL_BAD_ALLOC: return "EGL_BAD_ALLOC";
                    case EGL_BAD_ATTRIBUTE: return "EGL_BAD_ATTRIBUTE";
                    case EGL_BAD_CONTEXT: return "EGL_BAD_CONTEXT";
                    case EGL_BAD_CONFIG: return "EGL_BAD_CONFIG";
                    case EGL_BAD_CURRENT_SURFACE: return "EGL_BAD_CURRENT_SURFACE";
                    case EGL_BAD_DISPLAY: return "EGL_BAD_DISPLAY";
                    case EGL_BAD_SURFACE: return "EGL_BAD_SURFACE";
                    case EGL_BAD_MATCH: return "EGL_BAD_MATCH";
                    case EGL_BAD_PARAMETER: return "EGL_BAD_PARAMETER";
                    case EGL_BAD_NATIVE_PIXMAP: return "EGL_BAD_NATIVE_PIXMAP";
                    case EGL_BAD_NATIVE_WINDOW: return "EGL_BAD_NATIVE_WINDOW";
                    case EGL_CONTEXT_LOST: return "EGL_CONTEXT_LOST";
                    default: return "Unknown error (" + std::to_string(condition) + ")";
                }
            }
        };

        const EGLErrorCategory eglErrorCategory {};
    }

    RenderDeviceAndroid::RenderDeviceAndroid(const std::function<void(const Event&)>& initCallback):
        RenderDevice(initCallback)
    {
    }

    RenderDeviceAndroid::~RenderDeviceAndroid()
    {
        running = false;
        CommandBuffer commandBuffer;
        commandBuffer.pushCommand(std::make_unique<PresentCommand>());
        submitCommandBuffer(std::move(commandBuffer));

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

    void RenderDeviceAndroid::init(Window* newWindow,
                                    const Size2U&,
                                    std::uint32_t newSampleCount,
                                    bool newSrgb,
                                    bool newVerticalSync,
                                    bool newDepth,
                                    bool newStencil,
                                    bool newDebugRenderer)
    {
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

        if (!display)
            throw std::runtime_error("Failed to get display");

        if (!eglInitialize(display, nullptr, nullptr))
            throw std::runtime_error("Failed to initialize EGL");

        const EGLint attributeList[] =
        {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, newDepth ? 24 : 0,
            EGL_STENCIL_SIZE, newStencil ? 8 : 0,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_SAMPLE_BUFFERS, (newSampleCount > 1) ? 1 : 0,
            EGL_SAMPLES, static_cast<int>(newSampleCount),
            EGL_NONE
        };
        EGLConfig config;
        EGLint numConfig;
        if (!eglChooseConfig(display, attributeList, &config, 1, &numConfig))
            throw std::runtime_error("Failed to choose EGL config");

        if (!eglBindAPI(EGL_OPENGL_ES_API))
            throw std::runtime_error("Failed to bind OpenGL ES API");

        EGLint format;
        if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format))
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to get config attribute");

        auto windowAndroid = static_cast<NativeWindowAndroid*>(newWindow->getNativeWindow());

        ANativeWindow_setBuffersGeometry(windowAndroid->getNativeWindow(), 0, 0, format);

        surface = eglCreateWindowSurface(display, config, windowAndroid->getNativeWindow(), nullptr);
        if (surface == EGL_NO_SURFACE)
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to create EGL window surface");

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
                apiVersion = ApiVersion(version, 0);
                engine->log(Log::Level::info) << "EGL OpenGL ES " << version << " context created";
                break;
            }
        }

        if (context == EGL_NO_CONTEXT)
            throw std::runtime_error("Failed to create EGL context");

        if (!eglMakeCurrent(display, surface, surface, context))
            throw std::runtime_error("Failed to set current EGL context");

        if (!eglSwapInterval(display, newVerticalSync ? 1 : 0))
            throw std::runtime_error("Failed to set EGL frame interval");

        EGLint surfaceWidth;
        EGLint surfaceHeight;

        if (!eglQuerySurface(display, surface, EGL_WIDTH, &surfaceWidth) ||
            !eglQuerySurface(display, surface, EGL_HEIGHT, &surfaceHeight))
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to get query window size");

        frameBufferWidth = surfaceWidth;
        frameBufferHeight = surfaceHeight;

        auto backBufferSize = Size2U(static_cast<std::uint32_t>(frameBufferWidth),
                                        static_cast<std::uint32_t>(frameBufferHeight));

        RenderDevice::init(newWindow,
                            backBufferSize,
                            newSampleCount,
                            newSrgb,
                            newVerticalSync,
                            newDepth,
                            newStencil,
                            newDebugRenderer);

        if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
            throw std::runtime_error("Failed to unset EGL context");

        running = true;
        renderThread = Thread(&RenderDeviceAndroid::renderMain, this);
    }

    void RenderDeviceAndroid::reload()
    {
        running = false;
        CommandBuffer commandBuffer;
        commandBuffer.pushCommand(std::make_unique<PresentCommand>());
        submitCommandBuffer(std::move(commandBuffer));

        if (renderThread.isJoinable()) renderThread.join();

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
            throw std::runtime_error("Failed to choose EGL config");

        if (!eglBindAPI(EGL_OPENGL_ES_API))
            throw std::runtime_error("Failed to bind OpenGL ES API");

        EGLint format;
        if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format))
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to get config attribute");

        auto windowAndroid = static_cast<NativeWindowAndroid*>(window->getNativeWindow());

        ANativeWindow_setBuffersGeometry(windowAndroid->getNativeWindow(), 0, 0, format);

        surface = eglCreateWindowSurface(display, config, windowAndroid->getNativeWindow(), nullptr);
        if (surface == EGL_NO_SURFACE)
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to create EGL window surface");

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
                apiVersion = ApiVersion(version, 0);
                engine->log(Log::Level::info) << "EGL OpenGL ES " << version << " context created";
                break;
            }
        }

        if (context == EGL_NO_CONTEXT)
            throw std::runtime_error("Failed to create EGL context");

        if (!eglMakeCurrent(display, surface, surface, context))
            throw std::runtime_error("Failed to set current EGL context");

        if (!eglSwapInterval(display, verticalSync ? 1 : 0))
            throw std::runtime_error("Failed to set EGL frame interval");

        EGLint surfaceWidth;
        EGLint surfaceHeight;

        if (!eglQuerySurface(display, surface, EGL_WIDTH, &surfaceWidth) ||
            !eglQuerySurface(display, surface, EGL_HEIGHT, &surfaceHeight))
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to get query window size");

        frameBufferWidth = surfaceWidth;
        frameBufferHeight = surfaceHeight;

        stateCache = StateCache();

        glDisableProc(GL_DITHER);
        glDepthFuncProc(GL_LEQUAL);

        GLenum error;

        if ((error = glGetErrorProc()) != GL_NO_ERROR)
            throw std::system_error(makeErrorCode(error), "Failed to set depth function");

        if (glGenVertexArraysProc) glGenVertexArraysProc(1, &vertexArrayId);

        for (const auto& resource : resources)
            if (resource) resource->invalidate();

        for (const auto& resource : resources)
            if (resource) resource->restore();

        if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
            throw std::runtime_error("Failed to unset EGL context");

        running = true;
        renderThread = Thread(&RenderDeviceAndroid::renderMain, this);
    }

    void RenderDeviceAndroid::destroy()
    {
        running = false;
        CommandBuffer commandBuffer;
        commandBuffer.pushCommand(std::make_unique<PresentCommand>());
        submitCommandBuffer(std::move(commandBuffer));

        if (renderThread.isJoinable()) renderThread.join();

        if (context)
        {
            if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
                engine->log(Log::Level::error) << "Failed to unset EGL context";

            if (!eglDestroyContext(display, context))
                engine->log(Log::Level::error) << "Failed to destroy EGL context";

            context = nullptr;
        }

        if (surface)
        {
            if (!eglDestroySurface(display, surface))
                engine->log(Log::Level::error) << "Failed to destroy EGL surface";

            surface = nullptr;
        }
    }

    void RenderDeviceAndroid::present()
    {
        if (eglSwapBuffers(display, surface) != EGL_TRUE)
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to swap buffers");
    }

    void RenderDeviceAndroid::renderMain()
    {
        Thread::setCurrentThreadName("Render");

        if (!eglMakeCurrent(display, surface, surface, context))
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to set current EGL context");

        while (running)
        {
            try
            {
                process();
            }
            catch (const std::exception& e)
            {
                engine->log(Log::Level::error) << e.what();
            }
        }

        if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to unset EGL context");
    }
}

#endif
