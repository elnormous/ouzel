#ifndef FLUXION_GRAPHICS_OGLRENDERDEVICELINUX_HPP
#define FLUXION_GRAPHICS_OGLRENDERDEVICELINUX_HPP

#include "../../../core/Setup.h"

#if defined(__linux__) && !defined(__ANDROID__) && FLUXION_COMPILE_OPENGL

#include "../OGL.h"

#include <atomic>
#include <thread>
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "../OGLRenderDevice.hpp"
#include "../../../thread/Thread.hpp"

namespace fluxion::graphics::opengl::linux
{
    class RenderDevice final: public opengl::RenderDevice
    {
        friend Graphics;
    public:
        RenderDevice(const Settings& settings,
                     core::Window& initWindow);
        ~RenderDevice() override;

        std::vector<math::Size<std::uint32_t, 2>> getSupportedResolutions() const final;
        void start() final;

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

#endif // FLUXION_GRAPHICS_OGLRENDERDEVICELINUX_HPP
