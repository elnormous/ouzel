// Ouzel by Elviss Strazdins

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

        std::vector<EGLConfig> chooseConfig(EGLDisplay display,
                                            bool depth,
                                            bool stencil,
                                            std::uint32_t sampleCount,
                                            EGLint version)
        {
            const EGLint attributeList[] = {
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_ALPHA_SIZE, 8,
                EGL_DEPTH_SIZE, depth ? 24 : 0,
                EGL_STENCIL_SIZE, stencil ? 8 : 0,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
#if OUZEL_OPENGLES
                EGL_RENDERABLE_TYPE, (version >= 3) ? EGL_OPENGL_ES3_BIT :
                                     (version == 2) ? EGL_OPENGL_ES2_BIT :
                                     EGL_OPENGL_ES_BIT,
#else
                EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
#endif
                EGL_SAMPLE_BUFFERS, (sampleCount > 1) ? 1 : 0,
                EGL_SAMPLES, static_cast<EGLint>(sampleCount),
                EGL_NONE
            };
            
            EGLint configCount;
            if (!eglChooseConfig(display, attributeList, nullptr, 0, &configCount))
                throw std::system_error(eglGetError(), eglErrorCategory, "Failed to choose EGL config");

            if (configCount == 0) return {};

            std::vector<EGLConfig> configs(configCount);
            if (!eglChooseConfig(display, attributeList, configs.data(), configCount, &configCount))
                throw std::system_error(eglGetError(), eglErrorCategory, "Failed to choose EGL config");

            return configs;
        }
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
        const auto nativeDisplay = EGL_DEFAULT_DISPLAY;
#else
        const auto nativeDisplay = bitCast<EGLNativeDisplayType>(windowLinux->getDisplay());
#endif

        display = eglGetDisplay(nativeDisplay);
        if (display == EGL_NO_DISPLAY)
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to get display");

        if (!eglInitialize(display, nullptr, nullptr))
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to initialize EGL");

        const auto eglVersionPtr = eglQueryString(display, EGL_VERSION);
        if (!eglVersionPtr)
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to get EGL version");
        logger.log(Log::Level::all) << "EGL version: " << eglVersionPtr;

        const auto eglExtensionsPtr = eglQueryString(display, EGL_EXTENSIONS);
        if (!eglExtensionsPtr)
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to get EGL extensions");
        const auto eglExtensions = explodeString(eglExtensionsPtr, ' ');
        logger.log(Log::Level::all) << "Supported EGL extensions: " << eglExtensions;

#if OUZEL_OPENGLES
        const auto nativeWindow = bitCast<EGLNativeWindowType>(&windowLinux->getNativeWindow());
#else
        const auto nativeWindow = bitCast<EGLNativeWindowType>(windowLinux->getNativeWindow());
#endif

#if OUZEL_OPENGLES
        for (EGLint version = 3; version >= 2; --version)
#else
        for (EGLint version = 4; version >= 3; --version)
#endif
        {
            const auto configs = chooseConfig(display, settings.depth, settings.stencil, settings.sampleCount, version);
            if (configs.empty()) continue;

#if OUZEL_OPENGLES
            if (!eglBindAPI(EGL_OPENGL_ES_API))
                throw std::system_error(eglGetError(), eglErrorCategory, "Failed to bind OpenGL ES API");
#else
            if (!eglBindAPI(EGL_OPENGL_API))
                throw std::system_error(eglGetError(), eglErrorCategory, "Failed to bind OpenGL API");
#endif

            surface = eglCreateWindowSurface(display, configs[0], nativeWindow, nullptr);
            if (surface == EGL_NO_SURFACE)
                throw std::system_error(eglGetError(), eglErrorCategory, "Failed to create EGL window surface");

            const EGLint contextAttributes[] = {
                EGL_CONTEXT_CLIENT_VERSION, version,
                // EGL_CONTEXT_FLAGS_KHR, settings.debugRenderer ? EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR : 0,
                EGL_NONE
            };

            context = eglCreateContext(display, configs[0], EGL_NO_CONTEXT, contextAttributes);

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
            else // TODO: use RAII for surface
                eglDestroySurface(display, surface);
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
        renderThread = thread::Thread(&RenderDevice::renderMain, this);
    }

    RenderDevice::~RenderDevice()
    {
        running = false;
        CommandBuffer commandBuffer;
        commandBuffer.pushCommand(std::make_unique<PresentCommand>());
        submitCommandBuffer(std::move(commandBuffer));

        if (renderThread.isJoinable()) renderThread.join();

        if (display != EGL_NO_DISPLAY)
        {
            if (context != EGL_NO_CONTEXT)
            {
                eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
                eglDestroyContext(display, context);
            }

            if (surface != EGL_NO_SURFACE)
                eglDestroySurface(display, surface);

            eglTerminate(display);
        }
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
        thread::setCurrentThreadName("Render");

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
