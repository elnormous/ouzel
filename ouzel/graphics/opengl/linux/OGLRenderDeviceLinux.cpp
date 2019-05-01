// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if defined(__linux__) && !defined(__ANDROID__) && OUZEL_COMPILE_OPENGL

#if OUZEL_SUPPORTS_X11
#  include <X11/Xlib.h>
#  include <X11/extensions/xf86vmode.h>
#endif
#include "OGLRenderDeviceLinux.hpp"
#include "core/linux/EngineLinux.hpp"
#include "core/linux/NativeWindowLinux.hpp"
#include "utils/Log.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    namespace graphics
    {
#if OUZEL_OPENGL_INTERFACE_EGL
        class EGLErrorCategory final: public std::error_category
        {
        public:
            const char* name() const noexcept override
            {
                return "EGL";
            }

            std::string message(int condition) const override
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
#endif

        OGLRenderDeviceLinux::OGLRenderDeviceLinux(const std::function<void(const Event&)>& initCallback):
            OGLRenderDevice(initCallback)
        {
        }

        OGLRenderDeviceLinux::~OGLRenderDeviceLinux()
        {
            running = false;
            CommandBuffer commandBuffer;
            commandBuffer.pushCommand(std::unique_ptr<Command>(new PresentCommand()));
            submitCommandBuffer(std::move(commandBuffer));

            if (renderThread.joinable()) renderThread.join();

#if OUZEL_OPENGL_INTERFACE_GLX
            EngineLinux* engineLinux = static_cast<EngineLinux*>(engine);

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

        void OGLRenderDeviceLinux::init(Window* newWindow,
                                        const Size2U& newSize,
                                        uint32_t newSampleCount,
                                        Texture::Filter newTextureFilter,
                                        uint32_t newMaxAnisotropy,
                                        bool newSrgb,
                                        bool newVerticalSync,
                                        bool newDepth,
                                        bool newStencil,
                                        bool newDebugRenderer)
        {
            NativeWindowLinux* windowLinux = static_cast<NativeWindowLinux*>(newWindow->getNativeWindow());

#if OUZEL_OPENGL_INTERFACE_GLX
            EngineLinux* engineLinux = static_cast<EngineLinux*>(engine);

            // make sure OpenGL's GLX extension supported
            int errorBase;
            int eventBase;
            if (!glXQueryExtension(engineLinux->getDisplay(), &errorBase, &eventBase))
                throw std::runtime_error("X server has no OpenGL GLX extension");

            int glxMajor;
            int glxMinor;
            if (!glXQueryVersion(engineLinux->getDisplay(), &glxMajor, &glxMinor))
                throw std::runtime_error("Failed to get GLX version");

            engine->log(Log::Level::ALL) << "GLX version: " << glxMajor << "." << glxMinor;

            Screen* screen = XDefaultScreenOfDisplay(engineLinux->getDisplay());
            int screenIndex = XScreenNumberOfScreen(screen);

            GLXContext tempContext = glXCreateContext(engineLinux->getDisplay(), windowLinux->getVisualInfo(), None, GL_TRUE);
            if (!tempContext)
                throw std::runtime_error("Failed to create GLX context");
            if (!glXMakeCurrent(engineLinux->getDisplay(), windowLinux->getNativeWindow(), tempContext))
                throw std::runtime_error("Failed to make GLX context current");

            std::vector<std::string> extensions;

            if (const char* extensionsPtr = glXQueryExtensionsString(engineLinux->getDisplay(), screenIndex))
                extensions = explodeString(reinterpret_cast<const char*>(extensionsPtr), ' ');

            engine->log(Log::Level::ALL) << "Supported GLX extensions: " << extensions;

            glXMakeCurrent(engineLinux->getDisplay(), None, nullptr);
            glXDestroyContext(engineLinux->getDisplay(), tempContext);

            PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsProc = nullptr;
            PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXTProc = nullptr;

            for (const std::string& extension : extensions)
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
                GLX_DEPTH_SIZE, newDepth ? 24 : 0,
                GLX_STENCIL_SIZE, newStencil ? 8 : 0,
                GLX_SAMPLE_BUFFERS, (newSampleCount > 1) ? 1 : 0,
                GLX_SAMPLES, static_cast<int>(newSampleCount),
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

                    if (newDebugRenderer)
                    {
                        contextAttribs.push_back(GL_CONTEXT_FLAGS);
                        contextAttribs.push_back(GL_CONTEXT_FLAG_DEBUG_BIT);
                    }

                    contextAttribs.push_back(0);

                    context = glXCreateContextAttribsProc(engineLinux->getDisplay(), *frameBufferConfig, nullptr, True, contextAttribs.data());

                    if (context)
                    {
                        apiMajorVersion = 3;
                        apiMinorVersion = 2;
                        engine->log(Log::Level::INFO) << "GLX OpenGL 3.2 context created";
                    }
                }
            }

            if (!context)
            {
                context = glXCreateContext(engineLinux->getDisplay(), windowLinux->getVisualInfo(), None, GL_TRUE);

                if (context)
                {
                    apiMajorVersion = 2;
                    apiMinorVersion = 0;
                    engine->log(Log::Level::INFO) << "GLX OpenGL 2 context created";
                }
                else
                    throw std::runtime_error("Failed to create GLX context");
            }

            // bind the rendering context to the window
            if (!glXMakeCurrent(engineLinux->getDisplay(), windowLinux->getNativeWindow(), context))
                throw std::runtime_error("Failed to make GLX context current");

            if (glXSwapIntervalEXTProc)
                glXSwapIntervalEXTProc(engineLinux->getDisplay(), windowLinux->getNativeWindow(), newVerticalSync ? 1 : 0);
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
                EGL_DEPTH_SIZE, newDepth ? 24 : 0,
                EGL_STENCIL_SIZE, newStencil ? 8 : 0,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_SAMPLE_BUFFERS, (newSampleCount > 1) ? 1 : 0,
                EGL_SAMPLES, static_cast<int>(newSampleCount),
                EGL_NONE
            };
            EGLConfig config;
            EGLint numConfig;
            if (!eglChooseConfig(display, attributeList, &config, 1, &numConfig))
                throw std::system_error(eglGetError(), eglErrorCategory, "Failed to choose EGL config");

            if (!eglBindAPI(EGL_OPENGL_ES_API))
                throw std::system_error(eglGetError(), eglErrorCategory, "Failed to bind OpenGL ES API");

            surface = eglCreateWindowSurface(display, config, reinterpret_cast<EGLNativeWindowType>(&windowLinux->getNativeWindow()), nullptr);
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
                    apiMajorVersion = version;
                    apiMinorVersion = 0;
                    engine->log(Log::Level::INFO) << "EGL OpenGL ES " << version << " context created";
                    break;
                }
            }

            if (context == EGL_NO_CONTEXT)
                throw std::system_error(eglGetError(), eglErrorCategory, "Failed to create EGL context");

            if (!eglMakeCurrent(display, surface, surface, context))
                throw std::system_error(eglGetError(), eglErrorCategory, "Failed to set current EGL context");

            if (!eglSwapInterval(display, newVerticalSync ? 1 : 0))
                throw std::system_error(eglGetError(), eglErrorCategory, "Failed to set EGL frame interval");
