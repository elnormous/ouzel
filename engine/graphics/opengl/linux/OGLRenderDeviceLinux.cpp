// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include "../../../core/Setup.h"

#if defined(__linux__) && !defined(__ANDROID__) && OUZEL_COMPILE_OPENGL

#if OUZEL_SUPPORTS_X11
#  include <X11/Xlib.h>
#  include <X11/extensions/xf86vmode.h>
#endif
#include "OGLRenderDeviceLinux.hpp"
#include "../../../core/linux/EngineLinux.hpp"
#include "../../../core/linux/NativeWindowLinux.hpp"
#include "../../../utils/Log.hpp"
#include "../../../utils/Utils.hpp"

namespace ouzel::graphics::opengl::linux
{
#if OUZEL_OPENGL_INTERFACE_EGL
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
#endif

    RenderDevice::RenderDevice(const std::function<void(const Event&)>& initCallback):
        opengl::RenderDevice(initCallback)
    {
#if OUZEL_OPENGLES
        embedded = true;
#else
        embedded = false;
#endif
    }

    RenderDevice::~RenderDevice()
    {
        running = false;
        CommandBuffer commandBuffer;
        commandBuffer.pushCommand(std::make_unique<PresentCommand>());
        submitCommandBuffer(std::move(commandBuffer));

        if (renderThread.isJoinable()) renderThread.join();

#if OUZEL_OPENGL_INTERFACE_GLX
        auto engineLinux = static_cast<core::linux::Engine*>(engine);

        if (engineLinux->getDisplay() && context)
        {
            glXMakeCurrent(engineLinux->getDisplay(), None, nullptr);
            glXDestroyContext(engineLinux->getDisplay(), context);
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

    void RenderDevice::init(core::Window* newWindow,
                            const Size2U& newSize,
                            const Settings& settings)
    {
        auto windowLinux = static_cast<core::linux::NativeWindow*>(newWindow->getNativeWindow());

#if OUZEL_OPENGL_INTERFACE_GLX
        auto engineLinux = static_cast<core::linux::Engine*>(engine);

        // make sure OpenGL's GLX extension supported
        int errorBase;
        int eventBase;
        if (!glXQueryExtension(engineLinux->getDisplay(), &errorBase, &eventBase))
            throw std::runtime_error("X server has no OpenGL GLX extension");

        int glxMajor;
        int glxMinor;
        if (!glXQueryVersion(engineLinux->getDisplay(), &glxMajor, &glxMinor))
            throw std::runtime_error("Failed to get GLX version");

        logger.log(Log::Level::all) << "GLX version: " << glxMajor << "." << glxMinor;

        Screen* screen = XDefaultScreenOfDisplay(engineLinux->getDisplay());
        const int screenIndex = XScreenNumberOfScreen(screen);

        GLXContext tempContext = glXCreateContext(engineLinux->getDisplay(), windowLinux->getVisualInfo(), None, GL_TRUE);
        if (!tempContext)
            throw std::runtime_error("Failed to create GLX context");
        if (!glXMakeCurrent(engineLinux->getDisplay(), windowLinux->getNativeWindow(), tempContext))
            throw std::runtime_error("Failed to make GLX context current");

        std::vector<std::string> extensions;

        if (const char* extensionsPtr = glXQueryExtensionsString(engineLinux->getDisplay(), screenIndex))
            extensions = explodeString(std::string(extensionsPtr), ' ');

        logger.log(Log::Level::all) << "Supported GLX extensions: " << extensions;

        glXMakeCurrent(engineLinux->getDisplay(), None, nullptr);
        glXDestroyContext(engineLinux->getDisplay(), tempContext);

        PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsProc = nullptr;
        PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXTProc = nullptr;

        for (const auto& extension : extensions)
        {
            if (extension == "GLX_ARB_create_context")
                glXCreateContextAttribsProc = reinterpret_cast<PFNGLXCREATECONTEXTATTRIBSARBPROC>(glXGetProcAddress(reinterpret_cast<const GLubyte*>("glXCreateContextAttribsARB")));
            else if (extension == "GLX_EXT_swap_control")
                glXSwapIntervalEXTProc = reinterpret_cast<PFNGLXSWAPINTERVALEXTPROC>(glXGetProcAddress(reinterpret_cast<const GLubyte*>("glXSwapIntervalEXT")));
        }

        int fbcount = 0;

        const int attributes[] = {
            GLX_X_RENDERABLE, GL_TRUE,
            GLX_RENDER_TYPE, GLX_RGBA_BIT,
            GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
            GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
            GLX_DOUBLEBUFFER, GL_TRUE,
            GLX_RED_SIZE, 8,
            GLX_GREEN_SIZE, 8,
            GLX_BLUE_SIZE, 8,
            GLX_ALPHA_SIZE, 8,
            GLX_DEPTH_SIZE, settings.depth ? 24 : 0,
            GLX_STENCIL_SIZE, settings.stencil ? 8 : 0,
            GLX_SAMPLE_BUFFERS, (settings.sampleCount > 1) ? 1 : 0,
            GLX_SAMPLES, static_cast<int>(settings.sampleCount),
            GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB, settings.srgb ? 1 : 0,
            0
        };

        std::unique_ptr<GLXFBConfig, int(*)(void*)> frameBufferConfig(glXChooseFBConfig(engineLinux->getDisplay(), screenIndex, attributes, &fbcount), XFree);
        if (frameBufferConfig)
        {
            if (glXCreateContextAttribsProc)
            {
                // create an OpenGL rendering context
                std::vector<int> contextAttribs = {
                    GLX_CONTEXT_PROFILE_MASK_ARB,
                    GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
                    GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
                    GLX_CONTEXT_MINOR_VERSION_ARB, 2
                };

                if (settings.debugRenderer)
                {
                    contextAttribs.push_back(GL_CONTEXT_FLAGS);
                    contextAttribs.push_back(GL_CONTEXT_FLAG_DEBUG_BIT);
                }

                contextAttribs.push_back(0);

                context = glXCreateContextAttribsProc(engineLinux->getDisplay(), *frameBufferConfig, nullptr, True, contextAttribs.data());

                if (context)
                {
                    apiVersion = ApiVersion(3, 0);
                    logger.log(Log::Level::info) << "GLX OpenGL 3.2 context created";
                }
            }
        }

        if (!context)
        {
            context = glXCreateContext(engineLinux->getDisplay(), windowLinux->getVisualInfo(), None, GL_TRUE);

            if (context)
            {
                apiVersion = ApiVersion(2, 0);
                logger.log(Log::Level::info) << "GLX OpenGL 2 context created";
            }
            else
                throw std::runtime_error("Failed to create GLX context");
        }

        // bind the rendering context to the window
        if (!glXMakeCurrent(engineLinux->getDisplay(), windowLinux->getNativeWindow(), context))
            throw std::runtime_error("Failed to make GLX context current");

        if (glXSwapIntervalEXTProc)
            glXSwapIntervalEXTProc(engineLinux->getDisplay(), windowLinux->getNativeWindow(), settings.verticalSync ? 1 : 0);
#elif OUZEL_OPENGL_INTERFACE_EGL
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

        if (!display)
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to get display");

        if (!eglInitialize(display, nullptr, nullptr))
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to initialize EGL");

        const EGLint attributeList[] =
        {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, settings.depth ? 24 : 0,
            EGL_STENCIL_SIZE, settings.stencil ? 8 : 0,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_SAMPLE_BUFFERS, (settings.sampleCount > 1) ? 1 : 0,
            EGL_SAMPLES, static_cast<int>(settings.sampleCount),
            EGL_NONE
        };
        EGLConfig config;
        EGLint numConfig;
        if (!eglChooseConfig(display, attributeList, &config, 1, &numConfig))
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to choose EGL config");

        if (!eglBindAPI(EGL_OPENGL_ES_API))
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to bind OpenGL ES API");

        const auto dispmanxWindow = &windowLinux->getNativeWindow();
        const auto nativeWindow = bitCast<EGLNativeWindowType>(dispmanxWindow);
        surface = eglCreateWindowSurface(display, config, nativeWindow, nullptr);
        if (surface == EGL_NO_SURFACE)
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to create EGL window surface");

        for (EGLint version = 3; version >= 2; --version)
        {
            std::vector<EGLint> contextAttributes =
            {
                EGL_CONTEXT_CLIENT_VERSION, version
            };

            if (settings.debugRenderer)
            {
                contextAttributes.push_back(EGL_CONTEXT_FLAGS_KHR);
                contextAttributes.push_back(EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR);
            }

            contextAttributes.push_back(EGL_NONE);

            context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttributes.data());

            if (context != EGL_NO_CONTEXT)
            {
                apiVersion = ApiVersion(version, 0);
                logger.log(Log::Level::info) << "EGL OpenGL ES " << version << " context created";
                break;
            }
        }

        if (context == EGL_NO_CONTEXT)
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to create EGL context");

        if (!eglMakeCurrent(display, surface, surface, context))
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to set current EGL context");

        if (!eglSwapInterval(display, settings.verticalSync ? 1 : 0))
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to set EGL frame interval");
#endif

        opengl::RenderDevice::init(newWindow,
                                   newSize,
                                   settings);

#if OUZEL_OPENGL_INTERFACE_EGL
        if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to unset EGL context");
#endif

        running = true;
        renderThread = Thread(&RenderDevice::renderMain, this);
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
#if OUZEL_OPENGL_INTERFACE_GLX
        auto engineLinux = static_cast<core::linux::Engine*>(engine);
        auto windowLinux = static_cast<core::linux::NativeWindow*>(window->getNativeWindow());

        glXSwapBuffers(engineLinux->getDisplay(), windowLinux->getNativeWindow());
#elif OUZEL_OPENGL_INTERFACE_EGL
        if (eglSwapBuffers(display, surface) != EGL_TRUE)
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to swap buffers");
#endif
    }

    void RenderDevice::renderMain()
    {
        Thread::setCurrentThreadName("Render");

#if OUZEL_OPENGL_INTERFACE_GLX
        auto engineLinux = static_cast<core::linux::Engine*>(engine);
        auto windowLinux = static_cast<core::linux::NativeWindow*>(window->getNativeWindow());

        if (!glXMakeCurrent(engineLinux->getDisplay(), windowLinux->getNativeWindow(), context))
            throw std::runtime_error("Failed to make GLX context current");
#elif OUZEL_OPENGL_INTERFACE_EGL
        if (!eglMakeCurrent(display, surface, surface, context))
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to set current EGL context");
#endif

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

#if OUZEL_OPENGL_INTERFACE_EGL
        if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
            throw std::system_error(eglGetError(), eglErrorCategory, "Failed to unset EGL context");
#endif
    }
}

#endif
