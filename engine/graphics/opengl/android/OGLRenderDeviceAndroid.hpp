#ifndef FLUXION_GRAPHICS_OGLRENDERDEVICEANDROID_HPP
#define FLUXION_GRAPHICS_OGLRENDERDEVICEANDROID_HPP

#include "../../../core/Setup.h"

#if defined(__ANDROID__) && FLUXION_COMPILE_OPENGL

#include <atomic>
#include <thread>
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "../OGLRenderDevice.hpp"
#include "../../../core/Window.hpp"
#include "../../../thread/Thread.hpp"

namespace fluxion::graphics::opengl::android
{
    class RenderDevice final: public opengl::RenderDevice
    {
        friend Graphics;
    public:
        RenderDevice(const Settings& settings,
                     core::Window& initWindow);
        ~RenderDevice() override;

        void start() final;
        void reload();
        void destroy();

    private:
        void present() final;
        void renderMain();

        EGLDisplay display = EGL_NO_DISPLAY;
        EGLSurface surface = EGL_NO_SURFACE;
        EGLContext context = EGL_NO_CONTEXT;

        std::atomic_bool running{false};
        thread::Thread renderThread;
    };
}
#endif

#endif // FLUXION_GRAPHICS_OGLRENDERDEVICEANDROID_HPP
