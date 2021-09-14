// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_OGLRENDERDEVICELINUX_HPP
#define OUZEL_GRAPHICS_OGLRENDERDEVICELINUX_HPP

#include "../../../core/Setup.h"

#if defined(__linux__) && !defined(__ANDROID__) && OUZEL_COMPILE_OPENGL

#include "../OGL.h"

#include <atomic>
#include <thread>
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "../OGLRenderDevice.hpp"
#include "../../../thread/Thread.hpp"

namespace ouzel::graphics::opengl::linux
{
    class RenderDevice final: public opengl::RenderDevice
    {
        friend Graphics;
    public:
        RenderDevice(const Settings& settings,
                     core::Window& initWindow,
                     const std::function<void(const Event&)>& initCallback);
        ~RenderDevice() override;

        std::vector<math::Size<std::uint32_t, 2>> getSupportedResolutions() const final;

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

#endif // OUZEL_GRAPHICS_OGLRENDERDEVICELINUX_HPP