#endif

            OGLRenderDevice::init(newWindow,
                                  newSize,
                                  newSampleCount,
                                  newTextureFilter,
                                  newMaxAnisotropy,
                                  newSrgb,
                                  newVerticalSync,
                                  newDepth,
                                  newStencil,
                                  newDebugRenderer);

#if OUZEL_OPENGL_INTERFACE_EGL
            if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
                throw std::system_error(eglGetError(), eglErrorCategory, "Failed to unset EGL context");
#endif

            running = true;
            renderThread = std::thread(&OGLRenderDeviceLinux::main, this);
        }

        std::vector<Size2U> OGLRenderDeviceLinux::getSupportedResolutions() const
        {
            std::vector<Size2U> result;

#if OUZEL_OPENGL_INTERFACE_GLX
            EngineLinux* engineLinux = static_cast<EngineLinux*>(engine);

            int modeCount;
            XF86VidModeModeInfo** modeInfo;

            XF86VidModeGetAllModeLines(engineLinux->getDisplay(), 0, &modeCount, &modeInfo);

            for (int i = 0; i < modeCount; ++i)
                result.push_back(Size2U(static_cast<uint32_t>(modeInfo[i]->hdisplay),
                                        static_cast<uint32_t>(modeInfo[i]->vdisplay)));

            XFree(modeInfo);
#elif OUZEL_OPENGL_INTERFACE_EGL
            // TODO: return screen resolution
#endif

            return result;
        }

        void OGLRenderDeviceLinux::present()
        {
#if OUZEL_OPENGL_INTERFACE_GLX
            EngineLinux* engineLinux = static_cast<EngineLinux*>(engine);
            NativeWindowLinux* windowLinux = static_cast<NativeWindowLinux*>(window->getNativeWindow());

            glXSwapBuffers(engineLinux->getDisplay(), windowLinux->getNativeWindow());
#elif OUZEL_OPENGL_INTERFACE_EGL
            if (eglSwapBuffers(display, surface) != EGL_TRUE)
                throw std::system_error(eglGetError(), eglErrorCategory, "Failed to swap buffers");
#endif
        }

        void OGLRenderDeviceLinux::main()
        {
            setCurrentThreadName("Render");

#if OUZEL_OPENGL_INTERFACE_GLX
            EngineLinux* engineLinux = static_cast<EngineLinux*>(engine);
            NativeWindowLinux* windowLinux = static_cast<NativeWindowLinux*>(window->getNativeWindow());

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
                    engine->log(Log::Level::ERR) << e.what();
                }
            }

#if OUZEL_OPENGL_INTERFACE_EGL
            if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
                throw std::system_error(eglGetError(), eglErrorCategory, "Failed to unset EGL context");
#endif
        }
    } // namespace graphics
} // namespace ouzel

#endif
