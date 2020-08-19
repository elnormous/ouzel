// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "../../../core/Setup.h"

#if defined(__linux__) && !defined(__ANDROID__) && OUZEL_COMPILE_OPENGL

#if OUZEL_SUPPORTS_X11
#  include <X11/Xlib.h>
#  include <X11/extensions/xf86vmode.h>
#endif
#include "OGLRenderDeviceLinux.hpp"
#include "../EGLErrorCategory.hpp"
#include "../../../core/linux/EngineLinux.hpp"
#include "../../../core/linux/NativeWindowLinux.hpp"
#include "../../../utils/Log.hpp"
#include "../../../utils/Utils.hpp"

namespace ouzel::graphics::opengl::linux
{
    namespace
    {
        const egl::ErrorCategory eglErrorCategory{};
    }

    RenderDevice::RenderDevice(const Settings& settings,
                               core::Window& initWindow,
                               const std::function<void(const Event&)>& initCallback):
        opengl::RenderDevice(settings, initWindow, initCallback)
    {
#if OUZEL_OPENGLES
        embedded = true;
#else
        embedded = false;
#endif

        auto windowLinux = static_cast<core::linux::NativeWindow*>(window.getNativeWindow());

#if OUZEL_OPENGLES
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
#else
        const auto nativeDisplay = bitCast<EGLNativeDisplayType>(windowLinux->getDisplay());
        display = eglGetDisplay(nativeDisplay);
#endif

        if (display == EGL_NO_DISPLAY)
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to get display");

        if (!eglInitialize(display, nullptr, nullptr))
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to initialize EGL");

        const EGLint attributeList[] = {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, settings.depth ? 24 : 0,
            EGL_STENCIL_SIZE, settings.stencil ? 8 : 0,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
#if OUZEL_OPENGLES
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
#else
            EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
#endif
            EGL_SAMPLE_BUFFERS, (settings.sampleCount > 1) ? 1 : 0,
            EGL_SAMPLES, static_cast<int>(settings.sampleCount),
            EGL_NONE
        };
        EGLConfig config;
        EGLint numConfig;
        if (!eglChooseConfig(display, attributeList, &config, 1, &numConfig))
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to choose EGL config");

#if OUZEL_OPENGLES
        if (!eglBindAPI(EGL_OPENGL_ES_API))
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to bind OpenGL ES API");
#else
        if (!eglBindAPI(EGL_OPENGL_API))
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to bind OpenGL API");
#endif

#if OUZEL_OPENGLES
        const auto dispmanxWindow = &windowLinux->getNativeWindow();
        const auto nativeWindow = bitCast<EGLNativeWindowType>(dispmanxWindow);
#else
        const auto nativeWindow = bitCast<EGLNativeWindowType>(windowLinux->getNativeWindow());
#endif

        surface = eglCreateWindowSurface(display, config, nativeWindow, nullptr);
        if (surface == EGL_NO_SURFACE)
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to create EGL window surface");

#if OUZEL_OPENGLES
        for (EGLint version = 3; version >= 2; --version)
#else
        for (EGLint version = 4; version >= 3; --version)
#endif
        {
            EGLint contextAttributes[] = {
                EGL_CONTEXT_CLIENT_VERSION, version,
                EGL_CONTEXT_OPENGL_DEBUG, settings.debugRenderer ? EGL_TRUE : EGL_FALSE,
                EGL_NONE
            };

            context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttributes);

            if (context != EGL_NO_CONTEXT)
            {
                apiVersion = ApiVersion(version, 0);
#if OUZEL_OPENGLES
                logger.log(Log::Level::info) << "EGL OpenGL ES " << version << " context created";
#else
                logger.log(Log::Level::info) << "EGL OpenGL " << version << " context created";
#endif
                break;
            }
        }

        if (context == EGL_NO_CONTEXT)
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to create EGL context");

        if (!eglMakeCurrent(display, surface, surface, context))
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to set current EGL context");

        if (!eglSwapInterval(display, settings.verticalSync ? 1 : 0))
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to set EGL frame interval");

        init(static_cast<GLsizei>(window.getResolution().v[0]),
             static_cast<GLsizei>(window.getResolution().v[1]));

        if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to unset EGL context");

        running = true;
        renderThread = Thread(&RenderDevice::renderMain, this);
    }

    RenderDevice::~RenderDevice()
    {
        running = false;
        CommandBuffer commandBuffer;
        commandBuffer.pushCommand(std::make_unique<PresentCommand>());
        submitCommandBuffer(std::move(commandBuffer));

        if (renderThread.isJoinable()) renderThread.join();

        if (context != EGL_NO_CONTEXT)
        {
            eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            eglDestroyContext(display, context);
        }

        if (surface != EGL_NO_SURFACE)
            eglDestroySurface(display, surface);

        if (display != EGL_NO_DISPLAY)
            eglTerminate(display);
    }

    std::vector<Size2U> RenderDevice::getSupportedResolutions() const
    {
        std::vector<Size2U> result;

#if OUZEL_OPENGL_INTERFACE_GLX
        auto engineLinux = static_cast<core::linux::Engine*>(engine);

        int modeCount;
        XF86VidModeModeInfo** modeInfo;

        XF86VidModeGetAllModeLines(engineLinux->getDisplay(), 0, &modeCount, &modeInfo);

        for (int i = 0; i < modeCount; ++i)
            result.emplace_back(static_cast<std::uint32_t>(modeInfo[i]->hdisplay),
                                static_cast<std::uint32_t>(modeInfo[i]->vdisplay));

        XFree(modeInfo);
#elif OUZEL_OPENGL_INTERFACE_EGL
        // TODO: return screen resolution
#endif

        return result;
    }

    void RenderDevice::present()
    {
        if (eglSwapBuffers(display, surface) != EGL_TRUE)
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to swap buffers");
    }

    void RenderDevice::renderMain()
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
                logger.log(Log::Level::error) << e.what();
            }
        }

        if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to unset EGL context");
    }
}

#endif
